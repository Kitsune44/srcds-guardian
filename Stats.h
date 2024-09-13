#include <Windows.h>
#include <vector>
#include "Shlwapi.h"
#include "psapi.h"

#pragma once
class Stats
{
    public:
        void initStats(int samples, int rate);
        void reset();
        void setPid(int pid);
        int getCpu();
        int getLoad();
        int getMemory();
        int getMemorySelf();
        int getNumSamples();
    
    private:
        ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
        int pid;
        int numProcessors;
        int sampleRate; // [ms]
        int maxSamples;
        std::vector<int> load;
};
