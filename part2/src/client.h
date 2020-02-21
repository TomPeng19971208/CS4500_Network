#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <thread>


#pragma once

class Client {
public:
    int socket_fd;
    struct sockaddr_in server_addr, self_addr;
    bool connected;
    int client_count = 0;
    char** client_ips;
    char send_buff[1024];
    char recv_buff[1024];
    std::thread* t;
public:
    Client() {
        connected = false;
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        assert(socket_fd >= 0);
        self_addr.sin_family = AF_INET;
        self_addr.sin_port = htons(9000);
        assert(bind(socket_fd, (struct sockaddr *)&self_addr,sizeof(self_addr)) >=0 );
        client_ips = new char*[1];
    }

    Client(const char* ip_addr) {
        connected = false;
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        assert(socket_fd >= 0);
        self_addr.sin_family = AF_INET;
        self_addr.sin_port = htons(9000);
        assert(inet_pton(AF_INET, ip_addr, &self_addr.sin_addr)>0);
        client_ips = new char*[1];
    }

    Client(const char* self_ip_addr, const char* server_ip_addr): Client(self_ip_addr) {
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);
        server_addr.sin_addr.s_addr = inet_addr(server_ip_addr);
    }

    ~Client() {
        delete [] client_ips;
    }

public:

    //bind to port 9000 and register
    void start() {
        assert(bind(socket_fd, (struct sockaddr *)&self_addr,sizeof(self_addr)) >=0 );
        register_self();
        t = new std::thread(handle_message, this);
    }

    //register the client to the server, receive list of client ip
    void register_self() {
        connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        assert(send(socket_fd, "register" , strlen("register"), 0 )>0);
        bzero(recv_buff, sizeof(recv_buff));
        while(read( socket_fd , recv_buff, 1024) == 0) {
            sleep(1);
        };
        initialize_client_ips(recv_buff);
        connected = true;
    }

    void initialize_client_ips(char* resp) {
        char *token;
        token = strtok(resp, ",");
        //get rid of the msaage's type
        token = strtok(NULL, ",");
        while( token != NULL ) {
            client_ips[client_count] = token;
            client_count += 1;
            token = strtok(NULL, ",");
        }
    }

    void on_message() {
        while(connected) {
            bzero(recv_buff, sizeof(recv_buff));
            int valread = read(socket_fd, recv_buff, 1024);
            if (valread > 0) {
                //receive buffer might contain multiple messages if they were sent consequtively.
                char* token = strstr(recv_buff, "\\END");
                char* start = recv_buff;
                while(token != nullptr) {
                    *token = '\0';
                    handle_single_message(start);
                    start = token + strlen("\\END") * sizeof(char);
                    token = strstr(start, "\\END");
                }
            }
        }
    }

    void handle_single_message(char* msg) {
        int type_end=0, sender_end=0, current_idx=0;
        while(type_end < 1 || sender_end < 1) {
            if (msg[current_idx] == ',') {
                if (type_end == 0) {type_end = current_idx;}
                else {sender_end = current_idx;}
            }
            current_idx+=1;
        }
        char msg_type[type_end+1];
        char sender_ip[sender_end - type_end];
        memcpy(msg_type, msg, type_end * sizeof(char));
        memcpy(sender_ip, msg + ((type_end + 1) * sizeof(char)), (sender_end - type_end - 1) * sizeof(char));
        msg_type[type_end] = '\0';
        sender_ip[sender_end - type_end - 1] = '\0';
        char* content = msg + (sender_end + 1) * sizeof(char);
        //From indicates a direct communication, Term means the message is a termination signal
        if (strcmp(msg_type, "From") == 0){
            printf("client: %s, received from: %s, message: %s\n", inet_ntoa(self_addr.sin_addr), sender_ip, content);
        } else if (strcmp(msg_type, "Term") == 0) {
            printf("%s terminated\n", inet_ntoa(self_addr.sin_addr));
            terminate();
        }
    }

    void terminate() {
        connected = false;
        close(socket_fd);
    }

    void send_message(char* msg, char* receiver_ip) {
        bzero(send_buff, sizeof(send_buff));
        strcat(send_buff, "Send,");
        strcat(send_buff, receiver_ip);
        strcat(send_buff, ",");
        strcat(send_buff, msg);
        strcat(send_buff, "\\END");
        send(socket_fd, send_buff, strlen(send_buff)* sizeof(char), 0);
    }

    void send_termination_signal() {
        bzero(send_buff, sizeof(send_buff));
        strcat(send_buff, "Term,");
        strcat(send_buff, inet_ntoa(self_addr.sin_addr));
        strcat(send_buff, ",");
        strcat(send_buff, "\\END");
        assert(send(socket_fd, send_buff, strlen(send_buff)* sizeof(char), 0)>0);
    }

    void wait_for_termination() {
        t->join();
    }

    int get_client_ip_index(char* client_ip) {
        for(int i=0; i<client_count; i++) {
            if (strcmp(client_ip, client_ips[i]) == 0) {
                return i;
            }
        }
        return -1;
    }

    static void handle_message(Client* c) {
        c->on_message();
    }
};
