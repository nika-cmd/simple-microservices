/* 
 * Author: Thessalonika Magadia
 * File Name: indirect_server.c
 * 
 * Uses TCP for data transfer with the client. Directs client to
 * the micro service it requests
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

#define MAX_MESSAGE_LENGTH 255
#define MAX_WORD_LENGTH 10
#define PORTNUM 10000

#define TRANSLATOR_PORT 10001
#define CURRENCY_PORT 10002
#define VOTING_PORT 10003
#define SERVERIP "136.159.5.25"
#define FLAG_OK "OK"

int main (int argc, char *argv[]) {

    printf ("Welcome to the Indirection Server!");
    

    // initializing data
    int translator_socket, currency_socket, voting_socket, tcp_socket, tcp_socket_conn;
    char service_response[MAX_MESSAGE_LENGTH];
    char msg_from_client[MAX_MESSAGE_LENGTH], msg_to_client[MAX_MESSAGE_LENGTH];

    // TCP (server-client side)
    struct sockaddr_in address;
    memset (&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons (PORTNUM);
    address.sin_addr.s_addr = htonl (INADDR_ANY);

    if ( (tcp_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("call to tcp socket() failed!\n");
        exit(1);
    }

    if (bind (tcp_socket, (struct sockaddr *)&address, sizeof (struct sockaddr_in)) == -1) {
        perror ("call to bind() failed!\n");
        exit(1);
    }

    if (listen (tcp_socket, 5) == -1) {
        perror ("call to listen() failed!\n");
        exit(1);
    } else {
        printf (" Indirect server now listning on %s port %d...\n", SERVERIP, PORTNUM);
    }

    // UDP (client - micro service side)
    
    

    int done_service = 0;
    int bytes;
    while (1){
        if ( (tcp_socket_conn = accept (tcp_socket, NULL, NULL)) == -1) {
            perror ("call to accept() failed!");
            exit(1);
        } else {
            printf ("\nClient Connected.\n");
        }

        while (!done_service) {
            memset (msg_from_client, 0 , MAX_MESSAGE_LENGTH);
            memset (msg_to_client, 0, MAX_MESSAGE_LENGTH);
            memset (service_response, 0, MAX_MESSAGE_LENGTH);

            printf ("\nin da loop\n");

            // gets the service
            if ( (bytes =  recv (tcp_socket_conn, msg_from_client, MAX_MESSAGE_LENGTH, 0)) < 0) {
                perror ("call to recv failed!");
                exit(1);
            }  else {
                printf ("\nReceived from client(%d): %s", bytes, msg_from_client);
            }

            // sends "OK" to client
            if (send (tcp_socket_conn, "FLAG_OK", MAX_MESSAGE_LENGTH, 0) == -1) {
                    perror ("call to send failed!");
                    exit(1);
            }
            else {
                printf ("\nMessage sent to client: OK");
            }

            //
            
            if (strncmp (msg_from_client, "1", 1) == 0) {
                
                printf ("\nTranslation service selected.");

                // receives inputs from client
                if ( (bytes =  recv (tcp_socket_conn, msg_from_client, MAX_MESSAGE_LENGTH, 0)) < 0) {
                perror ("call to recv failed!");
                exit(1);
                }  else {
                    printf ("\nReceived from client(%d): %s", bytes, msg_from_client);
                }

                //udp
                if ( (translator_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
                    perror ("call to udp socket() failed!\n");
                    exit(1);
                }

                // setting timeout for translator socket
                // if no response after 2 seconds from UDP micro server
                struct timeval timeout;
                timeout.tv_sec = 2;
                timeout.tv_usec = 0;

                if (setsockopt (translator_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,  sizeof (timeout)) <0) {
                    perror ("setsockopt failed!\n");
                }
                //struct sockaddr *client;
                struct sockaddr_in ip_client;

                // initializing adress of translator server
                memset ( &ip_client, 0, sizeof(ip_client) );
                ip_client.sin_family = AF_INET;
                ip_client.sin_port = htons(TRANSLATOR_PORT);
                ip_client.sin_addr.s_addr = inet_addr (SERVERIP);
                //client = (struct sockaddr *) &ip_client;
                socklen_t client_struct_length = sizeof (ip_client);

                // send inputs to translator server (UDP)
                if ( sendto(translator_socket, msg_from_client, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *) &ip_client, sizeof(ip_client)) == -1) {
                    perror ("call to sendto() translation failed");
                    exit(1);
                }  else {
                    printf ("\nSent to translate server: %s", msg_from_client);
                }

                // receive inputs from translator server (UDP)
                if ( recvfrom(translator_socket, service_response, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *) &ip_client, &client_struct_length) == -1) {
                    perror ("call to recvfrom() failed");
                    exit(1);
                } else {
                    printf ("\nmessage recieved from server: %s", service_response);
                }

                if (send (tcp_socket_conn, service_response, MAX_MESSAGE_LENGTH, 0) == -1) {
                    perror ("call to send failed!");
                    exit(1);
                }
                else {
                    printf ("\nMessage sent to client: %s", service_response);
                }
                // clost socket
                close (translator_socket);
            } 
            else if (strncmp (msg_from_client, "2", 1) == 0) {

                printf ("\nCurrency converter service selected.\n");

                // recieves input from client 
                if ( (bytes =  recv (tcp_socket_conn, msg_from_client, MAX_MESSAGE_LENGTH, 0)) < 0) {
                    perror ("call to recv failed!");
                    exit(1);
                }  else {
                    printf ("\nReceived from client(%d): %s", bytes, msg_from_client);
                }

                if ( (currency_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
                    perror ("call to udp socket() failed!\n");
                    exit(1);
                }

                // setting timeout for currency converter socket
                // if no response after 2 seconds from UDP micro server
                struct timeval timeout;
                timeout.tv_sec = 2;
                timeout.tv_usec = 0;

                if (setsockopt (currency_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,  sizeof (timeout)) <0) {
                    perror ("setsockopt failed!\n");
                }
                //struct sockaddr *client;
                struct sockaddr_in ip_client;

                // initializing adress of translator server
                memset ( &ip_client, 0, sizeof(ip_client) );
                ip_client.sin_family = AF_INET;
                ip_client.sin_port = htons(CURRENCY_PORT);
                ip_client.sin_addr.s_addr = inet_addr (SERVERIP);
                //client = (struct sockaddr *) &ip_client;
                socklen_t client_struct_length = sizeof (ip_client);

                // send inputs to translator server (UDP)
                if ( sendto(currency_socket, msg_from_client, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *) &ip_client, sizeof(ip_client)) == -1) {
                    perror ("call to sendto() translation failed");
                    exit(1);
                }  else {
                    printf ("\nSent to translate server: %s", msg_from_client);
                }

                // receive inputs from translator server (UDP)
                if ( recvfrom(currency_socket, service_response, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *) &ip_client, &client_struct_length) == -1) {
                    perror ("call to recvfrom() failed");
                    exit(1);
                } else {
                    printf ("\nmessage recieved from server: %s", service_response);
                }

                // send response to client (TCP)
                if (send (tcp_socket_conn, service_response, MAX_MESSAGE_LENGTH, 0) == -1) {
                    perror ("call to send failed!");
                    exit(1);
                }
                else {
                    printf ("\nMessage sent to client: %s", service_response);
                }
                
            }
            else if (strncmp (msg_from_client, "3", 1) == 0) {
                
                printf ("\nVoting service selected.\n");

                // recieves task request from client 
                if ( (bytes =  recv (tcp_socket_conn, msg_from_client, MAX_MESSAGE_LENGTH, 0)) < 0) {
                    perror ("call to recv failed!");
                    exit(1);
                }  else {
                    printf ("\nReceived from client(%d): %s", bytes, msg_from_client);
                }


                if ( (voting_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
                    perror ("call to udp socket() failed!\n");
                    exit(1);
                }

                // setting timeout for voting socket
                // if no response after 2 seconds from UDP micro server
                struct timeval timeout;
                timeout.tv_sec = 2;
                timeout.tv_usec = 0;

                if (setsockopt (voting_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,  sizeof (timeout)) <0) {
                    perror ("setsockopt failed!\n");
                }

                //struct sockaddr *client;
                struct sockaddr_in ip_client;

                // initializing adress of voting server
                memset ( &ip_client, 0, sizeof(ip_client) );
                ip_client.sin_family = AF_INET;
                ip_client.sin_port = htons(VOTING_PORT);
                ip_client.sin_addr.s_addr = inet_addr (SERVERIP);
                //client = (struct sockaddr *) &ip_client;
                socklen_t client_struct_length = sizeof (ip_client);

                // send inputs to translator server (UDP)
                if ( sendto(voting_socket, msg_from_client, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *) &ip_client, sizeof(ip_client)) == -1) {
                    perror ("call to sendto() translation failed");
                    exit(1);
                }  else {
                    printf ("\nSent to translate server: %s", msg_from_client);
                }

                if ( (strncmp(msg_from_client, "A", 1) == 0) || (strncmp(msg_from_client, "C", 1) == 0)) {
                    // receive inputs from translator server (UDP)
                    if ( recvfrom(voting_socket, service_response, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *) &ip_client, &client_struct_length) == -1) {
                        perror ("call to recvfrom() failed");
                        exit(1);
                    } else {
                        printf ("\nmessage recieved from server: %s", service_response);
                    }

                    // send response to client (TCP)
                    if (send (tcp_socket_conn, service_response, MAX_MESSAGE_LENGTH, 0) == -1) {
                        perror ("call to send failed!");
                        exit(1);
                    }
                    else {
                        printf ("\nMessage sent to client: %s", service_response);
                    }
                } 
                else if (strncmp (msg_from_client, "B", 1) == 0) {
                    // receive encryption key from server (UDP)
                    if ( recvfrom(voting_socket, service_response, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *) &ip_client, &client_struct_length) == -1) {
                        perror ("call to recvfrom() failed");
                        exit(1);
                    } else {
                        printf ("\nmessage recieved from server: %s", service_response);
                    }

                    // send encryption key to client (TCP)
                    if (send (tcp_socket_conn, service_response, MAX_MESSAGE_LENGTH, 0) == -1) {
                        perror ("call to send failed!");
                        exit(1);
                    }
                    else {
                        printf ("\nMessage sent to client: %s", service_response);
                    }

                    // receive encrypted vote (TCP)
                    if ( recv (tcp_socket_conn, msg_from_client, MAX_MESSAGE_LENGTH, 0) == -1) {
                        perror ("call to recvfrom() failed");
                        exit(1);
                    } else {
                        printf ("\nmessage recieved from server: %s", msg_from_client);
                    }

                    // send encrypted vote to micro (UDP)
                    if ( sendto(voting_socket, msg_from_client, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *) &ip_client, sizeof(ip_client)) == -1) {
                        perror ("call to sendto() translation failed");
                        exit(1);
                    }  else {
                        printf ("\nSent to translate server: %s", msg_from_client);
                    }

                    // receive if vote was successful
                    if ( recvfrom(voting_socket, service_response, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *) &ip_client, &client_struct_length) == -1) {
                        perror ("call to recvfrom() failed");
                        exit(1);
                    } else {
                        printf ("\nmessage recieved from server: %s", service_response);
                    }

                    // send to client
                    if (send (tcp_socket_conn, service_response, MAX_MESSAGE_LENGTH, 0) == -1) {
                        perror ("call to send failed!");
                        exit(1);
                    }
                    else {
                        printf ("\nMessage sent to client: %s", service_response);
                    }
                }
                
                close(voting_socket);
                
            }
        }
           
    }
    close (translator_socket);
    close (currency_socket);
    close (voting_socket);
    close (tcp_socket);
    close (tcp_socket_conn);

    exit(0);

}