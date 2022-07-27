#pragma once
#include <cwctype>
#include "stdafx.h"

inline int isValidDataA(std::vector<uchar>& data)
{
    unsigned int i, trigger;
    unsigned int bad = 0;

    /* Consider the decrypted data as invalid if there is more than 10% of not printable characters */
    trigger = data.size() / 10;

    /* Count the number of not printable characters */
    for (auto& ch : data)
    {
        if ((!std::isprint(ch)) && !std::isspace(ch))
        {
            bad++;
            if (bad > trigger)
                return(0);
        }
    }

    return(1);
}


inline bool isValidDataW(std::vector<uchar>& data)
{
    unsigned int trigger;
    unsigned int bad = 0;

    trigger = (data.size() / 2) / 10;

    //just simple convert uchar to wchar
    wchar_t* begin = reinterpret_cast<wchar_t*>(data.data());
    wchar_t* end   = reinterpret_cast<wchar_t*>(data.data() + data.size());
    for (auto it = begin; it != end; ++it)
    {
        if ((!std::iswprint(*it)) && !std::iswspace(*it))
        {
            bad++;
            if (bad > trigger)
                return false;
        }
    }

    return true;
}

inline int isValidDataAK(std::vector<uchar>& data)
{
    unsigned int i, trigger;
    unsigned char c;
    unsigned int bad = 0;

    trigger = (data.size() / 2) / 10;

    for (i = 0; i < data.size(); i++)
    {
        c = data[i];

        //printable kyrilic
        if (c == 208 || c == 209)
        {
            if (data[i + 1] >= 128 && data[i + 1] <= 191)
                ++i;
            continue;
        }

        if ((!std::isprint(c)) && !std::isspace(c))
        {
            bad++;
            if (bad > trigger)
                return(0);
        }
    }

    return(1);
}
