// client.c

// ./client coordinator_ip coordinator_port

// running on local:
// ./client 127.0.0.1 8000


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include "nimble_structs.h"

// need to know the coordinator's IP and port in order to send messages.
#define DEFAULT_COORDINATOR_IP "127.0.0.1"  
#define DEFAULT_COORDINATOR_PORT 8000 

int main(int argc, char *argv[]) {
    // have to pass in two argument, the first one is ip address and the second one is port number
    if (argc < 3) {
        fprintf(stderr, "%s does not pass in coordinator_ip coordinator_port\n", argv[0]);
        fprintf(stderr, "Please make sure to use the format ./client coordinator_ip coordinator_port\n");
        exit(1);
    }

    const char* coordinator_ip = argv[1];
    // TCP protocol provides 16 bits for the port number
    uint16_t coordinator_port = atoi(argv[2]);

    // a non negative socket file descriptor, if negative means the socket is error.
    int socket_file_descriptor;
    // a IPv4 address info struct . If need a IPv6, use sockaddr_in6 and sin_family AF_INET6
    struct sockaddr_in coord_addr;

    // create a socket
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file_descriptor < 0) {
        perror("Error opening socket");
        exit(1);
    }

    // set the coordinator address.
    coord_addr.sin_family = AF_INET; // set socket internet family to IPv4 address
    coord_addr.sin_port = htons(coordinator_port); // htons: host to network short
    if (inet_pton(AF_INET, coordinator_ip, &coord_addr.sin_addr) <= 0) { // inet_pton: Internet Presentation to Network. Translate the ip address to binary and store in coord_addr.sin_addr
        perror("Invalid coordinator IP address");
        close(socket_file_descriptor);
        exit(1);
    }

    // connect to socket
    if (connect(socket_file_descriptor, (struct sockaddr* )&coord_addr, sizeof(coord_addr)) < 0) {
        perror("Error connecting to coordinator");
        close(socket_file_descriptor);
        exit(1);
    }

    //TODO: construct other type of messages.
    // construct the message that need to be updated.
    srand(time(0));
    ClientNewLedgerPayload new_ledger_message;
    // a new ledger, generate with a random num.
    Ledger l;
    l.x = rand();
    new_ledger_message.request = NEW_LEDGER;
    new_ledger_message.ledger = l;

    // send the new message to coordinator.
    write(socket_file_descriptor, &new_ledger_message, sizeof(new_ledger_message));

    // receive ack from coordinator.
    char ack_msg[256];
    int n = read(socket_file_descriptor, ack_msg, sizeof(ack_msg));
    // read() return bytes read and store the message in ack_msg
    // if 0 means the connection is closed
    // if -1 means error happend.

    if (n > 0 && n < 256) {
        ack_msg[n] = '\0';
        printf("Received acknowledgment from coordinator: %s\n", ack_msg);
    }
    else if (n == 0) {
        fprintf(stderr, "The connection to coordinator has been closed\n");
        close(socket_file_descriptor); 
        exit(1);
    }
    else {
        perror("Error receiving the ack_msg"); 
        close(socket_file_descriptor);
        exit(1);
    }

    close(socket_file_descriptor);
    return 0;
}



// Update
// • Issue append(l, B, c + 1) and get receipt, where B = (S′, σ), and σ = Sign(sk, (l, S′, c + 1)).
// • If the append succeeds and receipt is valid, update c ← c + 1, else follow the steps in the read protocol.

// Read protocol. When an application wishes to retrieve persistent state, it does the following:
// • nonce ← random() // e.g., 128 bits
// • (i,B,receipt)←read_latest(l,nonce)
// • ParseBas(S,σ).
// • IfVerify(vk,(l,S,i),σ)passesandreceiptisvalidwith
// respect to nonce, set c to i and use S as the state. If not, abort with Err(“rollback detected”).