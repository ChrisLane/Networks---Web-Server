char *addContentType(char *response, char *fileDest);

char *generateDate();

char *generateOkHeader(char *fileDest);

char *generateFailHeader(int statusCode);