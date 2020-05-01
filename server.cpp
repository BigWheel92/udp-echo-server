#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
using namespace std;

#define MAX_STRING_LENGTH 100
int sock_fd;

void* terminatorThread(void*)
{ 
   char a;
   cout<<"Enter any key and press enter to shut down the server: "<<endl;
   cin>>a;
   close(sock_fd);
   exit(0); //exit the whole server program
}

int main(int argc, char *argv[])
{
  if (argc==1)
  {
     cout<<"Please pass the port number on which you want the server to listen."<<endl;
     exit(-1);
  }
  
  int tid=0;
  if (pthread_create((pthread_t*) &tid, NULL, terminatorThread, NULL)==-1)
  {
   perror("pthread_create: ");
   exit(-1);
  }
  
  sockaddr_in server, client;

  if ((sock_fd= socket(AF_INET, SOCK_DGRAM, 0))==-1)
  {
     perror("socket: ");
     exit(-1);
  }

  server.sin_family= AF_INET;
  server.sin_port= htons(atoi(argv[1]));
  server.sin_addr.s_addr= INADDR_ANY;
  bzero(&server.sin_zero, 8);

  if ((bind(sock_fd, (sockaddr*) &server, (socklen_t) sizeof(server)))==-1)
  {
   perror("bind:");
   exit(-1);
  }

  char messageBuffer[MAX_STRING_LENGTH+1];

  while (true)
  {
    int len=sizeof(client);
    if (recvfrom(sock_fd, (void*) messageBuffer, (size_t) (MAX_STRING_LENGTH+1), MSG_WAITALL, (sockaddr*) &client, (socklen_t*) &len)==-1)
    {
      perror("recvfrom:");
      continue;
    } 
   
    cout<<"Data Received from client ("<<inet_ntoa(client.sin_addr)<<"):  "<<messageBuffer<<endl<<endl;
	 
    if (sendto(sock_fd, (void*) messageBuffer, (size_t) strlen(messageBuffer)+1, MSG_CONFIRM, (sockaddr*) &client, (socklen_t) len)==-1)
    {
      perror("sendto: ");
      continue;
    } 

  }//end of while loop

}
