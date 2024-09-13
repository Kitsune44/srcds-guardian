#include "Stats.h"

#include <numeric>
#include <iostream>

using namespace std;

Stats::Stats(int samples, int rate) : maxSamples(samples), sampleRate(rate) {
    load.resize(maxSamples, 0);

void Stats::reset() {
    //SYSTEM_INFO sysInfo;
    //GetSystemInfo(&sysInfo);
    //numProcessors = sysInfo.dwNumberOfProcessors;
    lastSysCPU = {0};
    lastUserCPU = {0};
        
    GetSystemTimeAsFileTime(&ftime);
    memcpy(&lastCPU, &ftime, sizeof(FILETIME));

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess != NULL) {
        GetProcessTimes(hProcess, &ftime, &ftime, &fsys, &fuser);
        CloseHandle(hProcess);

        memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
        memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
    } 

    load.clear();
    load.resize(samples, 0);
}

void Stats::setPid(int processId) {
    pid = processId;
}

int Stats::getCpu() {
    ULARGE_INTEGER now, sys = {0}, user = {0};
    double percent = 0.0;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess != NULL) {
        GetProcessTimes(hProcess, &ftime, &ftime, &fsys, &fuser);
        CloseHandle(hProcess);

        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
    }

    percent = (double)(sys.QuadPart - lastSysCPU.QuadPart) +
        (user.QuadPart - lastUserCPU.QuadPart);
    percent /= now.QuadPart - lastCPU.QuadPart;
    percent *= 100;
    //percent /= numProcessors;
    
    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;

    if (load.size() > maxSamples) load.erase(load.begin());
    if (percent >= 0) {
        load.emplace_back(ceil(percent));
    } else {
        percent = 0;
    }

    return (int)round(percent);
}

int Stats::getLoad() {
    const int avg = (int)ceil(accumulate(load.begin(), load.end(), 0) / load.size());
    return avg > 0 ? avg : 0;
}


int Stats::getMemory() {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T used = pmc.PrivateUsage;

    return (int)used;
}

int Stats::getMemorySelf() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T used = pmc.PrivateUsage;

    return (int)used;
}

int Stats::getNumSamples() {
    return (int)load.size();
}
