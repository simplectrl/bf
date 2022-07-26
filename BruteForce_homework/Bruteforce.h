#include "BlackBox.h"
#include "Utils.h"
#include "DecryptAes.h"

namespace Bruteforce
{
    struct WorkData
    {
        std::mutex mutex;

        std::atomic<bool>    stopCracking;
        std::atomic<int>     threadsCounter;
        std::atomic<long>    passwordsCounter;

        bool isPassFound;

        std::string foundedPassword;
        std::string outputFile;
    };

    class AntiZombie
    {
        std::atomic<bool>* m_bTrigger;

    public:
        explicit AntiZombie(std::atomic<bool>* trigger) 
            : m_bTrigger (trigger)
        {
            *m_bTrigger = false;
        }

        ~AntiZombie()
        {
            *m_bTrigger = true;
        }
    };

    void Worker(BlackBox bb, const std::vector<uchar>& chipText,
        const std::vector<uchar>& targetHash, const fs::path& outPath, WorkData* wd)
    {
        ++wd->threadsCounter;

        uchar key[EVP_MAX_KEY_LENGTH];
        uchar iv[EVP_MAX_IV_LENGTH];

        std::string pass;
        std::vector<uchar> plainText;
        std::vector<uchar> newHash;

        while (!wd->stopCracking)
        {
            pass = bb.next();
            if (pass.empty())
            {
                break;
            }
            wd->passwordsCounter++;

            utl::PasswordToKeyT(pass, key, iv);
            if (!DecryptAesT(chipText, plainText, key, iv))
            {
                continue;
            }
            utl::CalculateHash(plainText, newHash);

            if (utl::IsHashesEqual(targetHash, newHash))
            {     
                std::lock_guard lock(wd->mutex);

                wd->stopCracking = true;
                wd->isPassFound = true;
                wd->foundedPassword = pass;

                wd->outputFile = outPath.string() + "\\pass_" + wd->foundedPassword;
                utl::WriteFile(wd->outputFile, plainText);
            }
        }

        --wd->threadsCounter;
    };

    void DisplayProgress(WorkData* wd)
    {   
        do 
        {
            std::printf("\r  checked passpords: %d", static_cast<long>(wd->passwordsCounter));
            std::this_thread::sleep_for(100ms);

        } while (wd->threadsCounter > 0 );
        std::cout << std::endl;                      
    }

    void CrackFile(const fs::path& filePath)
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
            utl::GetAppendedSHA256Hash(chipherText) );
        utl::RemoveAppendedSHA256Hash(chipherText);

        std::cout << "=======================================================\n"
            << "bruteforce started\n";

        struct WorkData workData;
        workData.stopCracking     = false;
        workData.threadsCounter   = 0;
        workData.passwordsCounter = 0;
        workData.isPassFound      = false;

        AntiZombie zombieKiller(&workData.stopCracking);
        auto begin = std::chrono::high_resolution_clock::now();

        std::thread([&]() {Worker(BlackBox(4, {}, { '9','0','0','0' }), chipherText, targetHash, outPath, &workData); }).detach();
        std::thread([&]() {Worker(BlackBox(4, { 'a','0','0','0' }, { 'j','0','0','0' }), chipherText, targetHash, outPath, &workData); }).detach();
        std::thread([&]() {Worker(BlackBox(4, { 'j','0','0','0' }, { 't','0','0','0' }), chipherText, targetHash, outPath, &workData); }).detach();
        std::thread([&]() {Worker(BlackBox(4, { 't','0','0','0' }, {}), chipherText, targetHash, outPath, &workData); }).detach();
        
        std::thread([&workData]() {DisplayProgress(&workData); }).join();
            
        if (workData.isPassFound)
        {
            std::cout << "  pass found -> " << workData.foundedPassword << "\n"
                << "  check output at: " << workData.outputFile << "\n";
        }
        else
        {
            std::cout << "  pass NOT found :-( \n";
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        std::cout << "elapsed time: " << elapsed << "ms\n"
            << "=======================================================\n\n";
    }
}