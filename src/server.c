#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <dnet.h>
#include <unistd.h>
#include <magic.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#include "response.c"

int initialSocket;

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
 * Check that the program arguments were correct.
 *
 * @param argc The number of arguments.
 * @param argv The array of arguments.
 */
void checkRunParams(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        exit(1);
    }
}

/**
 * Initialise a socket for a client to connect to.
 *
 * @param initialSocket The initial socket number.
 * @param port The port to listen on.
 */
void init(const char *port) {
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
}

/**
 * Accept a client and move them onto a new socket.
 *
 * @param initialSocket The main socket that we run on.
 * @return Return the client socket.
 */
int acceptConnection() {
    struct sockaddr_in clientAddress;
    socklen_t clientLength = sizeof(clientAddress);

    // Accept the client to a new socket.
    int socket = accept(initialSocket, (struct sockaddr *) &clientAddress, &clientLength);

    if (socket < 0) {
        error("Failed to create new accepting socket");
    }

    return socket;
}

/**
 * Handle signals.
 *
 * @param signo The signal number.
 */
void sig_handler(int signo) {
    if (signo == SIGINT) {
        // Inform of interrupt being received.
        printf("\nReceived SIGINT\n");

        // Close the initial socket.
        printf("Closing socket...\n");
        if (close(initialSocket) != 0) {
            fprintf(stderr, "Failed to close socket.\n");
        }

        // Exit the program cleanly after threads.
        exit(0);
    }
}

void handleResponse(int socket, char *url) {
    if (!strcmp(url, "/")) {
        url = "/index.html";
    }

    char fileDest[256] = "public_html";
    strncat(fileDest, url, 256);

    int file = open(fileDest, O_RDONLY);
    if (file != -1) {
        char response[1024];
        strcat(response, ok_response);

        const char *mime;
        magic_t magic;
        magic = magic_open(MAGIC_MIME);
        magic_load(magic, NULL);
        mime = magic_file(magic, fileDest);

        char contentType[256];
        snprintf(contentType, sizeof(contentType), content_type_response, mime);

        // If magic determines the file is plaintext, leave the decision to the browser.
        if (!strstr(contentType, "text/plain")) {
            strcat(response, contentType);
        }
        strcat(response, "\n");
        magic_close(magic);

        send(socket, response, strlen(response), 0);

        struct stat *fileStats = malloc(sizeof(struct stat));
        stat(fileDest, fileStats);

        sendfile(socket, file, 0, (size_t) fileStats->st_size);

        close(file);
    } else {

    }
}

void handleRequest(int clientSocket) {
    size_t arraySize = 256;
    ssize_t length;
    char buffer[arraySize];

    if ((length = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        char method[arraySize];
        char url[arraySize];
        char protocol[arraySize];

        buffer[length] = '\0';

        sscanf(buffer, "%s %s %s", method, url, protocol);

        while (!strstr(buffer, "\r\n\r\n")) {
            length = recv(clientSocket, buffer, arraySize, 0);
        }
        if (length < 0) {
            close(clientSocket);
            return;
        }

        if (strcmp(protocol, "HTTP/1.0") && strcmp(protocol, "HTTP/1.1")) {
            send(clientSocket, bad_request_response, sizeof(bad_request_response), 0);
        } else if (strcmp(method, "GET")) {
            char formattedError[512];
            snprintf(formattedError, sizeof(formattedError), bad_method_response_template, method);
            send(clientSocket, formattedError, sizeof(formattedError), 0);
        } else {
            handleResponse(clientSocket, url);
        }
    }

}

void handleProcess(int clientSocket) {
    pid_t pid = fork();

    // Check that forking didn't return an error.
    if (pid < 0) {
        fprintf(stderr, "Failed to fork.");
        return;
    }

    if (pid == 0) { // Child process.
        // The child process doesn't need the listening socket.
        close(initialSocket);

        // Handle the client's request.
        handleRequest(clientSocket);

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
    while ((clientSocket = acceptConnection())) {
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

    // Initialise a socket listening on the given port.
    init(argv[1]);

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