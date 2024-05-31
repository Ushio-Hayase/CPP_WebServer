#include <thread>
#include <vector>
#include <iostream>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


const static unsigned int PACKET_SIZE = 1024;
const static unsigned int PORT = 8085;
const static int IP_ADDR_TYPE = AF_INET;
const static unsigned int CAN_LISTEN = 10;

/* 클라이언트의 정보 구조체 */
struct ClientInfo {
    int socket;
    sockaddr_in clientAddress;
};

/* 서버 클래스 */
class Server {
    std::vector<std::thread> clientThreads;
    std::vector<ClientInfo*> ClientPool;

    int server_socket;
    sockaddr_in serverAddr;

public:
    Server();
    int run(char** data);

    int get_serverSocket() const;
    const std::vector<ClientInfo*>& get_clientPool() const;
    std::vector<std::thread>& get_client_threads();
};