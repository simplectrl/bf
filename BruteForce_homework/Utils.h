#pragma once
#include "stdafx.h"

namespace utl
{
    inline bool ReadFile(const std::string& filePath, std::vector<uchar>& buf)
    {
        std::basic_fstream<uchar> fileStream(filePath, std::ios::binary | std::fstream::in);
        if (!fileStream.is_open())
        {
            std::cerr << "ReadFile: can't open file -> " << filePath << "\n";
            return false;
        }

        buf.insert(buf.begin(), std::istreambuf_iterator<uchar>(fileStream), std::istreambuf_iterator<uchar>());
        return true;
    }

    inline void WriteFile(const std::string& filePath, const std::vector<uchar>& buf)
    {
        std::basic_ofstream<uchar> fileStream(filePath, std::ios::binary);
        fileStream.write(buf.data(), buf.size());
    }

    inline void AppendToFile(const std::string& filePath, const std::vector<uchar>& buf)
    {
        std::basic_ofstream<uchar> fileStream(filePath, std::ios::binary | std::ios::app);
        fileStream.write(buf.data(), buf.size());
    }

    inline bool PasswordToKeyT(std::string& password, uchar* key, uchar* iv)
    {
        const uchar* salt = nullptr;
        if (!EVP_BytesToKey(EVP_aes_128_cbc(), EVP_md5(), salt,
            reinterpret_cast<uchar*>(password.data()),
            password.size(), 1, key, iv))
        {
            return false;
        }

        return true;
    }

    inline void CalculateHash(const std::vector<uchar>& data, std::vector<uchar>& hash)
    {
        std::vector<uchar> hashTmp(SHA256_DIGEST_LENGTH);

        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, data.data(), data.size());
        SHA256_Final(hashTmp.data(), &sha256);

        hash.swap(hashTmp);
    }

    inline std::vector<uchar> GetAppendedSHA256Hash(const std::vector<uchar>& data)
    {
        return std::vector<uchar>(data.end() - SHA256_DIGEST_LENGTH, data.end());
    }

    inline void RemoveAppendedSHA256Hash(std::vector<uchar>& data)
    {
        data.erase(data.end() - SHA256_DIGEST_LENGTH, data.end());
    }

    inline void CreatePathIfNotExists(const fs::path& path)
    {
        if (!fs::exists(path))
        {
            fs::create_directory(path);
        }
    }

    inline bool IsReularFile(const fs::path& filePath)
    {
        return (fs::exists(filePath) && fs::is_regular_file(filePath));
    }

    inline bool IsHashesEqual(const std::vector<uchar>& l, const std::vector<uchar>& r)
    {
        return std::equal(l.begin(), l.end(), r.begin(), r.end());
    }

    class CTX
    {
        EVP_CIPHER_CTX* m_ctx;

    public:
        CTX()
            : m_ctx(EVP_CIPHER_CTX_new())
        {}

        ~CTX()
        {
            EVP_CIPHER_CTX_free(m_ctx);
        }

        EVP_CIPHER_CTX* get()
        {
            return m_ctx;
        }
    };
}
