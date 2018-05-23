// Retriever
// Your retriever takes in an input from the command line and parses the server address 
// and file (web page) that is being requested.
// The program then issues a GET request to the server for the requested file.
// When the file is returned by the server, the retriever outputs the file to the screen
// and saves the retrieved file to the file system.
// If the server returns an error code instead of an OK code, then the retriever should 
// not save the file and should display on the screen whatever error page was sent with 
// the error.
// our retriever should exit after receiving the response.
// The server name can be either an IP address for simplicity.
// Resources used: https://www.geeksforgeeks.org/socket-programming-cc/
//                 https://stackoverflow.com/questions/8277581/how-to-search-and-print-specific-parts-in-a-string/8277596
//				   https://stackoverflow.com/questions/14004550/http-get-request-in-c
//				   https://stackoverflow.com/questions/36850554/http-server-response-using-sockets

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>      // bzero
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl. htons. inet_ntoa
#include <sys/types.h>    // socket, bind
#include <sys/uio.h>      // writev
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <arpa/inet.h>    // inet_ntoa
#include <netinet/tcp.h>  // SO_REUSEADDR

const int BUFSIZE = 4096; // not sure how big this should be, going with standard
const int PORT = 80;

using namespace std;

int main(int argc, char const *argv[])
{

	// Your retriever takes in an input from the command line and parses the 
	// server address and file (web page) that is being requested.
	const char *serverIP = argv[1];
	string host;
	stringstream getRequest;
	int port = PORT;
	size_t pos = serverIP.find("/");
	if(pos != string::npos)
	{
		host = serverIP.substr(0, pos);
		serverIP = serverIP.substr(pos);
	}
	else{
		host = serverIP;
		serverIP = "/";
	}
	cout << "Host     : " << host << endl;
	cout << "Server IP: " << serverIP << endl;

    // 2. Retrieve a hostent structure corresponding to this IP name by
    //    calling gethostbyname().

	struct hostent* host = gethostbyname(serverIP);

    //3. Declare a sockaddr_in structure, zero-initialize it by calling
    //   b_zero, and set its data members as follows:

    sockaddr_in sendSockAddr;
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET; // Address Family Internet
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);

    // 4. Open a stream-oriented socket with the Internet address family
    int clientSD = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSD < 0)
      {
        perror("Error: creating socket");
        close(clientSD);
        cout << endl;  
        exit(EXIT_FAILURE);
       }

    // 5. Connect this socket to the server by calling connect as passing
    //    the following arguements: the socket descriptor, the sockaddr_in
    //    structure defined above, and its data size(obtained from the
    //    sizeof() function

    int connectStatus = connect(clientSD, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
    if(connectStatus < 0)
      {
        perror("Error: Failed to connect to the server~~");
        close(clientSD);
        cout << endl;
        exit(EXIT_FAILURE);
      }
	  
	// The program then issues a GET request to the server for the requested file.
	getRequest << "GET " << serverIP << " HTTP/1.0\r\n"
    << "Host: " << host << "\r\n"
    << "\r\n";
	string s_request = getRequest.str();
	int send_size = s_request.length();
	cout << "Sending request to server\n";
	connectStatus = send(clientSD, s_request.c_str(), send_size, 0);
	cout << "Request sent to server\n";
	if(connectStatus < 0)
      {
        perror("Error: Failed to send GET request to the server~~");
        close(clientSD);
        cout << endl;
        exit(EXIT_FAILURE);
      }
	
  char databuf[BUFSIZE];
  bzero(&databuf, sizeof(databuf));
  
  
  
  
  

  
  int read_count = 0;
  read(clientSD, &read_count, sizeof(read_count));

  
  // 8. Close the socket by calling close
  close(clientSD);
  return 0;
}

