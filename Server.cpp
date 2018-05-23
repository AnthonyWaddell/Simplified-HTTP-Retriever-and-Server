// The server will create a tcp Socket that listens on a port(the last 4
// digits of your ID number. The server will accept an incoming connection
// and then create a new thread(use the pthreads lobrary) that will handle
// the connection. The new thread will read all the data from the client
// and respond back to it (acknowledgement). The response detail will be
// provided in Server.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>      // bzero
#include <cstring>
#include <string>
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <sys/types.h>    // socket, bind
#include <sys/uio.h>      // writev
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <arpa/inet.h>    // inet_ntoa
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <pthread.h>

const int BUFSIZE = 4096; // not sure how much this is supposed to be yolo
int serverSD;
using namespace std;

//Forward declaration of thread_data
struct thread_data
{
  int sd;
};

// Function to read in and handle GET request from client
void *handle_Request(void *ptr)
{
	int thread_ptr = ((struct thread_data*)ptr)->sd;
	stringstream client_request;
	stringstream server_response;
	int status_code;
	string client_file;
	string forbidden = "..";
	char response_buf[BUFSIZE];

    // Allocate databuf[BUFSIZE] where BUFSIZE = 4096 standard
	char databuf[BUFSIZE];

	// Read in data from client
	int inc_data = read(sd, &databuf, BUFSIZE);
	if(inc_data < 0)
	{
		perror("Error: reading data into buffer\n");
		exit(EXIT_FAILURE);
	}
	else{
		// While there is data to read in
		while(inc_data > 0)
		{
			// Read data of streamsize inc_data from request into buffer
			cout << "Reading data from client request" << endl;
			client_request.write(databuf, inc_data);
			if(inc_data >= BUFSIZE)
			{
				inc_data = read(sd, &databuf, BUFSIZE);
			}
			else
			{
				inc_data = 0;
			}
		}
	}
	
	client_file = client_request.str();
	cout << "Client Request: " << client_file << endl;
	
	// After receiving the GET request
	for(int i = 0; i < 4; i++)
	{
		client_file.replace(0, 1, "");
	}
	client_file = client_file.substr(0, client_file.find(" "));
	if (client_file == "/")
	{
		client_file = "testing.html";
	}
	else
	{
		client_file = client_file.replace(0, 1, "");
	}
	
	// Open file for reading
	ifstream ifs_file(client_file.c_str(), ifstream::in);
	
	// If the file requested does not exist, the server should return a 
	// 404 Not Found code along with a custom File Not Found page.
	if(!ifs_file.good())
	{
		httpcode = 404;
		server_response << status_code << " File Not Found\r\n\r\n";
		ifs_file.close();
	}
	// If the HTTP request is for SecretFile.html then the web server should
	// return a 401 Unauthorized.
	else if(client_file == "SecretFile.html")
	{
		status_code = 401;
		server_response << status_code << " Unauthorized\r\n\r\n";
		ifs_file.close();
	}
	// If the request is for a file that is above the directory structure where your 
	// web server is running ( for example, "GET ../../../etc/passwd" ), you should 
	// return a 403 Forbidden.
	else if(client_file.find(forbidden) != string::npos)
	{
		status_code = 403;
		server_response << status_code << " Forbidden \r\n\r\n";
		ifs_file.close();
	}
	// If the file exists, the server opens the file that is requested and sends it 
	// (along with the HTTP 200 OK code, of course).
	else
	{
		status_code = 200;
		server_response << "HTTP/1.0 " << status_code;
		while(true)
		{
			ifs_file.read(response_buf, BUFSIZE);
			if(ifs_file.eof())
			{
				break;
			}
			server_response.write(response_buf, BUFSIZE);
		}
		ifs_file.close();
	}
	
	string temp = server_response.str();
	int message_length = temp.length();
	int ret_val = send(sd,  temp.c_str(), message_length, 0);
	if(ret_val < 0)
	{
		cout << "Error: could not send server response\n";
		exit(EXIT_FAILURE);
	}
	
  // 7. close the connection
  close(serverSD);

  // 8. Optionally, terminate the server process by calling exit(0)
  exit(0);

}

int main(int argc, char const *argv[])
{
  // For listening to client
  int n_connection_rq = 5; // not sure how many this is supposed to be
  
  // 1. Declare a sockaddr_in structure, zero initialize by calling bzero,
  //    and set its data members as follows
  sockaddr_in acceptSockAddr; 
  bzero((char*)&acceptSockAddr, sizeof(acceptSockAddr));
  acceptSockAddr.sin_family = AF_INET; // Address family Internet
  acceptSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  acceptSockAddr.sin_port = htons(port);

  // 2. Open a stream-oriented socket with the Internet address family
  serverSD = socket(AF_INET, SOCK_STREAM, 0);

  // 3. Set the SO-REUSEADDR options
  const int on = 1;
  setsockopt(serverSD, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int));

  // 4. Bind this socket to its local address by calling bind as passing the
  //    following arguement: the socket descriptor, the sockaddr_in structure
  //    defined above, and its data size
  bind(serverSD, (sockaddr*)&acceptSockAddr, sizeof(acceptSockAddr));

  // 5. Instruct the operating system to listen up to n connection requests
  //    from clients at a time by calling listen
  listen(serverSD, n_connection_rq);
  cout << "Server listening to port: " << port << endl;

  // 6. Recieve a request from a client by calling accept that will return
  //    a new socket specific to this connection request
  sockaddr_in newSockAddr;
  socklen_t newSockAddrSize = sizeof(newSockAddr);
  int newSD = accept(serverSD, (sockaddr*)&newSockAddr, &newSockAddrSize);
  
  if(newSD < 0)
	{
		 perror("Error: failure to accept new socket\n");
		 cout << endl;
		 exit(EXIT_FAILURE);
	}
	else
	{
		cout << "Server successfully connected" << endl;
	}
  while(1)
    {
	  // Create thread and handle connection request
      pthread_t new_thread;
      struct thread_data *data = new thread_data();
      data->sd = newSD;
      int iretl = pthread_create(&new_thread, NULL, handle_request, (void*)data);
	  cout << "New thread created~~" << endl;
	  
	  newSD = accept(serverSD, (sockaddr*)&newSockAddr, &newSockAddrSize);
	  if(newSD < 0)
		{
			perror("Error: failure to accept new socket\n");
			cout << endl;
			exit(EXIT_FAILURE);
		}
	  else
		{
			cout << "Established new connection" << endl;
		}
			
    }

  return 0;
}
