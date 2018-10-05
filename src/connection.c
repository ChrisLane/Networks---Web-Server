#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#include "connection.h"
#include "header.h"
#include "response.h"

/**
 * Send appropriate data back to the client.
 *
 * @param socket The socket that the client is on.
 * @param url The URL that the client requested.
 */
void handleResponse(int socket, char *url, char *home) {
    if (!strcmp(url, "/")) {
        url = "/index.html";
    }

    int fileDestSize = 256;
    char fileDest[fileDestSize];
    strcpy(fileDest, home);
    strncat(fileDest, url, fileDestSize - strlen(home) - 1);

    // Check if the file exists.
    if (access(fileDest, F_OK)) {
        char *response = generateFailHeader(404);
        send(socket, response, strlen(response), 0);
        char formattedError[512];
        snprintf(formattedError, sizeof(formattedError), not_found_response_html_template, url);
        send(socket, formattedError, strlen(formattedError), 0);

        return;
    }

    // Check if we have read access to the file.
    if (access(fileDest, R_OK) < 0) {
        char *response = generateFailHeader(403);
        send(socket, response, strlen(response), 0);
        char formattedError[512];
        snprintf(formattedError, sizeof(formattedError), forbidden_response_html_template, url);
        send(socket, formattedError, strlen(formattedError), 0);

        return;
    }

    int file = open(fileDest, O_RDONLY);
    if (file != -1) {

        char *response = generateOkHeader(fileDest);

        send(socket, response, strlen(response), 0);
        free(response);

        struct stat *fileStats = malloc(sizeof(struct stat));
        if (!fileStats) {
            fprintf(stderr, "Failed to allocate memory.");
            return;
        }
        stat(fileDest, fileStats);

        sendfile(socket, file, 0, (size_t) fileStats->st_size);

        close(file);
    } else {
        fprintf(stderr, "Failed to open %s.\n", fileDest);
    }
}

/**
 * Read the client's request into appropriate data stores.
 *
 * @param socket The socket that the client is on.
 */
void handleRequest(int socket, char *home) {
    size_t arraySize = 256;
    ssize_t length;
    char buffer[arraySize];

    if ((length = recv(socket, buffer, sizeof(buffer), 0)) > 0) {
        char method[arraySize];
        char url[arraySize];
        char protocol[arraySize];

        buffer[length] = '\0';

        sscanf(buffer, "%s %s %s", method, url, protocol);

        while (!strstr(buffer, "\r\n\r\n")) {
            length = recv(socket, buffer, arraySize, 0);
        }
        if (length < 0) {
            close(socket);
            return;
        }

        // Check for error conditions
        if (strcmp(protocol, "HTTP/1.0") != 0 && strcmp(protocol, "HTTP/1.1") != 0) { // Bad request
            char *response = generateFailHeader(400);
            send(socket, response, strlen(response), 0);
            send(socket, bad_request_response_html, sizeof(bad_request_response_html), 0);
        } else if (strcmp(method, "GET") != 0) { // We only support GET
            char *response = generateFailHeader(501);
            send(socket, response, strlen(response), 0);
            char formattedError[512];
            snprintf(formattedError, sizeof(formattedError), bad_method_response_html_template, method);
            send(socket, formattedError, strlen(formattedError), 0);
        } else {
            handleResponse(socket, url, home);
        }
    }
}
