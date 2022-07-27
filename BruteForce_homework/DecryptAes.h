#pragma once
#include "stdafx.h"
#include "Utils.h"

inline bool DecryptAesT(const std::vector<uchar> in, std::vector<uchar>& out, uchar* key, uchar* iv)
{
    utl::CTX ctx;
    if (!EVP_DecryptInit_ex(ctx.get(), EVP_aes_128_cbc(), NULL, key, iv))
    {
        return false;
    }   

    std::vector<uchar> outCharBuff(in.size()+AES_BLOCK_SIZE);
    int total_out_len = 0;
    int current_out_len = 0;

    if (!EVP_DecryptUpdate(ctx.get(), outCharBuff.data(), &current_out_len, in.data(), in.size())) 
    {
        return false;
    }
    total_out_len += current_out_len;    
    
    //maybe useful on future
    //EVP_CIPHER_CTX_set_padding(ctx.get(), 0);
    if (!EVP_DecryptFinal_ex(ctx.get(), outCharBuff.data() + total_out_len, &current_out_len))
    {
        //maybe useful on future
        //ERR_print_errors_fp(stderr);
        return false;
    }
    total_out_len += current_out_len;

    outCharBuff.erase(outCharBuff.begin() + total_out_len, outCharBuff.end());
    out.swap(outCharBuff);

    return true;
}
