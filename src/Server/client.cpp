#include <sys/socket.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


int main(void)
{
  
  int socketfd = socket( AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
    std::cout << "could not create socket" << std::endl;
  std::cout << socketfd << std::endl;

  //  define the server address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(8080);
  server_address.sin_addr.s_addr = INADDR_ANY;

  // connection request
  int s_connection = connect(socketfd, (const struct sockaddr *)&server_address, (socklen_t)sizeof(server_address));
  if (s_connection < 0 )
  {
    std::cout << "Could not connect to socket" << std::endl;
    return 1;
  }

  // reseve data from the server
  char buf[255];
  /* int len = 0;
  while (read(socketfd, &buf[len++],1) != -1); */

  recv(socketfd, buf,sizeof(buf), 0);
  std::cout << "the message is -> " << buf << std::endl;

  return (0);
}
