#include "WinSocket.h"

WinSocket::WinSocket(SOCKET rawSock, struct sockaddr_in addr)
    : rawSock_(rawSock)
    , addr_(addr)
{
}

VOID WinSocket::Shutdown()
{
    if (rawSock_ != INVALID_SOCKET) {
        int ret = ::shutdown(rawSock_, SD_BOTH);
        if (ret == SOCKET_ERROR) {
            WinSocketException::Throw("shutdown@WinSocket::Shutdown");
        }
        ret = closesocket(rawSock_);
        if (ret == SOCKET_ERROR) {
            WinSocketException::Throw("closesocket@WinSocket::Shutdown");
        }

        rawSock_ = INVALID_SOCKET;
    }
}

BOOL WinSocket::WaitForData(long timeout)
{
    struct fd_set fdset;
    FD_ZERO (&fdset);
    FD_SET  (rawSock_, &fdset);

    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec= (timeout % 1000) * 1000;

    return ::select(0, &fdset, NULL, NULL, &tv) > 0;
}

DWORD WinSocket::Write(LPCVOID src, DWORD nbytes)
{
    int written = ::send(rawSock_, reinterpret_cast<const char*>(src), nbytes, 0);

    if (written == SOCKET_ERROR) {
        WinSocketException::Throw("send@WinSocket::Write");        
    }

    return written;
}

DWORD WinSocket::Read(LPVOID dst, DWORD nbytes)
{ 
    int readed = ::recv(rawSock_, reinterpret_cast<char*>(dst), nbytes, 0);
     
    //From MSDN
    //If no error occurs, recv returns the number of bytes received and the buffer 
    //pointed to by the buf parameter will contain this data received. 
    //If the connection has been gracefully closed, the return value is zero. 
    if (readed == SOCKET_ERROR || readed == 0) {
        WinSocketException::Throw("recv@WinSocket::Read");
    }
    if (readed == 0) {
        WinSocketException::Throw("recv@WinSocket::Read, \
            the conncetion has been gracefully closed.");
    }
    return readed;
}

#include <sstream>

WinSocketException::WinSocketException(const std::string & msg)
    : std::runtime_error(msg)
{
}

void WinSocketException::Throw(const std::string & reason)
{
    std::stringstream ss;
    ss << "WinSocketException reason: " << reason 
       << ", With error code: " << ::WSAGetLastError();
    throw WinSocketException(ss.str());
}