#include <string.h>
#include <stdio.h>
#include <time.h>
#include <magic.h>
#include <stdlib.h>
#include "header.h"
#include "response.h"

/**
 * Generate the content type header element for the given file.
 *
 * @param response The header that we're concatenating with.
 * @param fileDest The file to check the MIME type of.
 * @return The initial response + the Content-Type header element.
 */
int addContentType(char *response, char *fileDest) {
    const char *mime;
    struct magic_set *magic;
    magic = magic_open(MAGIC_MIME);
    magic_load(magic, NULL);
    mime = magic_file(magic, fileDest);

    char result[100];
    snprintf(result, sizeof(result), content_type_response, mime);

    char *contentType = malloc(strlen(result) + 1);
    if (!contentType) {
        fprintf(stderr, "Failed to allocate memory.");
        return -1;
    }
    strcpy(contentType, result);
    magic_close(magic);

    if (strstr(contentType, "text/plain")) {
        return 0;
    }

    strcat(response, contentType);
    free(contentType);

    return 0;
}

/**
 * Generate the date header element.
 *
 * @return The date header element.
 */
char *generateDate() {
    char result[100];
    char timebuf[50];
    time_t timer;
    struct tm *tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(timebuf, sizeof(timebuf), "%a, %d %b %Y %H:%M:%S %Z", tm_info);
    snprintf(result, sizeof(result), date_response, timebuf);

    char *date = malloc(strlen(result) + 1);
    if (!date) {
        fprintf(stderr, "Failed to allocate memory.");
        return "";
    }
    strcpy(date, result);

    return date;
}

/**
 * Generate a header for an OK request.
 *
 * @param fileDest The file to generate header elements with relation to.
 * @return The header.
 */
char *generateOkHeader(char *fileDest) {
    char *response = malloc(sizeof(char) * 512);
    if (!response) {
        fprintf(stderr, "Failed to allocate memory.");
        return "";
    }
    strcpy(response, "");

    // OK
    strcat(response, ok_response);
    // Date
    char *date = generateDate();
    strcat(response, date);
    free(date);

    // Content-Type
    if (addContentType(response, fileDest) != 0) {
        fprintf(stderr, "Failed to add content type for: %s", fileDest);
    }
    // Status
    char status[100];
    snprintf(status, sizeof(status), status_response, "200 OK");
    strcat(response, status);

    strcat(response, "\n");
    return response;
}

/**
 * Generate a header for a given http status code.
 *
 * @param statusCode The status code to generate the header for.
 * @return The header.
 */
char *generateFailHeader(int statusCode) {
    char *response = malloc(sizeof(char) * 512);
    if (!response) {
        fprintf(stderr, "Failed to allocate memory.");
        return "";
    }
    strcpy(response, "");
    char *date;
    char status[100];

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

            free(date);

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

            free(date);

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

            free(date);

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

            free(date);

            return response;
        default:
            // Date
            date = generateDate();
            strcat(response, date);

            strcat(response, "\n");

            free(date);

            return response;
    }
}