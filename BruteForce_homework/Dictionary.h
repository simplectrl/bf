#include "Utils.h"
#include "DecryptAes.h"

namespace Dictionary
{   
    struct WorkData
    {
        bool              isPassFound;
        std::atomic<long> passwordsCounter;
    };

    bool IsDictionaryHasRightExtension(const fs::path& dictionaryPath)
    {
        return (fs::is_regular_file(dictionaryPath) && dictionaryPath.extension() == "txt"); 
    }

    void Worker(const fs::path& dictionaryPath, const std::vector<uchar>& chipText,
        const std::vector<uchar>& targetHash, const fs::path& outPath, WorkData* wd)
    {   
        uchar key[EVP_MAX_KEY_LENGTH];
        uchar iv[EVP_MAX_IV_LENGTH];

        std::vector<uchar> newHash;
        std::vector<uchar> plainText;

        std::ifstream input(dictionaryPath);
        for (std::string pass; getline(input, pass); )
        {
            if (pass.empty())
            {
                break;
            }
            ++wd->passwordsCounter;

            utl::PasswordToKeyT(pass, key, iv);
            if (!DecryptAesT(chipText, plainText, key, iv))
            {
                continue;
            }
            utl::CalculateHash(plainText, newHash);

            if (utl::IsHashesEqual(targetHash, newHash))
            {  
                wd->isPassFound = true;
                std::cout << "  pass found -> " << pass << "\n"
                          << "  checked passpords: " << wd->passwordsCounter << "\n";

                std::string outFileName(outPath.string() + "\\pass_" + pass);
                utl::WriteFile(outFileName, plainText);

                std::cout << "  check output at: " << outFileName << "\n";
                break;
            }
        }
    }

    void CrackFile(const fs::path& filePath)
    {
        if (!utl::IsReularFile(filePath))
        {
            std::cerr << "parceDictionary: ERROR -> invalid input file\n";
            return;
        }

        WorkData workData;
        workData.isPassFound = false;
        workData.passwordsCounter = 0;

        fs::path outPath = fs::current_path().append("dctnry-aes-out");
        utl::CreatePathIfNotExists(outPath);

        std::vector<uchar> chipherText;
        utl::ReadFile(filePath.string(), chipherText);

        std::vector<uchar> targetHash(
            utl::GetAppendedSHA256Hash(chipherText));
        utl::RemoveAppendedSHA256Hash(chipherText);

        std::cout << "=======================================================\n"
            << "dictionary parser started\n";
        auto begin = std::chrono::high_resolution_clock::now();

        for (const auto& dir_entry : fs::directory_iterator{ "dictionary" })
        {
            if (workData.isPassFound) break;

            if (IsDictionaryHasRightExtension(dir_entry.path()))
            {
                std::cout << "  can't process with path: " << dir_entry.path() << '\n';
                return;
            }
            std::cout << "  current dictionary: " << dir_entry.path().filename() << '\n';
            Worker(dir_entry.path(), chipherText, targetHash, outPath, &workData);

        }

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        std::cout << "elapsed time: " << elapsed << "ms\n"
            << "=======================================================\n\n";
    }
}