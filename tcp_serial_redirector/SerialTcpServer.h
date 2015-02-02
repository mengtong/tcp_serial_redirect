#pragma once

#include "WinTcpServer.h"

class WinSerial;

class SerialTcpServer : public WinTcpServer
{
public:
    SerialTcpServer(WinSerial * serial, int tcpport);

    virtual void OnNewConnection(WinSocket * Socket);

private:
    WinSerial * serial_;
};