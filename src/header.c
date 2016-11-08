#include <string.h>
#include <stdio.h>
#include <time.h>
#include "header.h"
#include "response.h"
#include <magic.h>
#include <stdlib.h>

char *addContentType(char *response, char *fileDest) {
    const char *mime;
    struct magic_set *magic;
    magic = magic_open(MAGIC_MIME);
    magic_load(magic, NULL);
    mime = magic_file(magic, fileDest);

    char result[256];
    snprintf(result, sizeof(result), content_type_response, mime);

    char *contentType = malloc(sizeof(char) * strlen(result));
    strcpy(contentType, result);
    magic_close(magic);

    if (strstr(contentType, "text/plain")) {
        return response;
    }

    return strcat(response, contentType);
}

char *generateDate() {
    char result[256];
    char timebuf[50];
    time_t timer;
    struct tm *tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(timebuf, sizeof(timebuf), "%a, %d %b %Y %H:%M:%S %Z", tm_info);
    snprintf(result, sizeof(result), date_response, timebuf);

    char *date = malloc(sizeof(char) * strlen(result));
    strcpy(date, result);

    return date;
}

char *generateOkHeader(char *fileDest) {
    char *response = malloc(sizeof(char) * 1024);
    strcpy(response, "");

    // OK
    strcat(response, ok_response);
    // Date
    char *date = generateDate();
    strcat(response, date);
    // Content-Type
    response = addContentType(response, fileDest);
    // Status
    char status[256];
    snprintf(status, sizeof(status), status_response, "200 OK");
    strcat(response, status);

    strcat(response, "\n");
    return response;
}

char *generateFailHeader(int statusCode) {
    char *response = malloc(sizeof(char) * 1024);
    strcpy(response, "");
    char *date;
    char status[256];

    switch (statusCode) {
        case 400:
            // 400 Error Code
            strcat(response, bad_request_response);
            // Date
            date = generateDate();
            strcat(response, date);
            // Status
            snprintf(status, sizeof(status), status_response, "400 Bad Request");
            strcat(response, status);

            strcat(response, "\n");
            return response;
        case 403:
            // 403 Error Code
            strcat(response, forbidden_response);
            // Date
            date = generateDate();
            strcat(response, date);
            // Status
            snprintf(status, sizeof(status), status_response, "403 Forbidden");
            strcat(response, status);

            strcat(response, "\n");
            return response;
        case 404:
            // 404 Error Code
            strcat(response, not_found_response);
            // Date
            date = generateDate();
            strcat(response, date);
            // Status
            snprintf(status, sizeof(status), status_response, "404 Not Found");
            strcat(response, status);

            strcat(response, "\n");

            return response;
        case 501:
            // 501 Error Code
            strcat(response, bad_method_response);
            // Date
            date = generateDate();
            strcat(response, date);
            // Status
            snprintf(status, sizeof(status), status_response, "501 Method Not Implemented");
            strcat(response, status);

            strcat(response, "\n");
            return response;
        default:
            // Date
            date = generateDate();
            strcat(response, date);

            strcat(response, "\n");
            return response;
    }
}