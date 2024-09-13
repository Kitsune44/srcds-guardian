#include "Stats.h"

#include <numeric>
#include <iostream>

using namespace std;

void Stats::setPid(int processId) {
    pid = processId;
}

void Stats::reset() {
    SYSTEM_INFO sysInfo;
    FILETIME ftime, fsys, fuser;

    GetSystemInfo(&sysInfo);
    numProcessors = sysInfo.dwNumberOfProcessors;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&lastCPU, &ftime, sizeof(FILETIME));

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess != NULL) {
        GetProcessTimes(hProcess, &ftime, &ftime, &fsys, &fuser);
        CloseHandle(hProcess);

        memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
        memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
    } else {
        memset(&lastSysCPU, 0, sizeof(lastSysCPU));
        memset(&lastUserCPU, 0, sizeof(lastUserCPU));
    }

    load.clear();
    load.resize(samples, 0);
}


int Stats::getCpu() {
    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    GetProcessTimes(hProcess, &ftime, &ftime, &fsys, &fuser);
    CloseHandle(hProcess);

    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));

    double cpuTimeDiff = now.QuadPart - lastCPU.QuadPart;    
    if (cpuTimeDiff > 0) {
        percent = (double)(sys.QuadPart - lastSysCPU.QuadPart) +
            (user.QuadPart - lastUserCPU.QuadPart);
        percent /= cpuTimeDiff;
        percent *= 100;
    } else {
        percent = 0;
    }
    //percent /= numProcessors;
    
    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;

    if (load.size() > samples) load.erase(load.begin());
    if (percent >= 0 && percent <= 100) {
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
