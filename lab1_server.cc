#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <cstring>

#include "packet.h" // defined by us
#include "Bulls_And_Cows.h"
#include "lab1_server.h"

using namespace std;


#define BUFLEN 256

// Function prototypes
void SampleHandler(/*You decide*/);

int main(int argc, char *argv[])
{
    parse_argv(argc, argv); // In fact, this is not necessary.
                            // But I leave it here to capture invalid
                            // parameters.

    cout << "[SYS] Parent process for TCP communication." << endl; 
    cout << "[TCP] Bulls and Cows game server started..." << endl;

    // TCP: CREATE A TCP SERVER FOR ANY INCOMING CONNECTION
    //      REMEMBER TO CHECK ALL RETURN VALUES
    //
    //_________________________________________________________________
    //
    //Defining all the variables needed for the server, including file descripts
    //, socket addresses and the port
    unsigned short int tcp_server_port;
    sockaddr_in tcp_server_addr;     // socket addr for this process
    socklen_t   tcp_server_addr_len = sizeof(tcp_server_addr);
    sockaddr_in tcp_client_addr;     // socket addr for client
    socklen_t   tcp_client_addr_len = sizeof(tcp_client_addr);

    int         tcp_server_fd;       // socket file descriptor
    int         tcp_client_fd;

    int         bytes_received;
    int         bytes_sent;

    char        recv_buf[BUFLEN];
    char        send_buf[BUFLEN + 20];

    cout << "[TCP] Echo server started..." << endl;

    //---------------->// create a TCP listening socket for clients to connect
    // server file descriptor = socket(family, type(tcp), protocol)
    tcp_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    cout << "1. We set the TCP file descriptor : " << tcp_server_fd << endl;
  
    // if the return value is -1, the creation of socket is failed.
    if(tcp_server_fd < 0)
    {
        cerr << "[ERR] Unable to create TCP socket." << endl;
        exit(1);
    }
    

    // initialize the socket address strcut by setting all bytes to 0
    memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));
    cout << "2. We set the tcp server address to zero using memset" << endl;

    // details are covered in class/slides
    tcp_server_addr.sin_family      = AF_INET;    // internet family
    tcp_server_addr.sin_port        = 0;          // let the OS choose the port
    tcp_server_addr.sin_addr.s_addr = INADDR_ANY; // wild card machine address
    cout << "3. we set the tcp_server_addr family, port and machine address" << endl;

    //---------------->// Bind the socket name to the socket
    int bind_return = bind(tcp_server_fd,(sockaddr *)&tcp_server_addr, sizeof(tcp_server_addr));
    cout << "4. we bind the socket name to the socket, it is like giving the socket an identity: "<< bind_return << endl;

    if( bind_return < 0)
    {
        cerr << "[ERR] Unable to bind TCP socket." << endl;
        exit(1);
    }

    // get the socket name, to obtain the port number assigned by
    // the OS
    getsockname(tcp_server_fd, (struct sockaddr *) &tcp_server_addr,
                &tcp_server_addr_len);
    tcp_server_port = ntohs(tcp_server_addr.sin_port);
    // print the port name
    cout << "5. [TCP] socket has port: " << tcp_server_port << endl;

    // listen to the socket, wait for incoming connections
    listen(tcp_server_fd, 1);
    cout << "6. Now we sit here and wait listening patiently" << endl;

    //_______________________________________________________________________________________________

    while(1){
        // TCP: ACCETP NEW INCOMING CONNECTION/CLIENT
        cout << "   a. we recieved a request, yay: " << tcp_client_fd << endl;

        // accept the incoming connection
        // store client addr in tcp_client_addr
        tcp_client_fd = accept(tcp_server_fd, 
                               (struct sockaddr *) &tcp_client_addr,
                               &tcp_client_addr_len);

        if(tcp_client_fd < 0)
        {
            cerr << "[ERR] Error on accepting." << endl;
            exit(1);
        }


        // FOR EACH NEW CONNECTION / CLIENT
        // CREATE / FORK A CHILD PROCESS TO HANDLE IT
        pid_t pid = fork();
        // REMEMBER TO CHECK ALL RETURN VALUES

        // FOR CHILD PROCESS
        //     CREATE A UDP SERVER FOR THE GAME
        //     FOR EACH CHILD WE INVOKE A HANDLER FOR IT
        //     SampleHandler
        // FOR PARENT PROCESS
        //     CONTINUE THE LOOP TO ACCEPT NEW CONNECTION/CLIENT
        cout << "   b. we forked the process to handle the request!: " << pid << endl;

        // if pid < 1, the creation of process is failed
        if(pid < 0)
        {
            cerr << "[ERR] fork() failed." << endl;
            exit(1);
        }
        // if pid == 0, this is the child process
        else if (pid == 0)
        {
            // We create a child process to handle this TCP connection.
            // so that the parent process can continue to accept new 
            // connections
            cout << "   c. now we memset the recieve buffer to zero" << endl;
            // wipe out anything in recv_buf
            memset(recv_buf, 0, sizeof(recv_buf));

            // receive
            bytes_received = recv(tcp_client_fd, recv_buf, sizeof(recv_buf), 0);
            // can also use read()
            //bytes_received = read(tcp_client_fd, recv_buf, sizeof(recv_buf));
            cout << "   d. We are now recieveing all the beautiful bytes: "<< bytes_received << endl;

            // check
            if(bytes_received < 0)
            {
                cerr << "[ERR] Error receiving message from client." << endl;
                exit(1);
            }
            else if(bytes_received > 0)
            {
                cout << "[TCP] Rcvd: " << recv_buf << endl; 
  
                // wipe out anything in send_buf
                memset(send_buf, 0, sizeof(send_buf));
                memcpy(send_buf, "I got your message:", 19);
                memcpy(send_buf + 19, recv_buf, sizeof(recv_buf));
                bytes_sent = send(tcp_client_fd, send_buf, sizeof(send_buf), 0);
                // can also use write()
                //bytes_sent = write(tcp_client_fd, send_buf, sizeof(send_buf));
                cout << "   e. After recieving the message wer are going to reply" << endl;

                // check
                if(bytes_sent < 0)
                {
                    cerr << "[ERR] Error sending message to client." << endl;
                    exit(1);
                }
                cout << "[TCP] Sent: " << send_buf << endl;

                if(close(tcp_client_fd) < 0)
                {
                    cerr << "[ERR] Error when closing client socket." << endl;
                    exit(1);
                }
                return 0;
            }
        }
    }


    if(close(tcp_server_fd) < 0)
    {
        cerr << "[ERR] Error when closing server socket." << endl; 
        exit(1);
    }
    return 0;
}

/*********************************
 * Name:    SampleHandler
 * Purpose: The function to handle UDP communication from/to a client
 * Receive: You decide
 * Return:  You decide
*********************************/
void SampleHandler(/*You decide*/)
{
    // UDP: CREATE A UDP SERVER FOR GAME FOR ONE CLIENT
    //      OBTAIN THE UDP PORT NUMBER
    //      WAIT FOR THE CLIENT TO ISSUE JOIN

    // TCP: THE SERVER REPLIES THE UDP SERVER PORT TO THE CLIENT

    // REMEMBER TO CLOSE THE TCP SOCKET

    while(1) // UDP: GAMEPLAY
    {
        // GAME: START A NEW GAME IF THE PLAYER HAS ALL FOUR
        //       DIGITS CORRECTLY

        // UDP: RECV/SEND INTERACTIONS
    }
}

