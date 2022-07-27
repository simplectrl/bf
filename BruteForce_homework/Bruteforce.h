#pragma once
#include "BlackBox.h"
#include "Utils.h"

class Bruteforce
{
    struct WorkData
    {
        std::mutex mutex;
        std::atomic<long>    passwordsCounter;
        std::string          foundedPassword;
        std::string          outputFile;
    };

public:
    Bruteforce();
    ~Bruteforce();

    void CrackFile(const fs::path& filePath);

private:
    void Worker(BlackBox bb, const std::vector<uchar>& chipText,
        const std::vector<uchar>& targetHash, const fs::path& outPath);

    void DisplayProgress();
    void ResetWorkData();

private:
    std::atomic<bool>    m_stopCracking;
    std::atomic<int>     m_threadsCounter;
    std::atomic<bool>    m_isPassFound;

    WorkData             m_workData;
    WorkData*            m_pWorkData;
};