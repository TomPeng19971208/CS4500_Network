#include <iostream>
#include <assert.h>
#include "server.h"
int main(int argx, char** argv) {
    assert(argx == 3);
    Server* s = new Server(argv[2]);
    s->start(); //this will block until the server is terminated
    printf("server_terminated\n");
    return 0;
}
