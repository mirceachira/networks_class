#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP fix found */
  #endif
  #include <winsock2.h>
  #include <Ws2tcpip.h>
  #include <windows.h>
  #include <winsock2.h>
  #include <ws2tcpip.h>

  // Need to link with Ws2_32.lib
  #pragma comment (lib, "Ws2_32.lib")
  // #pragma comment (lib, "Mswsock.lib")
#else
  /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
  #include <unistd.h> /* Needed for close() */

  typedef int SOCKET;
  int INVALID_SOCKET = -1, SOCKET_ERROR=-1;
#endif


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int sockInit(void)
{
  #ifdef _WIN32
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(1,1), &wsa_data);
  #else
    return 0;
  #endif
}

int sockQuit(void)
{
  #ifdef _WIN32
    return WSACleanup();
  #else
    return 0;
  #endif
}

int sockClose(SOCKET sock)
{

  int status = 0;

  #ifdef _WIN32
    status = shutdown(sock, SD_BOTH);
    if (status == 0) { status = closesocket(sock); }
  #else
    status = shutdown(sock, SHUT_RDWR);
    if (status == 0) { status = close(sock); }
  #endif

  return status;

}


int main(void)
{
    int iResult;

    // Windows needs initial socket config
    if (sockInit() != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    } else {
        printf("Initial config OK!\n");
    }

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        sockQuit();
        return 1;
    } else {
        printf("get address success!\n");
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed\n");
        freeaddrinfo(result);
        sockQuit();
        return 1;
    } else {
        printf("socket init success!\n");
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed\n");
        freeaddrinfo(result);
        sockClose(ListenSocket);
        sockQuit();
        return 1;
    } else {
        printf("bind success!\n");
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed\n");
        sockClose(ListenSocket);
        sockQuit();
        return 1;
    } else {
        printf("listen success!\n");
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed\n");
        sockClose(ListenSocket);
        sockQuit();
        return 1;
    } else {
        printf("accept success!\n");
    }

    // No longer need server socket
    sockClose(ListenSocket);

    // Receive until the peer shuts down the connection
    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);

            // Echo the buffer back to the sender
            iSendResult = send( ClientSocket, recvbuf, iResult, 0 );
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed\n");
                sockClose(ClientSocket);
                sockQuit();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else  {
            printf("recv failed\n");
            sockClose(ClientSocket);
            sockQuit();
            return 1;
        }

    } while (iResult > 0);

    // cleanup
    sockClose(ClientSocket);
    sockQuit();

    return 0;
}