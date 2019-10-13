#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
 #include <arpa/inet.h> // for inet_addr
 
int main() {
  int c;
  struct sockaddr_in server;
  uint16_t a, b, n, suma;
  
  c = socket(AF_INET, SOCK_STREAM, 0);
  if (c < 0) {
    printf("Eroare la crearea socketului client\n");
    return 1;
  }
  
  memset(&server, 0, sizeof(server));
  server.sin_port = htons(1234);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  if (connect(c, (struct sockaddr *) &server, sizeof(server)) < 0) {
    printf("Eroare la conectarea la server\n");
    return 1;
  }

  do {
    printf("enter a number ");
    scanf("%hu", &n);  
    n = htons(n);
    send(c, &n, sizeof(n), 0);
  } while (1);
  
  // send(c, &b, sizeof(b), 0);
  // recv(c, &suma, sizeof(suma), 0);
  // suma = ntohs(suma);
  // printf("Suma este %hu\n", suma);
  close(c);
}