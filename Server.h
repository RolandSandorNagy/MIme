#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED


#include "defines.h"
#include "includes.h"
#include "Config.h"


class Controller;


class Server
{

public: /* members */


private: /* members */
    Config* config;
    Controller *controller;

	SOCKET ListenSocket;
	SOCKET ClientSocket;
	WSADATA wsaData;
	struct addrinfo *result;
	struct addrinfo hints;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen;
	int iResult;


public: /* methods */
    Server(Config*);
    void run();


private: /* methods */
    void initServer(Config*);
    bool initWinSock();
    bool resolveServerAddressAndPort();
    bool createSocket();
    bool setupListenSocket();
    bool acceptClientSocket();
    void closeServerSocket();
    bool receiveUntilPeerShutsDown();
    void shutDownConnection();
    void CleanUp();

};


namespace ServerNS
{
    extern void* sThreadMethod(void* hInst);
}



#endif // SERVER_H_INCLUDED
