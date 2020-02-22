#include <iostream>
#include <assert.h>
#include "client.h"
int main(int argx, char** argv) {
    assert(argx ==3);
    Client* s = new Client(argv[2], "127.0.0.1");
    s->start();
    s->send_message("hello", "127.0.0.2");
    s->send_message("hello", "127.0.0.2");
    sleep(1);
    s->send_termination_signal();
    s->wait_for_termination();
    return 0;
}
