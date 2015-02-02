#pragma once

#include <string>
#include <exception>

#include <WinSock2.h>
#include <Windows.h>

class WinSocket
{
public:
    WinSocket(SOCKET rawSock, struct sockaddr_in addr);

    VOID Shutdown();

    BOOL WaitForData(long timeout);

    DWORD Write(LPCVOID src, DWORD nbytes);
    DWORD Read(LPVOID dst, DWORD nbytes);

private:
    struct sockaddr_in addr_;
    SOCKET rawSock_;
};

class WinSocketException : public std::runtime_error
{
public:
    WinSocketException(const std::string & reason);

    static void Throw(const std::string & reason);

private:
    std::string errStr_;
};