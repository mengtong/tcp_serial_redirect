#pragma once

#include <Windows.h>
#include <list>

class WinSerial;

class SerialPool
{
public:
    SerialPool();

    void Store(WinSerial * ser);

    WinSerial * GetSerial(LPCTSTR ComStr);

private:
    std::list<WinSerial *> pool_;
};