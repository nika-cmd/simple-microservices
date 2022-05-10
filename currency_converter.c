/* 
 * Author: Thessalonika Magadia
 * File Name: currency_converter.c
 * 
 * Converts Canadian Dollar to another currency, either
 * US Dollar, Euro, British Pound, or Bitcoin
 * 
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
#define PORTNUM 10002
#define MAX_MESSAGE_LENGTH 255

// currency conversions
#define CAD_TO_USD 0.81
#define CAD_TO_EURO 0.70
#define CAD_TO_BPOUND 0.59
#define CAD_TO_BITCOIN 0.000013

int main (int argc, char *argv[]) {

    int mySocket, readbytes;
    char message_in[MAX_MESSAGE_LENGTH], message_out[MAX_MESSAGE_LENGTH];
    char src_currency[5], dst_currency[5], str_money_src[MAX_MESSAGE_LENGTH];
    struct sockaddr_in ip_server, ip_client;
    struct sockaddr *server, *client;
    memset ( (char*) &ip_server, 0, sizeof(ip_server) );
    ip_server.sin_family = AF_INET;
    ip_server.sin_port = htons(PORTNUM);
    //ip_server.sin_addr.s_addr = htonl(SERVERIP);
    ip_server.sin_addr.s_addr = inet_addr (SERVERIP);
    server = (struct sockaddr *) &ip_server;

    if ( (mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror ("call to socket() in currency_converter.c failed!\n");
        exit(1);
    }

    if ( bind(mySocket, server, sizeof(ip_server)) == -1 ){
        perror ("call to bind() in currency_converter.c failed!\n");
        exit(1);
    }

    // seting up sending/receiving 
    client = (struct sockaddr *) &ip_client;
    socklen_t client_struct_length = sizeof (ip_client);

    printf ("\nWelcome to Currency Converter Micro Server! Server sending and recieving data..\n\n");
    
    int bytes;
    int done = 0;
    double money_src;
    double money_dst;
    // change so can last thing must be new line
    while (!done){
        if ( recvfrom(mySocket, message_in, MAX_MESSAGE_LENGTH, 0, client, &client_struct_length) < 0 ) {
            perror ("recvfrom() call in currency_converter.c failed!\n");
            exit (-1);
        } 
        else {
            printf ("Message recieved form client: %s\n", message_in);
        }

        // parse through input 
        sscanf (message_in, "%s %s %lf", src_currency, dst_currency, &money_src);
        printf ("\nsource currency: %s", src_currency);
        printf ("\ndestination currency: %s", dst_currency);
        printf ("\nvalue: %lf", money_src);


        // see if inputs are valid
        if (strncmp(src_currency, "CAD", 3) == 0) {

            if (strncmp(dst_currency, "USD", 3) == 0) {
                money_dst = money_src * CAD_TO_USD;
                sprintf (message_out, "$%lf Canadian Dollars is $%lf US Dollars\n", money_src, money_dst);
            }
            else if (strncmp(dst_currency, "EUR", 3) == 0) {
                money_dst = money_src * CAD_TO_EURO;
                sprintf (message_out, "$%lf Canadian Dollars is €%lf Euros\n", money_src, money_dst);
            }
            else if (strncmp(dst_currency, "GBP",  3) == 0) {
                money_dst = money_src * CAD_TO_BPOUND;
                sprintf (message_out, "$%lf Canadian Dollars is £%lf British Pounds", money_src, money_dst);
            }
            else if (strncmp(dst_currency, "BTC", 3) == 0) {
                money_dst = money_src * CAD_TO_BITCOIN;
                sprintf (message_out, "$%lf Canadian Dollars is ฿%lf Bitcoin\n", money_src, money_dst);
            }
            else {
                sprintf (message_out, "\nError: destination currency not supported. Please try again.\n");
            }

        } 
        else {
            sprintf (message_out, "\nError: source currency not supported. Please try again.\n");
        }


        //send 
        printf ("\nSent to indirection server: %s\n", message_out);

        // sending data
        if (sendto(mySocket, message_out, strlen(message_out), 0, client, client_struct_length) == -1 ) {
            perror ("sendto() call in currency_converter.c failed!\n");
            exit(-1);
        }

        // clearing buffers
        memset (message_in, 0, MAX_MESSAGE_LENGTH);
        memset (message_out, 0, MAX_MESSAGE_LENGTH);
        memset (str_money_src, 0, MAX_MESSAGE_LENGTH);
        memset (src_currency, 0, 5);
        memset (dst_currency, 0, 5);
    }
    
    close (mySocket);
    exit(0);
    
}