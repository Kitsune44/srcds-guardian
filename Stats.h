#include <Windows.h>
#include <vector>
#include "Shlwapi.h"
#include "psapi.h"

#pragma once
class Stats
{
    public:
        Stats(int maxSample = 30, int sampleRate = 1000); 

        void reset();
        void setPid(int pid);
        int getCpu();
        int getLoad();
        int getMemory();
        int getMemorySelf();
        int getNumSamples();
    
    private:
        ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
        FILETIME ftime, fsys, fuser;
        int pid = -1;
        int samples;
        int rate; // [ms]
        std::vector<int> load;
};
