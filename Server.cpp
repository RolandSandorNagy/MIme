#include "Server.h"
#include "View.h"
#include "Controller.h"


#define INIT_WINSOCK_FAILED "getaddrinfo failed with error: "
#define ADDRESS_RESOLVE_FAILED "getaddrinfo failed with error: "
#define CREATE_SOCKET_FAILED "socket failed with error: "
#define BIND_SOCKET_FAILED "bind failed with error: "
#define LISTEN_SOCKET_FAILED "listen failed with error: "
#define ACCEPT_CLIENT_FAILED "accept failed with error: "
#define RECEIVE_FAILED "recv failed with error: "
#define SHUTDOWN_FAILED "shutdown failed with error: "


namespace ServerNS
{
    void* sThreadMethod(void* hInst);
}

namespace global
{
    extern View* hgView;

    extern bool isRunning;
}


Server::Server(Config* config)
{
    initServer(config);
    controller = (Controller*)new Controller(global::hgView/*, this*/);
    result = NULL;
}

void Server::initServer(Config* conf)
{
    config = conf;
    recvbuflen = DEFAULT_BUFLEN;
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
	result = NULL;
}

void Server::run()
{
    if(!global::isRunning || !initWinSock())
        return;
    if(!global::isRunning || !resolveServerAddressAndPort())
        return;
    if(!global::isRunning || !createSocket())
        return;
    if(!global::isRunning || !setupListenSocket())
        return;
    if(!global::isRunning || !acceptClientSocket())
        return;
    if(global::isRunning)
        closeServerSocket();
    if(!global::isRunning || !receiveUntilPeerShutsDown())
        return;
    if(global::isRunning)
        shutDownConnection();
    if(global::isRunning)
        CleanUp();
}

bool Server::initWinSock()
{
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << INIT_WINSOCK_FAILED << iResult << std::endl;
        WSACleanup();
        return false;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    return true;
}

bool Server::resolveServerAddressAndPort()
{
    iResult = getaddrinfo(config->getHost().c_str(), config->getPort().c_str(), &hints, &result);
    if (iResult != 0) {
        std::cerr << ADDRESS_RESOLVE_FAILED << iResult << std::endl;
        WSACleanup();
        return false;
    }
    return true;
}

bool Server::createSocket()
{
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << CREATE_SOCKET_FAILED << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }
    return true;
}

bool Server::setupListenSocket()
{
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << BIND_SOCKET_FAILED << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        CleanUp();
        return false;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cerr << LISTEN_SOCKET_FAILED << WSAGetLastError() << std::endl;
        CleanUp();
        return false;
    }

    return true;
}

bool Server::acceptClientSocket()
{
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        std::cerr << ACCEPT_CLIENT_FAILED << WSAGetLastError() << std::endl;
        CleanUp();
        return false;
    }
    return true;
}

void Server::closeServerSocket()
{
    closesocket(ListenSocket);
}


bool Server::receiveUntilPeerShutsDown()
{
    do
    {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            controller->processMessage(recvbuf, recvbuflen, iResult);
        } else if (iResult < 0) {
            std::cerr << RECEIVE_FAILED << WSAGetLastError() << std::endl;
            CleanUp();
            return false;
        }
    } while (iResult > 0 && global::isRunning);

    return true;
}

void Server::shutDownConnection()
{
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cerr << SHUTDOWN_FAILED << WSAGetLastError() << std::endl;
        CleanUp();
        return;
    }
}

void Server::CleanUp()
{
    closeServerSocket();
    WSACleanup();
}


// ----------------------------------------------------- //
/* ***************************************************** */
// ----------------------------------------------------- //


void* ServerNS::sThreadMethod(void* config)
{
    Server server((Config*)config);
	while(global::isRunning) server.run();

    return (void*)0;
}
