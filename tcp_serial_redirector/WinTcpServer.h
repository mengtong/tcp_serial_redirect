#pragma once

#include <WinSock2.h>

class WinSocket;

class WinTcpServer
{
public:
    WinTcpServer(int port);
    
    void Start();
    void Stop();
    void ServerLoop();

    virtual void OnError();
    virtual void OnNewConnection(WinSocket * Socket);
    
private:
    int port_;
    SOCKET serverSocket_;
};