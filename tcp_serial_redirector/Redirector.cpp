#include "Redirector.h"

#include "WinSocket.h"
#include "WinSerial.h"

static DWORD WINAPI SocketReadThread(LPVOID arg);

Redirector::Redirector(WinSocket * sock, WinSerial * ser)
    : sock_(sock)
    , ser_(ser)
    , runFlag_(TRUE)
{
}

void Redirector::shutcut()
{
    DWORD threadId;
    HANDLE thread = ::CreateThread(NULL, 0, SocketReadThread, this, 0, &threadId);
    ::CloseHandle(thread);

    SerialReadLoop();
}

void Redirector::SerialReadLoop()
{
    while (runFlag_) {
        try {            
            ServeSerialRead();
        } catch (WinSocketException & ex) {
            runFlag_ = FALSE;
            break;                
        } catch (WinSerialException & ex) {            
            sock_->Shutdown();
            runFlag_ = FALSE;
            break;
        }
        ::Sleep(50);
    }
}

void Redirector::SocketReadLoop()
{
    while (runFlag_) { 
        try {            
            ServeSocketRead();
        } catch (WinSocketException & ex) {
            runFlag_ = FALSE;
            break;                
        } catch (WinSerialException & ex) {
            sock_->Shutdown();
            runFlag_ = FALSE;
            break;
        }
        ::Sleep(50);
    }
}

void Redirector::ServeSerialRead()
{
    DWORD inWaiting = 0;
    DWORD toRead    = 0;
    DWORD readed    = 0;
    BYTE buffer[MAX_PATH];

    if ((inWaiting = ser_->InWaiting()) > 0) {
        toRead = min(inWaiting, MAX_PATH);            
        readed = ser_->Read(buffer, toRead);
        sock_->Write(buffer, readed);
    }
}

void Redirector::ServeSocketRead()
{
    DWORD readed    = 0;
    BYTE buffer[MAX_PATH];

    //if (sock_->WaitForData(1000)) {            
        readed = sock_->Read(buffer, MAX_PATH);
        ser_->Write(buffer, readed);
    //}
}

DWORD WINAPI SocketReadThread(LPVOID arg)
{
    Redirector * r = reinterpret_cast<Redirector*>(arg);
    r->SocketReadLoop();
    return 0L;
}