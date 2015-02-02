#pragma once

#include <list>
#include <Windows.h>

class SerialPool;
class SerialTcpServer;

class TiXmlElement;
class TiXmlText;

class Bootstrap
{
public:
    Bootstrap();

    void Init();
    void Run();
    void Exit();

private:
    void InitWin32SocketEnv();
    void UnInitWin32SocketEnv();

    void InitSerialPool(TiXmlElement * sproot);
    void InitSerialTcpServers(TiXmlElement * srvoot);   

    DWORD TiXmlTextToPort(TiXmlText * text, LPTSTR port);
    DWORD TiXmlTextToBaud(TiXmlText * text);
    BYTE  TiXmlTextToStopBits(TiXmlText * text);
    BYTE  TiXmlTextToParity(TiXmlText * text);
    BYTE  TiXmlTextToByteSize(TiXmlText * text);

    DWORD TiXmlTextToDWORD(TiXmlText * text);

    std::list<SerialTcpServer*> srvList_;
    SerialPool * spool_;  
};