#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <sstream>
#include <cstring>

#include <netdb.h>
#include <unistd.h>

#include "packet.h"      // defined by us
#include "lab1_client.h" // some supporting functions.

using namespace std;

#define BUFLEN 256

// Function prototype for parameter parser.
int parse_argv(int argc, char *argv[], 
              char **tcp_server_name_str, unsigned short int &tcp_server_port);

int main(int argc, char *argv[])
{
    char                 *server_name_str = 0;
    unsigned short int   tcp_server_port;

    sockaddr_in tcp_server_addr;
    int         tcp_connection_fd;

    int         bytes_received;
    int         bytes_sent;

    char        recv_buf[BUFLEN + 20];
    char        send_buf[BUFLEN];

    unsigned short int udp_port = 0;
    // prase the argvs, obtain server_name and tcp_server_port
    parse_argv(argc, argv, &server_name_str, tcp_server_port);

    struct hostent *hostEnd = gethostbyname(server_name_str);

    cout << "[TCP] Bulls and Cows client started..." << endl;
    cout << "[TCP] Connecting to server: " << server_name_str
         << ":" << tcp_server_port << endl;

    while(1)
    {
        // TCP: CONNECT TO SERVER
        // create a TCP socket with protocol family AF_INET
        tcp_connection_fd = socket(AF_INET, SOCK_STREAM, 0);

        // if the return value is -1, the creation of socket is failed.
        if(tcp_connection_fd < 0)
        {
             cerr << "[ERR] Unable to create TCP socket." << endl;
             exit(1);
        }

        // initialize the socket address strcut by setting all bytes to 0
        memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));

        // details are covered in class/slides
        tcp_server_addr.sin_family = AF_INET;
        tcp_server_addr.sin_port   = htons(tcp_server_port);

        memcpy(&tcp_server_addr.sin_addr, hostEnd -> h_addr, 
               hostEnd -> h_length);

        if(connect(tcp_connection_fd, (sockaddr *)&tcp_server_addr, 
                   sizeof(tcp_server_addr)) < 0)
        {
            cerr << "[ERR] Unable to connect to server." << endl;
            if(close(tcp_connection_fd) < 0){
                cerr << "[ERR] Error when closing TCP socket" << endl;
            }
            exit(1);
        }


        // TCP: THE FIRST COMMAND MUST BE JOIN
        //      THE RESPONSE MUST BE JOIN_GRANT WITH A UDP PORT NUMBER
        //
        My_Packet pkt;
        pkt.type = JOIN;
        bytes_sent = send(tcp_connection_fd, &pkt, sizeof(pkt), 0);

        char type_name_sent[16];

        get_type_name(pkt.type, type_name_sent);

        if(bytes_sent < 0)
        {
            cerr << "[ERR] Error sending message to server." << endl;
            exit(1);
        }
        else
        {
            cout << "[TCP] Sent: " << type_name_sent << endl;
        }

        //Recieve packet from the server with udp port
        My_Packet pkt_rcv;
        char type_name[16];

        // receive
        bytes_received = recv(tcp_connection_fd, &pkt_rcv, sizeof(recv_buf), 0);

        get_type_name(pkt_rcv.type, type_name);
        if(bytes_received < 0)
        {
            cerr << "[ERR] Error receiving message from server." << endl;
            exit(1);
        }
        else
        {
            cout << "[TCP] Rcvd: " << type_name << endl;
        }

        if(pkt_rcv.type == JOIN_GRANT)
        {
            // GOT THE PORT NUMBER
            //unsigned short int udp_port = 0;
            memcpy(&udp_port,pkt_rcv.buffer, sizeof(udp_port));
            cout << "The udp port is " << atoi(pkt_rcv.buffer)  << endl;
            udp_port = atoi(pkt_rcv.buffer);
            // EXIT THE LOOP AND CONTINUES TO UDP GAME PLAY
            break;
        }

        // wipe out anything in recv_buf
        //memset(recv_buf, 0, sizeof(recv_buf));
        // receive
        //bytes_received = recv(tcp_connection_fd, recv_buf, sizeof(recv_buf), 0);

        // check
        //if(bytes_received < 0)
        //{
           //  cerr << "[ERR] Error receiving message from server." << endl;
            //exit(1);
       // }
        //else
       // {
         //   cout << "[TCP] Rcvd: " << recv_buf << endl;
        //}

        if(close(tcp_connection_fd) < 0)
        {
            cerr << "[ERR] Error when closing connection socket." << endl;
            exit(1);
        }

    }

    // Game play using UDP
    cout << "[UDP] Guesses will be sent to: " << server_name_str << " at port:" << udp_port << endl;

    int  udp_connection_fd;       // socket file descriptor
    sockaddr_in udp_connection_addr;     // socket addr for this process
    socklen_t   udp_connection_addr_len = sizeof(udp_connection_addr);


    // create a UDP socket with protocol family AF_INET
    udp_connection_fd = socket(AF_INET, SOCK_DGRAM, 0);

    // if the return value is -1, the creation of socket is failed.
    if(udp_connection_fd < 0)
    {
            cerr << "[ERR] Unable to create UDP socket." << endl;
            exit(1);
    }

    // initialize the socket address strcut by setting all bytes to 0
    memset(&udp_connection_addr, 0, sizeof(udp_connection_addr));

    udp_connection_addr.sin_family = AF_INET;
    udp_connection_addr.sin_port   = htons(udp_port);
    memcpy(&udp_connection_addr.sin_addr, hostEnd -> h_addr, hostEnd -> h_length);
    while(1)
    {
        My_Packet outgoing_pkt;
        char type_name[16];

        // GAME PLAY IN UDP
        //
        while(get_command(outgoing_pkt) == false){}

        bytes_sent = sendto(udp_connection_fd, &outgoing_pkt, 
                                sizeof(outgoing_pkt), 0,
                                (sockaddr *) &udp_connection_addr, 
                                sizeof(udp_connection_addr));

        get_type_name(outgoing_pkt.type, type_name);
        if(bytes_sent < 0)
        {
            cerr << "[ERR] Error sending message to server." << endl;
            exit(1);
        }
        else
        {
            cout << "[UDP] Sent: " << type_name << endl;
        }

        // UDP: SEND/RECV INTERACTIONS
        //
        //
        My_Packet incoming_pkt;
        char type_name_incoming[16];

        // receive
        bytes_received = recv(udp_connection_fd, &incoming_pkt, sizeof(incoming_pkt), 0);

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
    }

    return 0;
}


