#pragma once

class WinSocket;
class WinSerial;

class Redirector
{
public:
    Redirector(WinSocket * sock, WinSerial * ser);

    void shutcut();

    void SerialReadLoop();
    void SocketReadLoop();

private:
    void ServeSerialRead();
    void ServeSocketRead();

    WinSocket * sock_;
    WinSerial * ser_;
    volatile int runFlag_;
};