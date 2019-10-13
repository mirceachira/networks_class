#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

int main() {
  int s;
  struct sockaddr_in server, client;
  int c, l;
  
  // Create server socket
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    printf("Eroare la crearea socketului server\n");
    return 1;
  } else  {
    printf("Client created!\n");
  }

  memset(&server, 0, sizeof(server));
  server.sin_port = htons(1234);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  
  if (bind(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
    printf("Eroare la bind\n");
    return 1;
  }
 
  listen(s, 5);
  
  l = sizeof(client);
  memset(&client, 0, sizeof(client));
  
  while (1) {
    uint16_t a, b, suma;
    c = accept(s, (struct sockaddr *) &client, &l);
    printf("S-a conectat un client.\n");
    
    while (1) {
      // deservirea clientului
      recv(c, &a, sizeof(a), MSG_DONTWAIT);
      // recv(c, &b, sizeof(b), MSG_WAITALL);
      a = ntohs(a);
      // b = ntohs(b);
      // suma = a + b;
      // suma = htons(suma);
      // send(c, &suma, sizeof(suma), 0);
      printf("%d", a);
    }

    close(c);
    // sfarsitul deservirii clientului;
  }
}