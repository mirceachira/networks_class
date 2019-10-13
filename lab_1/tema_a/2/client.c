#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP fix found */
  #endif
  #include <winsock2.h>
  #include <Ws2tcpip.h>
  #include <windows.h>
  #include <winsock2.h>
  #include <ws2tcpip.h>

  // Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
  #pragma comment (lib, "Ws2_32.lib")
  #pragma comment (lib, "Mswsock.lib")
  #pragma comment (lib, "AdvApi32.lib")
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



int main(int argc, char **argv)
{
    int iResult;
    // Windows needs initial socket config
    if (sockInit() != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    } else {
        printf("Initial config OK!\n");
    }

    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    } else {
        printf("arguments success!\n");
    }

    memset( &hints, 0, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        sockQuit();
        return 1;
    } else {
        printf("get address success %d!\n");
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed\n");
            sockQuit();
            return 1;
        } else {
            printf("socket init success!\n");
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            sockClose(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        sockQuit();
        return 1;
    } else {
        printf("connection success!\n");
    }

    // Send a string
    char *sendbuf = "this is a test";
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed\n");
        sockClose(ConnectSocket);
        sockQuit();
        return 1;
    } else {
        printf("send success!\n");
    }

    printf("Bytes Sent: %d\n", iResult);

    // Receive until the peer closes the connection
    do {
        int spaces;
        iResult = recv(ConnectSocket, &spaces, sizeof(spaces), 0);
        if ( iResult > 0 ) {
            printf("Bytes received: %d\n", iResult);
            printf("There are %d spaces\n", spaces);
        } else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed\n");

    } while( iResult > 0 );

    // cleanup
    sockClose(ConnectSocket);
    sockQuit();

    return 0;
}