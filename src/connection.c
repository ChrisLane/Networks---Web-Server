#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <magic.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#include "connection.h"
#include "response.h"

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
        struct magic_set *magic;
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
