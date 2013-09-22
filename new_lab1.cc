#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#include "packet.h" // defined by us
#include "Bulls_And_Cows.h"
#include "lab1_server.h"

using namespace std;


#define BUFLEN 256

int Create_tcpServer();
int Create_udpServer();
void test(int tcp_client_fd, int msg_num);

int main(int argc, char *argv[])
{

    parse_argv(argc, argv); // In fact, this is not necessary.
                            // But I leave it here to capture invalid parameters.

    int         tcp_client_fd;
    sockaddr_in tcp_client_addr;     // socket addr for client
    socklen_t   tcp_client_addr_len = sizeof(tcp_client_addr);
    int         bytes_received;
    int         bytes_sent;

    char        recv_buf[BUFLEN];
    char        send_buf[BUFLEN + 20];


    int tcp_server_fd = Create_tcpServer();
    // listen to the socket, wait for incoming connections
    listen(tcp_server_fd, 1);
    //cout << "6. Now we sit here and wait listening patiently" << endl;

    while(1)
    {
        tcp_client_fd = accept(tcp_server_fd, 
                               (struct sockaddr *) &tcp_client_addr,
                               &tcp_client_addr_len);

        test(tcp_client_fd,3); 

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

        test(pid,4);

        if (pid == 0)
        {
            // wipe out anything in recv_buf
            memset(recv_buf, 0, sizeof(recv_buf));

            // receive
            bytes_received = recv(tcp_client_fd, recv_buf, sizeof(recv_buf), 0);

            // check
            test(bytes_received, 6);

            if(bytes_received > 0)
            {
                cout << "[TCP] Rcvd: " << recv_buf << endl; 
            }
            
            int recv_int = atoi(recv_buf);
            char type_name[10];
            get_type_name(recv_int,type_name);
            cout << "we recieved : " << recv << ": " << type_name << endl;

            if (recv_int == 2000){

                // TCP: THE SERVER REPLIES THE UDP SERVER PORT TO THE CLIENT
                memset(send_buf, 0, sizeof(send_buf));
                memcpy(send_buf, "2001" , 19);
                memcpy(send_buf + 19, recv_buf, sizeof(recv_buf));
                bytes_sent = send(tcp_client_fd, send_buf, sizeof(send_buf), 0);
                // check
                test(bytes_sent, 7);

                cout << "[TCP] Sent: " << send_buf << endl;


            
                // REMEMBER TO CLOSE THE TCP SOCKET
                test(close(tcp_client_fd),8);

            }

        }
    }
    return 0;
}

int Create_tcpServer()
{
    cout << "[SYS] Parent process for TCP communication." << endl; 
    cout << "[TCP] Bulls and Cows game server started..." << endl;

    // TCP: CREATE A TCP SERVER FOR ANY INCOMING CONNECTION
    //      REMEMBER TO CHECK ALL RETURN VALUES
    //_________________________________________________________________
    //
    //Defining all the variables needed for the server, including file descripts
    //, socket addresses and the port
    unsigned short int tcp_server_port;
    sockaddr_in tcp_server_addr;     // socket addr for this process
    socklen_t   tcp_server_addr_len = sizeof(tcp_server_addr);

    int         tcp_server_fd;       // socket file descriptor

    cout << "[TCP] Echo server started..." << endl;

    //---------------->// create a TCP listening socket for clients to connect
    // server file descriptor = socket(family, type(tcp), protocol)
    tcp_server_fd = socket(AF_INET, SOCK_STREAM, 0);
  
    // if the return value is -1, the creation of socket is failed.
    test(tcp_server_fd,1);

    // initialize the socket address strcut by setting all bytes to 0
    memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));

    // details are covered in class/slides
    tcp_server_addr.sin_family      = AF_INET;    // internet family
    tcp_server_addr.sin_port        = 0;          // let the OS choose the port
    tcp_server_addr.sin_addr.s_addr = INADDR_ANY; // wild card machine address

    int bind_return = bind(tcp_server_fd,(sockaddr *)&tcp_server_addr, sizeof(tcp_server_addr));

    test(bind_return,2);

    // get the socket name, to obtain the port number assigned by
    // the OS
    getsockname(tcp_server_fd, (struct sockaddr *) &tcp_server_addr,
                &tcp_server_addr_len);

    tcp_server_port = ntohs(tcp_server_addr.sin_port);
    // print the port name
    cout << "[TCP] socket has port: " << tcp_server_port << endl;

    return tcp_server_fd;
}

int Create_udpServer()
{
    // UDP: CREATE A UDP SERVER FOR GAME FOR ONE CLIENT
    //      OBTAIN THE UDP PORT NUMBER
    //      WAIT FOR THE CLIENT TO ISSUE JOIN
    //
    unsigned short int udp_server_port;
    sockaddr_in udp_server_addr;     // socket addr for this process
    socklen_t   udp_server_addr_len = sizeof(udp_server_addr);
    sockaddr_in udp_client_addr;     // socket addr for client
    socklen_t   udp_client_addr_len = sizeof(udp_client_addr);

    int         udp_server_fd;       // socket file descriptor

    int         bytes_received;
    int         bytes_sent;

    char        recv_buf[BUFLEN];
    char        send_buf[BUFLEN + 20];

    cout << "[UDP] Echo server started..." << endl;

    // create a UDP socket with protocol family AF_INET
    udp_server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  
    // if the return value is -1, the creation of socket is failed.
    test(udp_server_fd, 5);

    // initialize the socket address strcut by setting all bytes to 0
    memset(&udp_server_addr, 0, sizeof(udp_server_addr));

    udp_server_addr.sin_family      = AF_INET;    // internet family
    udp_server_addr.sin_port        = 0;          // let the OS choose the port
    udp_server_addr.sin_addr.s_addr = INADDR_ANY; // wild card machine address

    // Bind the socket name to the socket
    if(bind(udp_server_fd, 
            (sockaddr *) &udp_server_addr, 
            sizeof(udp_server_addr)) < 0)
    {
        cerr << "[ERR] Unable to bind UDP socket." << endl;
        exit(1);
    }

    // get the socket name, to obtain the port number assigned by 
    // the OS
    getsockname(udp_server_fd, (struct sockaddr *) &udp_server_addr, 
                &udp_server_addr_len);
    udp_server_port = ntohs(udp_server_addr.sin_port);
    // print the port name
    cout << "[UDP] socket has port: " << udp_server_port << endl;

    return udp_server_fd;

}
void test(int int_given, int msg_num)
{
    if( int_given < 0)
    {
        switch(msg_num)
        {
            case 1:
                cerr << "[ERR] Unable to create TCP socket." << endl;
                break;

            case 2:
                cerr << "[ERR] Unable to bind TCP socket." << endl;
                break;

            case 3:
                cerr << "[ERR] Error on accepting." << endl;
                break;

            case 4:
                cerr << "[ERR] fork() failed." << endl;
                break;

            case 5:
                cerr << "[ERR] Unable to create UDP socket." << endl;
                break;

            case 6:
                cerr << "[ERR] Error receiving message from client." << endl;
                break;

            case 7:
                cerr << "[ERR] Error sending message to client." << endl;
                break;

            case 8:
                cerr << "[ERR] Error when closing client socket." << endl;
                break;

            default:
                cerr << "[ERR] Something went awry." << endl;
        }
        exit(1);
    }
}
