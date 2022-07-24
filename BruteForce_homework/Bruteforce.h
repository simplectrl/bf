#include "BlackBox.h"
#include "Utils.h"
#include "DecryptAes.h"

namespace Bruteforce
{
    static std::mutex mutex;

    static std::atomic<bool>    stopCracking;
    static std::atomic<int>     threadsCounter   = 0;
    static std::atomic<long>    passwordsCounter = 0;
    
    static bool isPassFound = false;

    std::string foundedPassword;
    std::string outputFile;

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
        const std::vector<uchar>& targetHash, const fs::path& outPath)
    {
        ++threadsCounter;

        uchar key[EVP_MAX_KEY_LENGTH];
        uchar iv[EVP_MAX_IV_LENGTH];

        std::string pass;
        std::vector<uchar> plainText;
        std::vector<uchar> newHash;

        while (!stopCracking)
        {
            pass = bb.next();
            if (pass.empty())
            {
                break;
            }
            passwordsCounter++;

            utl::PasswordToKeyT(pass, key, iv);
            if (!DecryptAesT(chipText, plainText, key, iv))
            {
                continue;
            }
            utl::CalculateHash(plainText, newHash);

            if (utl::IsHashesEqual(targetHash, newHash))
            {     
                std::lock_guard lock(mutex);

                stopCracking = true;
                isPassFound = true;
                foundedPassword = pass;

                outputFile = outPath.string() + "\\pass_" + foundedPassword;
                utl::WriteFile(outputFile, plainText);
            }
        }

        --threadsCounter;
    };

    void DisplayProgress()
    {   
        do 
        {
            std::printf("\r  checked passpords: %d", static_cast<long>(passwordsCounter));
            std::this_thread::sleep_for(100ms);

        } while ( threadsCounter > 0 );
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

        AntiZombie zombieKiller(&stopCracking);
        auto begin = std::chrono::high_resolution_clock::now();

        std::thread(Worker, BlackBox(4, {}, { '9','0','0','0' }),                  chipherText, targetHash, outPath).detach();
        std::thread(Worker, BlackBox(4, { 'a','0','0','0' }, { 'j','0','0','0' }), chipherText, targetHash, outPath).detach();
        std::thread(Worker, BlackBox(4, { 'j','0','0','0' }, { 't','0','0','0' }), chipherText, targetHash, outPath).detach();
        std::thread(Worker, BlackBox(4, { 't','0','0','0' }, {}),                  chipherText, targetHash, outPath).detach();

        std::thread(DisplayProgress).join();
            
        if (isPassFound)
        {
            std::cout << "  pass found -> " << foundedPassword << "\n"
                << "  check output at: " << outputFile << "\n";            
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