#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include "network.h"
#include "connection.h"

int initialSocket;
char *home;
pid_t pid;

/**
 * Check that the program arguments were correct.
 *
 * @param argc The number of arguments.
 * @param argv The array of arguments.
 */
void checkRunParams(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage %s port homedir\n", argv[0]);
        exit(1);
    }
}

/**
 * Handle signals.
 *
 * @param signo The signal number.
 */
void sig_handler(int signo) {
    if (signo == SIGINT) {
        if (pid != 0) {
            // Inform of interrupt being received.
            printf("\nReceived SIGINT\n");

            wait(NULL);

            // Close the initial socket.
            printf("Closing socket...\n");
            if (close(initialSocket) != 0) {
                fprintf(stderr, "Failed to close socket.\n");
            }

            // Exit the program cleanly after threads.
            exit(0);
        } else {
            exit(0);
        }
    }
}

/**
 * Handle processes based on parent/child.
 *
 * @param clientSocket The socket that the client is on.
 */
void handleProcess(int clientSocket) {
    pid = fork();

    // Check that forking didn't return an error.
    if (pid < 0) {
        fprintf(stderr, "Failed to fork.");
        return;
    }

    if (pid == 0) { // Child process.
        // The child process doesn't need the listening socket.
        close(initialSocket);

        // Handle the client's request.
        handleRequest(clientSocket, home);

        // Request handled, cleanup process.
        close(clientSocket);
        exit(0);
    } else { // Parent Process.
        // Parent doesn't need the new connection socket.
        close(clientSocket);
    }
}

/**
 * Accept connections to new sockets.
 */
void acceptConnections() {
    int clientSocket;

    // Create a new socket for a connecting client.
    while ((clientSocket = acceptConnection(initialSocket))) {
        handleProcess(clientSocket);

        // Check if an interrupt signal has been given and cleanup if so.
        if (signal(SIGINT, sig_handler) == SIG_ERR) {
            printf("Failed to handle SIGINT.\n");
        }
    }
}

int main(int argc, char *argv[]) {
    // Check that correct arguments were given.
    checkRunParams(argc, argv);

    // Set the home directory
    home = argv[2];

    // Initialise a socket listening on the given port.
    initialSocket = init(argv[1]);

    // Listen on the socket.
    if (listen(initialSocket, 20) < 0) {
        error("Failed listening on the socket");
    }

    // Accept client connections.
    acceptConnections();

    // Close the initial socket.
    close(initialSocket);

    return 0;
}