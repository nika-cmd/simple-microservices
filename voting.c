/* 
 * Author: Thessalonika Magadia
 * File Name: voting.c
 * References:
 */

/* Include files for C socket programming and stuff */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

#define SERVERIP "136.159.5.25"
#define PORTNUM 10003
#define MAX_MESSAGE_LENGTH 255
#define MAX_NAME_LENGTH 20
#define ENCRYPTION_KEY 2

typedef struct candidate
{
    char name[MAX_NAME_LENGTH];
    const int id;
    int votes;
}candidate;


int main (int argc, char *argv[]) {
    int mySocket, readbytes;
    char message_in[MAX_MESSAGE_LENGTH], message_out[MAX_MESSAGE_LENGTH];
    candidate list[4] = {
        {"Buttercup", 1112, 123},
        {"Blossom", 2222, 45},
        {"Bubbles",  3332, 76},
        {"Mojo", 4442, 82}
    };

    struct sockaddr_in ip_server, ip_client;
    struct sockaddr *server, *client;
    memset ( (char*) &ip_server, 0, sizeof(ip_server) );
    ip_server.sin_family = AF_INET;
    ip_server.sin_port = htons(PORTNUM);
    //ip_server.sin_addr.s_addr = htonl(SERVERIP);
    ip_server.sin_addr.s_addr = inet_addr (SERVERIP);
    server = (struct sockaddr *) &ip_server;

    if ( (mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror ("call to socket() in voting.c failed!\n");
        exit(1);
    }

    if ( bind(mySocket, server, sizeof(ip_server)) == -1 ){
        perror ("call to bind() in voting.c failed!\n");
        exit(1);
    }

    // seting up sending/receiving 
    client = (struct sockaddr *) &ip_client;
    socklen_t client_struct_length = sizeof (ip_client);
    
    // send initial message
    printf ("Welcome to voting server! Server is now sending and receiving data...\n");
    
    // Tasks
    /* 'A' - Show Candidates
     * 'B' - Secure Voting
     * 'C' - Voting Summary
     */
    
    int bytes;
    int done = 0;
    int voted = 0;
    // change so can last thing must be new line
    while (!done){

        if ( recvfrom(mySocket, message_in, MAX_MESSAGE_LENGTH, 0, client, &client_struct_length) < 0 ) {
            perror ("recvfrom() call in voting.c failed!\n");
            exit (-1);
        } 
        else {
            printf ("Message recieved form client: %s\n", message_in);
        }

        if (strncmp(message_in, "A", 1) == 0) {
            // list of candidates
            sprintf (message_out, "\nCANDIDATES: <id> <name>"
                "\n%d %s \n%d %s \n%d %s \n%d %s \n",
                list[0].id, list[0].name, list[1].id, list[1].name,
                list[2].id, list[2].name, list[3].id, list[3].name);
        }
        else if (strncmp(message_in, "B", 1) == 0) {
            /* if (voted == 1) {
                sprintf (message_out, "\nAlready voted! Cannot vote again");
            } */
            //else {
                sprintf (message_out, "%d", ENCRYPTION_KEY);

                // send key
                if (sendto(mySocket, message_out, MAX_MESSAGE_LENGTH, 0, client, client_struct_length) == -1 ) {
                    perror ("sendto() call in voting.c failed!\n");
                    exit(-1);
                } 

                // recieve encrypted vote
                if ( recvfrom(mySocket, message_in, MAX_MESSAGE_LENGTH, 0, client, &client_struct_length) < 0 ) {
                    perror ("recvfrom() call in voting.c failed!\n");
                    exit (-1);
                } 
                else {
                    printf ("Message recieved form client: %s\n", message_in);
                }

                // decrypt vote
                int encrypted_vote;
                int vote_id;
                sscanf (message_in, "%d",&encrypted_vote);
                vote_id = encrypted_vote / ENCRYPTION_KEY;

                int valid;
                // see if valid id & update votes
                if (vote_id == list[0].id) {
                    list[0].votes += 1;
                    valid = 1;
                    voted = 1;
                }
                else if (vote_id == list[1].id) {
                    list[1].votes += 1;
                    valid = 1;
                    voted = 1;
                }
                else if (vote_id == list[2].id) {
                    list[2].votes += 1;
                    valid = 1;
                    voted = 1;
                }
                else if (vote_id == list[3].id) {
                    list[3].votes += 1;
                    valid = 1;
                    voted = 1;
                }
                else {
                    valid = 0;
                }

                // send back YES or NO if vote successful or not
                if (valid) {
                    printf ("\nid received valid.");
                    sprintf (message_out, "YES");
                } else {
                    printf ("\nid received invalid.");
                    sprintf (message_out, "NO");
                }
                
        }
        else if (strncmp(message_in, "C", 1) == 0) {
            if (voted == 1) {
                sprintf (message_out, "\nVoting results! <id> <name> <# of votes>"
                    "\n%d %s %d \n%d %s %d \n%d %s %d \n%d %s %d \n",
                    list[0].id, list[0].name, list[0].votes, list[1].id, list[1].name, list[1].votes,
                    list[2].id, list[2].name, list[2].votes, list[3].id, list[3].name, list[3].votes);
            } else {
                sprintf (message_out, "\nmust vote first!\n");
            }

        }
        else {
            sprintf (message_out, "Error: not a valid request. Please try again.\n");
        }

        //send 
        printf ("\nSent to indirection server: %s\n", message_out);

        // sending (final) data of task
        if (sendto(mySocket, message_out, MAX_MESSAGE_LENGTH, 0, client, client_struct_length) == -1 ) {
            perror ("sendto() call in voting.c failed!\n");
            exit(-1);
        } 

        // clearing buffers
        memset (message_in, 0, MAX_MESSAGE_LENGTH);
        memset (message_out, 0, MAX_MESSAGE_LENGTH);

    }

}


