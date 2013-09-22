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
        // wipe out anything in send_buf
        memset(send_buf, 0, sizeof(send_buf));
        cout << "Ctrl + D to quit" << endl << ">";
        cin >> send_buf;
        if(cin.eof() == true)
        {
            if(close(tcp_connection_fd) < 0){
                cerr << "[ERR] Error when closing TCP socket" << endl;
            }
            break;
        }
        bytes_sent = send(tcp_connection_fd, send_buf, sizeof(send_buf), 0);
        //bytes_sent = write(tcp_connection_fd, send_buf, sizeof(send_buf));

        if(bytes_sent < 0)
        {
            cerr << "[ERR] Error sending message to server." << endl;
            exit(1);
        }
        else
        {
            cout << "[TCP] Sent: " << send_buf << endl;
        }


        // GOT THE PORT NUMBER
        // EXIT THE LOOP AND CONTINUES TO UDP GAME PLAY
        //
        // wipe out anything in recv_buf
        memset(recv_buf, 0, sizeof(recv_buf));
        // receive
        bytes_received = recv(tcp_connection_fd, recv_buf, sizeof(recv_buf), 0);
        // can also use read
        // bytes_received = read(tcp_connection_fd, recv_buf, sizeof(recv_buf));

        // check
        if(bytes_received < 0)
        {
            cerr << "[ERR] Error receiving message from server." << endl;
            exit(1);
        }
        else
        {
            cout << "[TCP] Rcvd: " << recv_buf << endl;
        }

        if(close(tcp_connection_fd) < 0)
        {
            cerr << "[ERR] Error when closing connection socket." << endl;
            exit(1);
        }

    }

    // Game play using UDP
    //cout << "[UDP] Guesses will be sent to: " << server_name_str 
         //<< " at port:" << udp_server_port << endl;

    while(1)
    {

        // GAME PLAY IN UDP
        //
        //while(get_command(outgoing_pkt) == false){}

        // UDP: SEND/RECV INTERACTIONS
    }

    return 0;
}

