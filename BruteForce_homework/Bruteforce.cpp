#include "Bruteforce.h"
#include "DecryptAes.h"

Bruteforce::Bruteforce()
    : m_stopCracking(false),
    m_threadsCounter(0),
    m_isPassFound(false),
    m_workData{ {} },
    m_pWorkData(&m_workData)
{
    m_workData.passwordsCounter = 0;
}

Bruteforce::~Bruteforce()
{
    if (m_threadsCounter > 0)
    {
        m_stopCracking = true;
    }
}

void Bruteforce::Worker(BlackBox bb, const std::vector<uchar>& chipText,
    const std::vector<uchar>& targetHash, const fs::path& outPath)
{
    ++m_threadsCounter;

    uchar key[EVP_MAX_KEY_LENGTH];
    uchar iv[EVP_MAX_IV_LENGTH];

    std::string pass;
    std::vector<uchar> plainText;
    std::vector<uchar> newHash;

    while (!m_stopCracking)
    {
        pass = bb.next();
        if (pass.empty())
        {
            break;
        }
        m_pWorkData->passwordsCounter++;

        utl::PasswordToKeyT(pass, key, iv);
        if (!DecryptAesT(chipText, plainText, key, iv))
        {
            continue;
        }
        utl::CalculateHash(plainText, newHash);

        if (utl::IsHashesEqual(targetHash, newHash))
        {
            std::lock_guard lock(m_pWorkData->mutex);

            m_stopCracking = true;
            m_isPassFound = true;
            m_pWorkData -> foundedPassword = pass;

            m_pWorkData->outputFile = outPath.string() + "\\pass_" + pass;
            utl::WriteFile(m_pWorkData->outputFile, plainText);
        }
    }

    --m_threadsCounter;
};

void Bruteforce::DisplayProgress()
{
    do
    {   
        std::this_thread::sleep_for(100ms);

        std::printf("\r  checked passpords: %d", static_cast<long>(m_pWorkData->passwordsCounter));         
        std::cout.flush();

    } while (m_threadsCounter > 0);
    std::cout << std::endl;
}

void Bruteforce::CrackFile(const fs::path& filePath)
{
    if (!utl::IsReularFile(filePath))
    {
        std::cerr << "bruteforce: ERROR -> invalid input file\n";
        return;
    }

    fs::path outPath = fs::current_path().append("bf-aes-out");
    utl::CreatePathIfNotExists(outPath);

    std::vector<uchar> chipherText;
    utl::ReadFile(filePath.string(), chipherText);

    std::vector<uchar> targetHash(
        utl::GetAppendedSHA256Hash(chipherText));
    utl::RemoveAppendedSHA256Hash(chipherText);

    std::cout << "=======================================================\n"
        << "bruteforce started\n";

    auto begin = std::chrono::high_resolution_clock::now();

    std::thread([&]() {Worker(BlackBox(4, {}, { '9','0','0','0' }), chipherText, targetHash, outPath); }).detach();
    std::thread([&]() {Worker(BlackBox(4, { 'a','0','0','0' }, { 'j','0','0','0' }), chipherText, targetHash, outPath); }).detach();
    std::thread([&]() {Worker(BlackBox(4, { 'j','0','0','0' }, { 't','0','0','0' }), chipherText, targetHash, outPath); }).detach();
    std::thread([&]() {Worker(BlackBox(4, { 't','0','0','0' }, {}), chipherText, targetHash, outPath); }).detach();

    std::thread([this]() {DisplayProgress(); }).join();

    if (m_isPassFound)
    {
        std::cout << "  pass found -> " << m_workData.foundedPassword << "\n"
            << "  check output at: " << m_workData.outputFile << "\n";
    }
    else
    {
        std::cout << "  pass NOT found :-( \n";
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "elapsed time: " << elapsed << "ms\n"
        << "=======================================================\n\n";

    ResetWorkData();
}

void Bruteforce::ResetWorkData()
{
    while (m_threadsCounter > 0)
    {
        m_stopCracking = true;
        std::this_thread::sleep_for(30ms);
    }

    m_stopCracking = false;
    m_isPassFound = false;
    m_workData.passwordsCounter = 0;
    m_workData.foundedPassword.clear();
    m_workData.outputFile.clear();
}