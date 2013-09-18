#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <iostream>
#include <sstream>
#include <cstring>

#include "packet.h" // defined by us
#include "Bulls_And_Cows.h"
#include "lab1_server.h"

using namespace std;

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

    while(1){
        // TCP: ACCETP NEW INCOMING CONNECTION/CLIENT

        // FOR EACH NEW CONNECTION / CLIENT
        // CREATE / FORK A CHILD PROCESS TO HANDLE IT
        pid = fork();
        // REMEMBER TO CHECK ALL RETURN VALUES

        // FOR CHILD PROCESS
        //     CREATE A UDP SERVER FOR THE GAME
        //     FOR EACH CHILD WE INVOKE A HANDLER FOR IT
        //     SampleHandler
        // FOR PARENT PROCESS
        //     CONTINUE THE LOOP TO ACCEPT NEW CONNECTION/CLIENT
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

