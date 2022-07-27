#include "Utils.h"
#include "DecryptAes.h"

class Dictionary
{
public:
    Dictionary()
        : m_isPassFound      ( false ),
          m_passwordsCounter ( 0 )
    {}

    void CrackFile(const fs::path& filePath);

private:
    bool IsDictionaryHasRightExtension(const fs::path& dictionaryPath);

    void Worker(const fs::path& dictionaryPath, const std::vector<uchar>& chipText,
        const std::vector<uchar>& targetHash, const fs::path& outPath);

private:
    bool              m_isPassFound;
    long long         m_passwordsCounter;    
};