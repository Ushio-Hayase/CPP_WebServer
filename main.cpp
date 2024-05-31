#define PY_SSIZE_T_CLEAN 

#include <stdio.h>
#include <Python.h>

#include "http_server.hpp"

int main(void) {
    printf("Program Starting!\n");

    char* length_less_header = "Server:Custom\nContent-Type:text/html,Content-Length:";
    char** ptr = &length_less_header;

    Server server;
    server.run(ptr);

    
}



