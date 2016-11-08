/*static char *ver_HTTP10_response = "HTTP/1.0\n";
static char *ver_HTTP11_response = "HTTP/1.1\n";*/
static char *content_type_response = "Content-Type: %s\n";

/**
 * HTTP OK response and header.
 */
static char *ok_response =
        "HTTP/1.1 200 OK\n";

/**
 * HTTP bad request response, header and body.
 */
static char *bad_request_response =
        "HTTP/1.0 400 Bad Request\n"
                "Content-type: text/html\n"
                "\n"
                "<html>\n"
                " <body>\n"
                "  <h1>Bad Request</h1>\n"
                "  <p>This server did not understand your request.</p>\n"
                " </body>\n"
                "</html>\n";

/*static char *forbidden_response_template =
        "HTTP/1.0 403 Forbidden\n"
                "Content-type: text/html\n"
                "\n"
                "<html>\n"
                " <body>\n"
                "  <h1>Forbidden</h1>\n"
                "  <p>You don't have permission to access %s on this server.</p>\n"
                " </body>\n"
                "</html>\n";*/

/**
 * HTTP URL not found response, header and body with URL string variable.
 */
/*static char *not_found_response_template =
        "HTTP/1.0 404 Not Found\n"
                "Content-type: text/html\n"
                "\n"
                "<html>\n"
                " <body>\n"
                "  <h1>Not Found</h1>\n"
                "  <p>The requested URL %s was not found on this server.</p>\n"
                " </body>\n"
                "</html>\n";*/

/**
 * HTTP method not implemented response, header and body with method string variable.
 */
static char *bad_method_response_template =
        "HTTP/1.0 501 Method Not Implemented\n"
                "Content-type: text/html\n"
                "\n"
                "<html>\n"
                " <body>\n"
                "  <h1>Method Not Implemented</h1>\n"
                "  <p>The method %s is not implemented by this server.</p>\n"
                " </body>\n"
                "</html>\n";