#pragma once
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>

using uchar = unsigned char;

//static const std::vector<uchar> charset
//{ '0','1','2','3','4','5','6','7','8','9',
//
// 'a','b','c','d','e','f','g','h','i','j',
// 'k','l','m','n','o','p','q','r','s','t',
// 'u','v','w','x','y','z' };

static const std::vector<uchar> charset
{ '0','1','2','3','4','5','6','7','8','9',

  'A','B','C','D','E','F','G','H','I','J',
  'K','L','M','N','O','P','Q','R','S','T',
  'U','V','W','X','Y','Z',

  'a','b','c','d','e','f','g','h','i','j',
  'k','l','m','n','o','p','q','r','s','t',
  'u','v','w','x','y','z' };


class BlackBox
{
public:
    BlackBox(const int maxLen,
             const std::vector<uchar>& preset = {},
             const std::vector<uchar>& until = {})

        : m_arrsize   (charset.size()),
          m_sliderBox (maxLen, Slider(m_arrsize)),
          m_level     (0),
          m_maxLevel  (maxLen)
    {
        makeSlidersFriendly();
        presetSliderBox(preset);
        setLimit(until);
    }

    const std::string next()
    {
        std::string ret;
        if (m_isLimitTriggered) return ret;

        if (m_sliderBox[m_level].inc())
        {
            m_level++;
            if (m_level > m_maxLevel - 1) return ret;
            else m_sliderBox[m_level].inc();
        }

        for (auto& el : m_sliderBox)
        {
            if (el.get() == -1) break;
            ret += charset[el.get()];
        }

        if (m_hasLimit)
        {
            if (ret == m_limitArray)
            {
                m_isLimitTriggered = true;
            }
        }

        return std::move(ret);
    }

private:
    struct Slider
    {
        Slider(const int trigger)
            : m_val(-1),
            m_leftRef(nullptr),
            m_trigger(trigger)

        {}

        bool inc()
        {
            if (++m_val == m_trigger)
            {
                m_val = 0;
                if (m_leftRef != nullptr)
                {
                    return m_leftRef->inc();
                }
                return true;
            }
            return false;
        }

        const int get()
        {
            return m_val;
        }

        void set(const int val)
        {
            m_val = val;
        }

        void attachLeftReferece(Slider* ref)
        {
            m_leftRef = ref;
        }

    private:
        int       m_val;
        Slider* m_leftRef;
        const int m_trigger;
    };

    void presetSliderBox(const std::vector<uchar>& preset)
    {
        if (preset.empty())
        {
            return;
        }

        for (int i = 0; i < preset.size(); ++i)
        {
            if (i == m_maxLevel) break;

            for (int j = 0; j < m_arrsize; ++j)
            {
                if (charset[j] == preset[i])
                {
                    m_sliderBox[i].set(j);
                    if (i>0) m_level++;
                }
            }
        }
    }

    void setLimit(const std::vector<uchar>& until)
    {
        if (until.empty())
        {
            return;
        }

        for (int i = 0; i < until.size(); ++i)
        {
            if (i == m_maxLevel) break;
            m_limitArray += until[i];
        }

        if (!m_limitArray.empty()) m_hasLimit = true;
    }

    void makeSlidersFriendly()
    {
        for (int i = 1; i < m_maxLevel; ++i)
        {
            m_sliderBox[i].attachLeftReferece(&m_sliderBox[i - 1]);
        }
    }


private:
    const int           m_arrsize;
    std::vector<Slider> m_sliderBox;
    int                 m_level;
    const int           m_maxLevel;
    std::string         m_limitArray;

    bool                m_hasLimit = false;
    bool                m_isLimitTriggered = false;
};

int GetCombinationCount(const int elms, const int maxLen)
{
    int ret = 0;
    for (int i = 1; i <= maxLen; ++i)
    {
        ret += pow(elms, i);
    }

    return ret;
}
