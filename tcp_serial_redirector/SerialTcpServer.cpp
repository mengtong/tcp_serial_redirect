#include "SerialTcpServer.h"
#include "Redirector.h"

SerialTcpServer::SerialTcpServer(WinSerial * serial, int tcpport)
    : WinTcpServer(tcpport)
    , serial_(serial)
{
}

void SerialTcpServer::OnNewConnection(WinSocket * Socket)
{
    Redirector redirector(Socket, serial_);
    redirector.shutcut(); //this will make the server thread blocking,
                          //until remote connection broken
    delete Socket;
}