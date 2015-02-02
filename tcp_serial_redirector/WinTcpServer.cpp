#include "WinTcpServer.h"

#include "WinSocket.h"

static DWORD WINAPI WinTcpServerSericeThread(LPVOID arg);

WinTcpServer::WinTcpServer(int port)
    : port_(port)
{
}

void WinTcpServer::Start()
{
    serverSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket_ == INVALID_SOCKET) {
        
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;

    int ret = ::bind(serverSocket_, (SOCKADDR*)&addr, sizeof(addr));

    if (ret == SOCKET_ERROR) {
    }

    ret = ::listen(serverSocket_, 5);

    if (ret == SOCKET_ERROR) {

    }

    DWORD threadId;
    HANDLE thread = ::CreateThread(NULL, 0, WinTcpServerSericeThread, this, 0, &threadId);
    ::CloseHandle(thread);
}

void WinTcpServer::Stop()
{
}

void WinTcpServer::ServerLoop()
{
    while (TRUE) {
        sockaddr_in addr;
        int addrlen = sizeof(addr);
        SOCKET clientSocket = ::accept(serverSocket_, (SOCKADDR*)&addr, &addrlen);

        if (clientSocket == INVALID_SOCKET) {
            OnError();
        } else {
            WinSocket * newSocket = new WinSocket(clientSocket, addr);
            OnNewConnection(newSocket);
        }
    }
}

void WinTcpServer::OnError()
{
    DWORD err = ::GetLastError();
    //Log error
}

void WinTcpServer::OnNewConnection(WinSocket * Socket)
{
    Socket->Shutdown();
    delete Socket;
}

DWORD WINAPI WinTcpServerSericeThread(LPVOID arg)
{
    WinTcpServer* server = reinterpret_cast<WinTcpServer*>(arg);
    server->ServerLoop();
    return 0L;
}