#include <Windows.h>
#include <vector>
#include "Shlwapi.h"
#include "psapi.h"

#pragma once
class Stats
{
    public:
        Stats(int samples = 30, int rate = 1000); 

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
        int sampleRate; // [ms]
        int maxSamples;
        std::vector<int> load;
};
