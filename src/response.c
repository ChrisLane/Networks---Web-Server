char *date_response = "Date : %s\n";
char *content_type_response = "Content-Type: %s\n";
char *status_response = "Status: %s\n";

/**
 * HTTP OK response header element.
 */
char *ok_response =
        "HTTP/1.1 200 OK\n";

/**
 * HTTP 400 response header element.
 */
char *bad_request_response =
        "HTTP/1.1 400 Bad Request\n";

/**
 * HTTP bad request response body.
 */
char *bad_request_response_html =
        "<html>\n"
                " <body>\n"
                "  <h1>Bad Request</h1>\n"
                "  <p>This server did not understand your request.</p>\n"
                " </body>\n"
                "</html>\n";

/**
 * HTTP 403 response header element.
 */
char *forbidden_response =
        "HTTP/1.0 403 Forbidden\n";

/**
 * HTTP forbidden request body.
 */
char *forbidden_response_html_template =
        "<html>\n"
                " <body>\n"
                "  <h1>Forbidden</h1>\n"
                "  <p>You don't have permission to access %s on this server.</p>\n"
                " </body>\n"
                "</html>\n";

/**
 * HTTP 404 response header element.
 */
char *not_found_response =
        "HTTP/1.1 404 Not Found\n";

/**
 * HTTP URL not found response body with URL string variable.
 */
char *not_found_response_html_template =
        "<html>\n"
                " <body>\n"
                "  <h1>Not Found</h1>\n"
                "  <p>The requested URL %s was not found on this server.</p>\n"
                " </body>\n"
                "</html>\n";

/**
 * HTTP 501 response header element.
 */
char *bad_method_response =
        "HTTP/1.1 501 Method Not Implemented\n";

/**
 * HTTP method not implemented response, header and body with method string variable.
 */
char *bad_method_response_html_template =
        "<html>\n"
                " <body>\n"
                "  <h1>Method Not Implemented</h1>\n"
                "  <p>The method %s is not implemented by this server.</p>\n"
                " </body>\n"
                "</html>\n";