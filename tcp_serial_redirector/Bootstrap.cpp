#include <tinyxml.h>
#include <ctype.h>

#include "SerialTcpServer.h"

#include "Bootstrap.h"  //Must under SerialTcpServer.h, 
                        //because the header file includes, 
                        //windows.h must under winsock2.h

#include "WinSerial.h"
#include "SerialPool.h"

Bootstrap::Bootstrap()
    : spool_(new SerialPool())
{
}

void Bootstrap::Init()
{
    InitWin32SocketEnv();

    TiXmlDocument document("bootstrap.xml");
    document.LoadFile();

    if (document.Error()) {
        std::string msg = std::string("boostrap.xml") + std::string(document.ErrorDesc());
        throw std::domain_error(msg);
    }

    TiXmlElement * root = document.RootElement();

    if (!root || (strcmp(root->Value(), "bootstrap") != 0)) {
        throw std::domain_error("config file error");
    }

    TiXmlElement * sproot = root->FirstChildElement("serialpool");

    if (!sproot) {
        throw std::domain_error("config file error");
    }

    InitSerialPool(sproot);

    TiXmlElement * srvroot =  root->FirstChildElement("servers");
    if (!srvroot) {
        throw std::domain_error("config file error");
    }

    InitSerialTcpServers(srvroot);
}

void Bootstrap::Run()
{
    for(auto ite = srvList_.begin(); ite != srvList_.end(); ++ite) {
        (*ite)->Start();
    }
}

void Bootstrap::Exit()
{
    for(auto ite = srvList_.begin(); ite != srvList_.end(); ++ite) {
        (*ite)->Stop();
    }

    UnInitWin32SocketEnv();
}

void Bootstrap::InitWin32SocketEnv()
{
    WSADATA wsa={0};
    ::WSAStartup(MAKEWORD(2,2),&wsa);
}

void Bootstrap::UnInitWin32SocketEnv()
{
    ::WSACleanup();
}

void Bootstrap::InitSerialPool(TiXmlElement * sproot)
{
    TiXmlElement * xmlserial = sproot->FirstChildElement("serial");

    while (xmlserial != NULL) {
        TiXmlHandle handle(xmlserial);
        TCHAR port[MAX_PATH + 1];

        if (TiXmlTextToPort(handle.FirstChild("port").FirstChild().ToText(), port) != 0) {
            xmlserial = xmlserial->NextSiblingElement();
            continue;            
        }

        DWORD baud = TiXmlTextToBaud(handle.FirstChild("baud").FirstChild().ToText());
        BYTE  stopbits = TiXmlTextToStopBits(handle.FirstChild("stopbits").FirstChild().ToText());
        BYTE  parity = TiXmlTextToParity(handle.FirstChild("parity").FirstChild().ToText());
        BYTE  bytesize = TiXmlTextToByteSize(handle.FirstChild("bytesize").FirstChild().ToText());
                
        try {
            WinSerial * serial = new WinSerial(port, baud, stopbits, parity, bytesize);
            serial->Open();
            spool_->Store(serial);
        } catch (WinSerialException &) {
            //Log Error msg
        }

        xmlserial = xmlserial->NextSiblingElement();
    }
}

void Bootstrap::InitSerialTcpServers(TiXmlElement * srvoot)
{
    TiXmlElement * xmlredirector = srvoot->FirstChildElement("redirector");

    while (xmlredirector) {
        TiXmlHandle handle(xmlredirector);

        TCHAR port[MAX_PATH + 1];
        if (TiXmlTextToPort(handle.FirstChild("port").FirstChild().ToText(), port) !=0 ) {
            xmlredirector = xmlredirector->NextSiblingElement();
            continue;                
        }
        try {
            DWORD tcp = TiXmlTextToDWORD(handle.FirstChild("tcp").FirstChild().ToText());
            WinSerial * serial = spool_->GetSerial(port);
            if (serial) {
                SerialTcpServer * srv = new SerialTcpServer(serial, tcp);
                srvList_.push_back(srv);
            } else {
                //Log
            }
        } catch (std::runtime_error& ) {
            xmlredirector = xmlredirector->NextSiblingElement();
            continue; 
        }

        xmlredirector = xmlredirector->NextSiblingElement();
    }
}

DWORD Bootstrap::TiXmlTextToPort(TiXmlText * text, LPTSTR port)
{
    if (!text) return -1;
     
    const char * str = text->Value();
#ifdef UNICODE
    int len = MultiByteToWideChar(CP_OEMCP, 0, str, -1, port,0);
    len = min(len, MAX_PATH);
    ::MultiByteToWideChar(CP_OEMCP, 0, str, -1, port, len);
#else
    strncpy(port, str, MAX_PATH);
#endif
    return 0;
}

DWORD Bootstrap::TiXmlTextToBaud(TiXmlText * text)
{
    const DWORD BAUD_DEFAULT = CBR_9600;

    if (!text) {
        return BAUD_DEFAULT;
    }

    const char * str = text->Value();    
    
    if (strstr(str, "CBR_")) {
        str = strstr(str, "CBR_");
    }

    try {
        return TiXmlTextToDWORD(text);
    } catch (std::runtime_error &) {
        return BAUD_DEFAULT;
    }
}

BYTE  Bootstrap::TiXmlTextToStopBits(TiXmlText * text)
{
    const BYTE STOPBITS_DEFAULT = ONESTOPBIT;

    if (!text) {
        return STOPBITS_DEFAULT;
    }

    try {
        return (BYTE)TiXmlTextToDWORD(text);
    } catch (std::runtime_error &) {
        return STOPBITS_DEFAULT;
    }
}

BYTE  Bootstrap::TiXmlTextToParity(TiXmlText * text)
{
    const BYTE PARITY_DEFAULT = NOPARITY;

    if (!text) {
        return PARITY_DEFAULT;
    }

    try {
        return (BYTE)TiXmlTextToDWORD(text);
    } catch (std::runtime_error &) {
        return PARITY_DEFAULT;
    }
}

BYTE  Bootstrap::TiXmlTextToByteSize(TiXmlText * text)
{
    const BYTE BYTESIZE_DEFAULT = 8;

    if (!text) {
        return BYTESIZE_DEFAULT;
    }

    try {
        return (BYTE)TiXmlTextToDWORD(text);
    } catch (std::runtime_error &) {
        return BYTESIZE_DEFAULT;
    }
}

DWORD Bootstrap::TiXmlTextToDWORD(TiXmlText * text)
{
    const char * str = text->Value();   
    for (size_t i = 0; i < strlen(str); i++) {
        if (!isdigit(str[i])) {
            throw std::runtime_error("TiXmlText's text isn't digit.");
        }
    }

    return strtoul(str, NULL, 10);
}