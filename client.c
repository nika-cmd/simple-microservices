/* 
 * Author: Thessalonika Magadia
 * File Name: client.c
 * Makes a TCP connection with the Indirection Server 
 * References:
 *  [1] https://www.includehelp.com/c/c-program-to-read-string-with-spaces-using-scanf-function.aspx (Accessed: 22 Oct 2021)
 *  
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
#define SERVERIP "136.159.5.25"

int main (int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "usage: <UDP Server IP> <UDP Server Port>\n");
        exit(1);
    }
    int portnum;

    sscanf (argv[2], "%d", &portnum);

    printf ("\nUDP server ip: %s", argv[1]);
    printf ("\nUDP server portnum: %d", portnum);

    // initializing data
    int client_sock;
    char msg_to_server[MAX_MESSAGE_LENGTH], msg_from_server[MAX_MESSAGE_LENGTH];
    char service[MAX_MESSAGE_LENGTH];

    // getting Indirection Server's IP and portnumber
    /* const char* indirect_serverip = argv[1];
    int indirect_portnum;
    sscanf (argv[2], "%d", &indirect_portnum); */

    struct sockaddr_in address;
	memset (&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(portnum);
	address.sin_addr.s_addr = inet_addr (argv[1]);

    if ( ( client_sock = socket (PF_INET, SOCK_STREAM, 0)) == -1 ) {
        perror ("socket() in client.c call failed!\n");
        exit(1);
    } 
    else {
        printf ("\nsocket created.\n");
    }

    if ( connect(client_sock, (struct sockaddr *)&address, sizeof (struct sockaddr_in)) ){
        perror ("connect() in client.c call failed!\n");
        exit(1);
    }
    else {
        printf ("connection to indirection server established.\n");
    }

    int running = 1;
    int new_service = 1;
    int bytes;
    while (running) {

        memset (msg_to_server, 0, MAX_MESSAGE_LENGTH);
        memset (msg_from_server, 0, MAX_MESSAGE_LENGTH);
        memset (service, 0, MAX_MESSAGE_LENGTH);

        // initial message to select service
        if (new_service) {
            printf ("\nPlease select a service, options are: "
                "\n1 - translator \n2 - currency converter \n3 - voting \n0 - exit program"
                "\n> Service Request: ");
            new_service = 0;
        }

        
        scanf ("%s", msg_to_server);
        strcpy (service, msg_to_server);


        if (strncmp(service, "0", 1) == 0) {
            printf ("\nExiting Program...\n\n");
            running = 0;
            break;
        }
        
            
        // sends selected service to indirection server
        if ((bytes = send (client_sock, msg_to_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
            perror ("call to send() failed");
            exit(1);
        } else {
            printf ("\nsent to server(%d): %s",bytes, msg_to_server);
        }
        

        // receives "OK"
        if ((bytes = recv (client_sock, msg_from_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
            perror ("call to recv() failed");
            exit(1);
        }
        else {
            printf ("\n%s", msg_from_server);
        }
        
        // getting client inputs for micro service
        if (strncmp (service, "1", 1) == 0) {
            printf ("\n> Enter English word: ");
            scanf ("%s", msg_to_server);

            // sends input to server
            if ((bytes = send (client_sock, msg_to_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
                perror ("call to send() failed");
                exit(1);
            } else {
                printf ("\nsent to server(%d): %s",bytes, msg_to_server);
            }

            // receives result from server
            if ((bytes = recv (client_sock, msg_from_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
                perror ("call to recv() failed");
                exit(1);
            } else {
                printf ("\n%s", msg_from_server); 
                new_service = 1;
            }
        }
        else if (strncmp (service, "2", 1) == 0) {
            // [1] reading a string with whitespace from terminal
            char temp;
            printf ("\n> Enter <SRC> <DST> <VALUE>: ");
            scanf ("%c", &temp);
            fgets(msg_to_server, MAX_MESSAGE_LENGTH, stdin); 

            // sends input to server
            if ((bytes = send (client_sock, msg_to_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
                perror ("call to send() failed");
                exit(1);
            } else {
                printf ("\nsent to server(%d): %s",bytes, msg_to_server);
            }

            // receives result from server
            if ((bytes = recv (client_sock, msg_from_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
                perror ("call to recv() failed");
                exit(1);
            } else {
                printf ("\n%s", msg_from_server); 
                new_service = 1;
            }

        } 
        else if (strncmp (service, "3", 1) == 0) {
            printf ("\nSelect an option: "
                "\nA - show candidates \nB - secure vote \nC - show results"
                "\n> Task Request: ");
                scanf ("%s", msg_to_server);

            // sends task requested
            if ((bytes = send (client_sock, msg_to_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
                perror ("call to send() failed");
                exit(1);
            } else {
                printf ("\nsent to server(%d): %s",bytes, msg_to_server);
            }

            if (strncmp(msg_to_server, "A", 1) == 0) {
                if ((bytes = recv (client_sock, msg_from_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
                    perror ("call to recv() failed");
                    exit(1);
                } else {
                printf ("\n%s", msg_from_server); 
                new_service = 1;
                }
            } 
            else if (strncmp(msg_to_server, "B", 1) == 0) {
                // receives encryption key
                if ((bytes = recv (client_sock, msg_from_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
                    perror ("call to recv() failed");
                    exit(1);
                } else {
                printf ("\nEncryption key: %s", msg_from_server); 
                }
                char str_vote[MAX_MESSAGE_LENGTH];
                int vote_id;
                int key;
                int encrypt_key;
                
                sscanf (msg_from_server, "%d", &key); // gets string key and puts it into int
                printf ("\nVote: ");
                scanf ("%s", str_vote); // gets vote as string
                sscanf (str_vote, "%d", &vote_id); // turns string id to int
                
                encrypt_key = key * vote_id;

                sprintf (msg_to_server, "%d", encrypt_key);

                // send key to server
                if ((bytes = send (client_sock, msg_to_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
                    perror ("call to send() failed");
                    exit(1);
                } else {
                    printf ("\nsent to server(%d): %s",bytes, msg_to_server);
                }

                // receive confirmation voted?
                if ((bytes = recv (client_sock, msg_from_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
                    perror ("call to recv() failed");
                    exit(1);
                } else {
                    printf ("\nEncryption key: %s", msg_from_server); 
                }
                if (strncmp(msg_from_server, "YES", 3) == 0)  {
                    printf ("\nVoted Submitted Successfully!");
                } 
                else if (strncmp(msg_from_server, "NO", 2) == 0) {
                    printf ("\nError: id was invalid so vote was unsuccessful.");
                }
                new_service = 1;
            }
            else if (strncmp(msg_to_server, "C", 1) == 0) {
                if ((bytes = recv (client_sock, msg_from_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
                    perror ("call to recv() failed");
                    exit(1);
                } else {
                    printf ("\n%s", msg_from_server); 
                    new_service = 1;
                }
            }
            else {
                printf ("\nNot a valid selection. Please try again.\n");
                new_service = 1;
            }

            
        }
        else {
            printf ("\nNot a valid selection. Please try again.\n");
            new_service = 1;
            //continue;
        }
        
        /* // sends input to server
        if ((bytes = send (client_sock, msg_to_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
            perror ("call to send() failed");
            exit(1);
        } else {
            printf ("\nsent to server(%d): %s",bytes, msg_to_server);
        }

        // receives output from server
        if ((bytes = recv (client_sock, msg_from_server, MAX_MESSAGE_LENGTH, 0)) == -1) {
            perror ("call to recv() failed");
            exit(1);
        } else {
            printf ("\n%s", msg_from_server); 
            new_service = 1;
        } */

    }

    close (client_sock);
    return 0;
}