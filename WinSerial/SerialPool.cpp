#include "SerialPool.h"
#include "WinSerial.h"

#include <Shlwapi.h>
#include <algorithm>

struct WinSerialMatcher {
    WinSerialMatcher(LPCTSTR ComStr) : ComStr_(ComStr) {}

    bool operator()(WinSerial * ser) {        
        return StrCmp(ComStr_, ser->Port()) == 0;
    }

    LPCTSTR ComStr_;
};

SerialPool::SerialPool()
{
}

void SerialPool::Store(WinSerial * ser)
{
    pool_.push_back(ser);
}

WinSerial * SerialPool::GetSerial(LPCTSTR ComStr)
{
    auto pos = std::find_if(pool_.begin(), pool_.end(), WinSerialMatcher(ComStr));
    return pos == pool_.end() ? NULL : *pos;
}