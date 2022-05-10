/* 
 * Author: Thessalonika Magadia
 * File Name: translator.c
 * 
 * Translate English word to French. Only supports the 
 * following words: hello, apple, computer, water, and goodbye
 * 
 * References:
 * https://www.educative.io/edpresso/how-to-implement-udp-sockets-in-c (accessed: Oct 20, 2021)
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
#define PORTNUM 10001
#define MAX_MESSAGE_LENGTH 255
#define FRENCH_PREFIX "> French translation:"


int main (int argc, char *argv[]) {

    int mySocket, readbytes;
    char message_in[MAX_MESSAGE_LENGTH], message_out[MAX_MESSAGE_LENGTH];

    struct sockaddr_in ip_server, ip_client;
    struct sockaddr *server, *client;
    memset ( (char*) &ip_server, 0, sizeof(ip_server) );
    ip_server.sin_family = AF_INET;
    ip_server.sin_port = htons(PORTNUM);
    //ip_server.sin_addr.s_addr = htonl(SERVERIP);
    ip_server.sin_addr.s_addr = inet_addr (SERVERIP);
    server = (struct sockaddr *) &ip_server;

    if ( (mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror ("call to socket() in translator.c failed!\n");
        exit(1);
    }

    if ( bind(mySocket, server, sizeof(ip_server)) == -1 ){
        perror ("call to bind() in translator.c failed!\n");
        exit(1);
    }

    // seting up sending/receiving 
    client = (struct sockaddr *) &ip_client;
    socklen_t client_struct_length = sizeof (ip_client);

    printf ("\nWelcome to Translator Micro Server! Server sending and recieving data..\n\n");
    
    int bytes;
    int done = 0;
    // change so can last thing must be new line
    while (!done){
        if ( recvfrom(mySocket, message_in, MAX_MESSAGE_LENGTH, 0, client, &client_struct_length) < 0 ) {
            perror ("recvfrom() call in translator.c failed!\n");
            exit (-1);
        } 
        else {
            printf ("Message recieved form client: %s\n", message_in);
        }
        // translate "hello"
        if (strncmp (message_in, "hello", 5) == 0) {
            sprintf (message_out, "%s bonjour\n", FRENCH_PREFIX);
        }
        // translate "apple"
        else if (strncmp(message_in, "apple", 5) == 0){
            sprintf (message_out, "%s pomme\n", FRENCH_PREFIX);
        }
        // translate "computer"
        else if (strncmp(message_in, "computer", 8) == 0){
            sprintf (message_out, "%s ordinateur\n", FRENCH_PREFIX);
        }
        // translate "water"
        else if (strncmp(message_in, "water", 5) == 0){
            sprintf (message_out, "%s leau\n", FRENCH_PREFIX);
        }
        // translate "goodbye"
        else if (strncmp(message_in, "goodbye", 7) == 0){
            sprintf (message_out, "%s au revoir\n", FRENCH_PREFIX);
        }
        else {
            sprintf (message_out, "<<Error>> Word could not be found in library. Unable to translate.\n");
        }
        
        printf ("Sent to indirection server: %s\n", message_out);

        // sending data
        if (sendto(mySocket, message_out, strlen(message_out), 0, client, client_struct_length) == -1 ) {
            perror ("sendto() call in translator.c failed!\n");
            exit(-1);
        }

        // clear messagein and messageout buffer
        bzero (message_in, MAX_MESSAGE_LENGTH);
        bzero (message_out, MAX_MESSAGE_LENGTH);
        
        
    }

    close (mySocket);
    exit(0);

}