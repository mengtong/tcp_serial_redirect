#pragma once

#include <exception>
#include <string>
#include <Windows.h>

class WinSerial
{
public:    
    WinSerial(LPCTSTR port, 
        DWORD baud = CBR_9600 , 
        BYTE stopbits = ONESTOPBIT, 
        BYTE parity = NOPARITY,
        BYTE ByteSize = 8);
    
    LPCTSTR Port();

    VOID Open();
    VOID Close();   

    DWORD InWaiting();
    DWORD Write(LPCVOID src, DWORD nbytes);
    DWORD Read(LPVOID dst, DWORD nbytes);
private:
    TCHAR port_[MAX_PATH + 1];
    DWORD baud_;
    BYTE stopbits_;
    BYTE parity_;
    BYTE ByteSize_;

    HANDLE handle_;
};

class WinSerialException : public std::runtime_error 
{
public:
    WinSerialException(const std::string & reason);
    ~WinSerialException();

    virtual const char * what() const;

    static void Throw(const std::string & reason);

private:
    std::string errstr;
};