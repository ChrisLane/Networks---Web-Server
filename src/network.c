#include <string.h>
#include <sys/socket.h>
#include <dnet.h>
#include <stdio.h>
#include <stdlib.h>

#include "network.h"

/**
 * Print an error and exit with a given message.
 *
 * @param msg The error message.
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}

/**
 * Initialise a socket for a client to connect to.
 *
 * @param initialSocket The initial socket number.
 * @param port The port to listen on.
 */
int init(const char *port) {
    int initialSocket;
    struct addrinfo hints, *res;

    // Set server details.
    memset((char *) &hints, 0, sizeof(hints));
    hints.ai_addr = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, port, &hints, &res);

    // Make a socket.
    if ((initialSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        error("Failed to create the initial socket");
    }

    // lose the pesky "Address already in use" error message
    int yes = 1;
    if (setsockopt(initialSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0) {
        error("Failed to set socket option. Port may stay bound for a while");
    }

    // Bind the socket to the set details.
    if (bind(initialSocket, res->ai_addr, res->ai_addrlen) < 0) {
        error("Failed to bind the socket");
    }

    // Free the address info.
    freeaddrinfo(res);

    return initialSocket;
}

/**
 * Accept a client and move them onto a new socket.
 *
 * @param initialSocket The main socket that we run on.
 * @return Return the client socket.
 */
int acceptConnection(int initialSocket) {
    struct sockaddr_in clientAddress;
    socklen_t clientLength = sizeof(clientAddress);

    // Accept the client to a new socket.
    int socket = accept(initialSocket, (struct sockaddr *) &clientAddress, &clientLength);

    if (socket < 0) {
        error("Failed to create new accepting socket");
    }

    return socket;
}
