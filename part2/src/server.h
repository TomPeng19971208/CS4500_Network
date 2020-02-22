#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>


#pragma once
class Server {
public:
    int socket_fd, sock;
    static const int max_client = 3;
    struct sockaddr_in server_addr, client_addr;
    int client_count = 0;
    int client_socket[max_client];
    char** client_ips;
    char send_buff[1024];
    char recv_buff[1024];
public:
    Server() {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        assert(socket_fd >= 0);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        client_ips = new char*[max_client];
    }

    Server(const char* ip_addr) {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        assert(socket_fd >= 0);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);
        server_addr.sin_addr.s_addr = inet_addr(ip_addr);
        assert(inet_pton(AF_INET, ip_addr, &server_addr.sin_addr)>0);
        client_ips = new char*[max_client];
    }

    ~Server() {
        delete [] client_ips;
    }

public:
    //bind to port 8080 and start listening
    void start() {
        assert(bind(socket_fd, (struct sockaddr *)&server_addr,sizeof(server_addr)) >=0 );
        assert(listen(socket_fd, 3) >= 0);
        int struct_len = sizeof(server_addr);
        while(client_count < max_client) {
            assert((sock = accept(socket_fd, (struct sockaddr*)&client_addr,(socklen_t*)&struct_len))>=0);
            read(sock , recv_buff, 1024);
            register_client(sock);
        }
        broadcast_client_ip();
        fd_set read_fds;
        int max_sd = 0;
        FD_ZERO(&read_fds);
        for(int i=0; i<client_count;i++) {
            FD_SET( client_socket[i] , &read_fds);
            max_sd = client_socket[i] > max_sd ? client_socket[i] : max_sd;
        }
        //this system contains a centralized server, all communication must go through the server
        while(client_count == max_client) {
            int socket_idx = get_active_socket_idx(max_sd+1, read_fds);
            handle_client_message(socket_idx);
        }
    }

    int get_active_socket_idx(int max_sd, fd_set read_fds) {
        int ready = select(max_sd+1, &read_fds, NULL, NULL, NULL);
        if (ready > 0) {
            for(int i=0; i<client_count; i++) {
                if (FD_ISSET(client_socket[i], &read_fds)) {
                    return i;
                }
            }
        }
    }

    void register_client(int socket) {
        if (client_count < max_client) {
            client_socket[client_count] = socket;
            client_ips[client_count] = new char[strlen(inet_ntoa(client_addr.sin_addr))];
            strcpy(client_ips[client_count], inet_ntoa(client_addr.sin_addr));
            client_count += 1;
        }
    }

    void broadcast_client_ip() {
        strcat(send_buff, "reg_ack,");
        char* ptr = send_buff + strlen(send_buff)* sizeof(char);
        for(int i=0; i<client_count; i++) {
            memcpy(ptr, client_ips[i], strlen(client_ips[i])* sizeof(char));
            ptr += strlen(client_ips[i]) * sizeof(char);
            *ptr++ = ',';
        }
        for(int i=0; i<client_count; i++) {
            assert(send(client_socket[i], send_buff, strlen(send_buff) * sizeof(char), 0) > 0);
        }
    }

    void handle_client_message(int client_idx) {
        memset(recv_buff, '\0', sizeof(recv_buff));
        int valread = read(client_socket[client_idx] , recv_buff, 1024);
        if (valread > 0) {
            //receive buffer might contain multiple messages if they were sent consequtively.
            char* token = strstr(recv_buff, "\\END");
            char* start = recv_buff;
            while(token != nullptr) {
                *token = '\0';
                on_message(client_ips[client_idx], start);
                start = token + strlen("\\END") * sizeof(char);
                token = strstr(start, "\\END");
            }
        }
        //One of the client disconnected, signal all clients
        if (valread == 0) {
            terminate();
        }
    }

    //the message format is "type,dst_ip,message"
    //there are two types of messages, send/term
    void on_message(char* sender_ip, const char* msg) {
        if (strlen(msg) < 1) {return;}
        //parse the message
        int type_end=0, dst_end=0, current_idx=0;
        while(type_end < 1 || dst_end < 1) {
            if (msg[current_idx] == ',') {
                if (type_end == 0) {type_end = current_idx;}
                else {dst_end = current_idx;}
            }
            current_idx+=1;
        }
        char msg_type[type_end+1];
        char dst_ip[dst_end - type_end];
        memcpy(msg_type, msg, type_end * sizeof(char));
        memcpy(dst_ip, msg + ((type_end + 1) * sizeof(char)), (dst_end - type_end - 1) * sizeof(char));
        msg_type[type_end] = '\0';
        dst_ip[dst_end - type_end - 1] = '\0';
        //send indicates a direct communication, term means the message is a termination signal
        if (strcmp(msg_type, "Send") == 0){
            send_msg(sender_ip, dst_ip, msg + (dst_end + 1) * sizeof(char));
        } else if (strcmp(msg_type, "Term") == 0) {
            terminate();
        }
    }

    int get_client_ip_index(char* client_ip) {
        for(int i=0; i<client_count; i++) {
            if (strcmp(client_ip, client_ips[i]) == 0) {
                return i;
            }
        }
        return -1;
    }

    // the client should receive a message formatted like "From,sender_ip,msg_content"
    void send_msg(char* sender_ip, char* receiver_ip, const char* msg) {
        int fp_idx = get_client_ip_index(receiver_ip);
        memset(send_buff, '\0', sizeof(send_buff));
        strcat(send_buff, "From,");
        strcat(send_buff, sender_ip);
        strcat(send_buff, ",");
        strcat(send_buff, msg);
        strcat(send_buff, "\\END");
        send(client_socket[fp_idx], send_buff, strlen(send_buff), 0);
    }

    //server will exit when it receives a terminate signal from one of its clients
    void terminate() {
        memset(send_buff, '\0', sizeof(send_buff));
        strcat(send_buff, "Term,");
        strcat(send_buff, inet_ntoa(server_addr.sin_addr));
        strcat(send_buff, ",");
        strcat(send_buff, "\\END");
        for(int i=0; i<client_count; i++) {
            send(client_socket[i], send_buff, strlen(send_buff)* sizeof(char), 0);
            close(client_socket[i]);
        }
        client_count = 0;
        close(socket_fd);
    }
};