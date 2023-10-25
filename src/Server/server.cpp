#include <sys/socket.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


int main(void)
{

  char message[255] = "<h1>Hello form server</h1>";
  
  while(true)
  {

    while(true)
    {
      int socketfd;
      socketfd = socket( AF_INET, SOCK_STREAM, 0);
      if (socketfd < 0)
        std::cout << "could not create socket" << std::endl;
      std::cout << socketfd << std::endl;

      //  define the server address
      struct sockaddr_in server_address;
      server_address.sin_family = AF_INET;
      server_address.sin_port = htons(8080);
      server_address.sin_addr.s_addr = INADDR_ANY;

      // bind the IP and port to the server
      if (bind(socketfd, (const struct sockaddr *)&server_address, (socklen_t)sizeof(server_address)) < 0)
        std::cout << "Could not bind the address \n";

      // listen at the port
      listen(socketfd, 5);
      int client_socket = accept(socketfd,NULL, NULL);
      if (client_socket < 0)
        std::cout << "could not create socket for client " << std::endl;
      if(fork())
      {
        // accept the incomming connection from the client
        // send the massage to the client address``
        /* if (!send(client_socket, message, strlen(message) , 0))
      std::cout << "Could not send the message " << std::endl; */
        write(client_socket, message, sizeof(message));
      }

    }

   // close(client_socket);
  }
  return (0);
}
