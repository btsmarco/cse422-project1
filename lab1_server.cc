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

sockaddr_in udp_server_addr;     // socket addr for this process
socklen_t   udp_server_addr_len = sizeof(udp_server_addr);


int Create_tcpServer();
int Create_udpServer(short unsigned int* port);
void test(int int_given, int msg_num);
void RequestHandler(int tcp_client_fd);

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
           RequestHandler(tcp_client_fd);
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

int Create_udpServer(short unsigned int *port)
{
    // UDP: CREATE A UDP SERVER FOR GAME FOR ONE CLIENT
    //      OBTAIN THE UDP PORT NUMBER
    //      WAIT FOR THE CLIENT TO ISSUE JOIN
    //
    unsigned short int udp_server_port;
    int         udp_server_fd;       // socket file descriptor

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
    *port = udp_server_port;

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
void RequestHandler(int tcp_client_fd)
{
    sockaddr_in udp_client_addr;     // socket addr for client
    socklen_t   udp_client_addr_len = sizeof(udp_client_addr);

    int         bytes_received;
    int         bytes_sent;

    char        recv_buf[BUFLEN];
    char        send_buf[BUFLEN + 20];

    // wipe out anything in recv_buf
    memset(recv_buf, 0, sizeof(recv_buf));

    My_Packet pkt_rcv;
    char type_name[16];

    // receive
    bytes_received = recv(tcp_client_fd, &pkt_rcv, sizeof(recv_buf), 0);
    // check
    test(bytes_received, 6);

    get_type_name(pkt_rcv.type, type_name);
    if(bytes_received > 0)
    {
        cout << "[TCP] Rcvd: " << type_name << endl; 
    }

    //if packet is join
    if (pkt_rcv.type == JOIN){
        // TCP: THE SERVER REPLIES JOIN_GRANT AND THE UDP SERVER PORT TO THE CLIENT
        char buf[32];
        short unsigned int udp_port = 0;
        int udp_server_fd = Create_udpServer(&udp_port);

        stringstream ss;
        ss << udp_port;
        memset(buf, 0, sizeof(buf));

        My_Packet pkt_send;
        pkt_send.type = JOIN_GRANT;
        memcpy(pkt_send.buffer, ss.str().c_str(), sizeof(ss.str().c_str()));

        bytes_sent = send(tcp_client_fd, &pkt_send , sizeof(pkt_send), 0);

        //check
        test(bytes_sent,7);

        // listen to the socket, wait for incoming connections
        listen(udp_server_fd, 1);
 
        // REMEMBER TO CLOSE THE TCP SOCKET
        test(close(tcp_client_fd),8);


        Bulls_And_Cows game;
        game.Restart_Game();
        while(1)
        {
            //GAME: START A NEW GAME IF THE PLAYER HAS ALL
            //   FOUR DIGITS CORRETLY
            My_Packet incoming_pkt;
            char type_name_incoming[16];

            // receive
            //bytes_received = recv(udp_server_fd, &incoming_pkt, sizeof(incoming_pkt), 0);

            bytes_received = recvfrom(udp_server_fd, &incoming_pkt, sizeof(recv_buf), 0, 
                                  (sockaddr *) &udp_client_addr, 
                                  &udp_client_addr_len);

           get_type_name(incoming_pkt.type, type_name_incoming);

            if(bytes_received < 0)
            {
                cerr << "[ERR] Error receiving message from server." << endl;
                exit(1);
            }
            else
            {
                cout << "[UDP] Rcvd: " << type_name_incoming << endl;
            }

            if(incoming_pkt.type == GUESS)
            {
                // GOT THE PORT NUMBER
                //int guess_num = 0;
                char guess[4];
                int bulls = 0;
                int cows = 0;
                int sn = 0;
                bool won = false;

                //memcpy(&guess_num,incoming_pkt.buffer, sizeof(guess_num));
                memcpy(&guess,incoming_pkt.buffer, 4);
                cout << "The guess is " << atoi(incoming_pkt.buffer)  << endl;
                won = game.Guess(guess,bulls,cows);
                game.get_secret_number(sn);
                cout << "bulls: " << bulls << " and cows " << cows << endl;

                cout << "did you win? " << won << endl;
                cout << "secret number " << sn << endl;

                My_Packet outgoing_pkt;
                outgoing_pkt.type = RESPONSE;

                stringstream ss_out;
                ss_out << bulls << "A" << cows << "B" ;
                memset(buf, 0, sizeof(buf));

                memcpy(outgoing_pkt.buffer, ss_out.str().c_str(), sizeof(ss.str().c_str()));

                //bytes_sent = sendto(tcp_client_fd, &outgoing_pkt, 
                  //              sizeof(outgoing_pkt), 0,
                    //            (sockaddr *) &udp_server_addr, 
                      //          sizeof(udp_server_addr));
                bytes_sent = sendto(udp_server_fd, &outgoing_pkt, sizeof(send_buf), 
                            0, (sockaddr *) &udp_client_addr, 
                            udp_client_addr_len);


                //check
                test(bytes_sent,7);

                char type_name_sent[16];

                get_type_name(outgoing_pkt.type, type_name_sent);
                if(bytes_sent > 0)
                {
                    cout << "[UDP] Sent: " << type_name_sent << endl;
                }


            }
            else if(incoming_pkt.type == EXIT)
            {
            
            }


            //UDP: RECV/SEND INTERACTIONS
        }
    }
}
