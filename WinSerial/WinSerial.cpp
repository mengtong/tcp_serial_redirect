#include "WinSerial.h"

#include <tchar.h>
#include <Shlwapi.h>

WinSerial::WinSerial(LPCTSTR port, DWORD baud, BYTE stopbits, BYTE parity, BYTE ByteSize)
    : handle_(INVALID_HANDLE_VALUE)
    , baud_(baud)
    , stopbits_(stopbits)
    , parity_(parity)
    , ByteSize_(ByteSize)
{
    StrCpy(port_, port);
}

LPCTSTR WinSerial::Port()
{
    return port_;
}

VOID WinSerial::Open()
{
    handle_ = ::CreateFile(port_, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (handle_ == INVALID_HANDLE_VALUE) {
        WinSerialException::Throw("CreateFile@WinSerial::Open");
    }

    DCB dcb;
    ::GetCommState(handle_, &dcb);
    dcb.BaudRate = baud_;
    dcb.StopBits = stopbits_;
    dcb.Parity   = parity_;
    dcb.ByteSize = 8;
    BOOL ret = ::SetCommState(handle_, &dcb);

    if (ret != TRUE) {
        ::CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
        WinSerialException::Throw("SetCommState@WinSerial::Open");
    }

    ::PurgeComm(handle_, PURGE_TXCLEAR|PURGE_RXCLEAR);
}

void WinSerial::Close()
{
    BOOL ret = ::CloseHandle(handle_);
    if (ret != TRUE) {
        WinSerialException::Throw("CloseHandle@WinSerial::Close");
    }
    handle_ = INVALID_HANDLE_VALUE;
}

DWORD WinSerial::InWaiting()
{
    DWORD errs = 0;
    COMSTAT stat;

    BOOL ret = ::ClearCommError(handle_, &errs, &stat);
    if (ret != TRUE) {
        WinSerialException::Throw("ClearCommError@WinSerial::InWaiting");
    }

    return stat.cbInQue;
}

DWORD WinSerial::Write(LPCVOID src, DWORD nbytes)
{
    DWORD written;
    BOOL ret = ::WriteFile(handle_, src, nbytes, &written, NULL);

    if (ret != TRUE) {
        WinSerialException::Throw("WriteFile@WinSerial::Write");
    }

    return written;
}

DWORD WinSerial::Read(LPVOID dst, DWORD nbytes)
{
    DWORD readed;

    DWORD inWaiting = InWaiting();
    nbytes = min(nbytes, inWaiting);

    BOOL ret = ::ReadFile(handle_, dst, nbytes, &readed, NULL);

    if (ret != TRUE) {
        WinSerialException::Throw("ReadFile@WinSerial::Read");
    }

    return readed;
}

#include <sstream>
WinSerialException::WinSerialException(const std::string & msg)
    : std::runtime_error(msg)
{    
}

WinSerialException::~WinSerialException()
{
}

const char * WinSerialException::what() const
{
    return errstr.c_str();
}

void WinSerialException::Throw(const std::string & reason)
{
    std::stringstream ss;
    ss << "WinSerialException reason: " << reason 
       << ", With error code: " << ::GetLastError();
    throw WinSerialException(ss.str());
}