#pragma once
#include "stdafx.h"
#include "Inventory.h"

uchar key [EVP_MAX_KEY_LENGTH];
uchar iv  [EVP_MAX_IV_LENGTH];

bool EncryptAes(const std::vector<uchar> plainText, std::vector<uchar>& chipherText)
{
    CTX ctx;
    if (!EVP_EncryptInit_ex(ctx.get(), EVP_aes_128_cbc(), NULL, key, iv))
    {
        return false;
    }

    std::vector<uchar> chipherTextBuf(plainText.size() + AES_BLOCK_SIZE);
    int chipherTextSize = 0;
    if (!EVP_EncryptUpdate(ctx.get(), chipherTextBuf.data(), &chipherTextSize, plainText.data(), plainText.size())) {
        return false;
    }

    int lastPartLen = 0;
    if (!EVP_EncryptFinal_ex(ctx.get(), chipherTextBuf.data() + chipherTextSize, &lastPartLen)) {
        return false;
    }
    chipherTextSize += lastPartLen;
    chipherTextBuf.erase(chipherTextBuf.begin() + chipherTextSize, chipherTextBuf.end());

    chipherText.swap(chipherTextBuf);
    return true;
}


void Encrypt(const std::string& inFilePath, const std::string& outFilePath)
{
    std::vector<uchar> plainText;
    ReadFile(inFilePath, plainText);

    std::vector<uchar> hash;
    CalculateHash(plainText, hash);

    std::vector<uchar> chipherText;
    EncryptAes(plainText, chipherText);

    WriteFile(outFilePath, chipherText);

    AppendToFile(outFilePath, hash);
}
