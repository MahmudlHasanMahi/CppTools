#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
using namespace std;

class Request {
public:
    struct addrinfo* serverinfo;
private:
    const char* endpoint;
    const char* port = "80";
    string method;
    int socdscrip = -1;

private:
    int setaddrinfo() {
        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints)); // Clearing struct
        hints.ai_family = AF_UNSPEC;     // AF_INET: IPv4, AF_INET6: IPv6, AF_UNSPEC: not specfied
        hints.ai_socktype = SOCK_STREAM;

        int status = getaddrinfo(endpoint, port, &hints, &serverinfo);

        if (status) {
            printf("failed!");
        }
        return status;

    }
    int connect_socket() {
        int socket_file_discrp = -1;
        struct addrinfo* addresses;
        // Trying our all IPs until one connects

        for (addresses = this->serverinfo; addresses != nullptr; addresses = addresses->ai_next) {
            socket_file_discrp = socket(addresses->ai_family, addresses->ai_socktype, addresses->ai_protocol);
            if (socket_file_discrp == -1) continue;
            if (connect(socket_file_discrp, addresses->ai_addr, addresses->ai_addrlen) == -1) {
                close(socket_file_discrp);
                continue;
            }
            break;
        }
        if (addresses == nullptr) {
            // Clear memory
            freeaddrinfo(this->serverinfo);
            return 0;
        }
        return socket_file_discrp;
    }
    void get_request() {
        string request = "GET " + string("/todos/1") + " HTTP/1.1\r\n" +
            "Host: " + this->endpoint + "\r\n" +
            "Connection: close\r\n\r\n";

        send(this->socdscrip, request.c_str(), request.length(), 0);

        char buffer[4096];
        ssize_t bytesReceived;
        while ((bytesReceived = recv(this->socdscrip, buffer, sizeof(buffer) - 1, 0)) > 0) {

            buffer[bytesReceived] = '\0';
            cout << buffer << endl;
        }
        close(this->socdscrip);
        freeaddrinfo(this->serverinfo);
    }

public:
    Request(const char* endpoint, string method) {
        this->endpoint = endpoint;
        this->method = method;
        if (!setaddrinfo()) {
            this->socdscrip = connect_socket();
            get_request();
        };
    };
};
