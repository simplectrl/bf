#pragma once
#include <math.h>
#include <vector>
#include <string>

using uchar = unsigned char;

class Slider
{
public:
    Slider(const int trigger);
    bool      inc               ();
    const int get               ();
    void      set               (const int val);
    void      attachLeftReferece(Slider* ref);

private:
    int       m_val;
    Slider*   m_leftRef;
    const int m_trigger;
};

class BlackBox
{
public:
    BlackBox(const int maxLen,
        const std::vector<uchar>& preset = {},
        const std::vector<uchar>& until = {});

    const std::string next();

private:
    void presetSliderBox      (const std::vector<uchar>& preset);
    void setLimit             (const std::vector<uchar>& until);
    void makeSlidersFriendly  ();

private:
    const int           m_arrsize;
    std::vector<Slider> m_sliderBox;
    int                 m_level;
    const int           m_maxLevel;
    std::string         m_limitArray;

    bool                m_hasLimit         = false;
    bool                m_isLimitTriggered = false;
};

inline int GetCombinationCount(const int elms, const int maxLen)
{
    int ret = 0;
    for (int i = 1; i <= maxLen; ++i)
    {
        ret += pow(elms, i);
    }

    return ret;
}
