#include "http_server.hpp"

void client_accept(Server* server);
void handle_client(Server* server, ClientInfo* data);

Server::Server() : clientThreads(), ClientPool(), server_socket(socket(IP_ADDR_TYPE, SOCK_STREAM, 0)), serverAddr() {
    const int optval = 1;

    if (this->server_socket == -1) { // 소켓 생성 실패 시 
        std::cerr << "fail to create server socket\n";

        throw std::runtime_error("Exception by Server class Initializer"); // 예외 발생 
    }

    setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); // socket의 SO_REUSEADDR 속성을 TRUE로 

    memset(&this->serverAddr, 0, sizeof(this->serverAddr));

    this->serverAddr.sin_family = IP_ADDR_TYPE; // IP주소 타입 설정 
    this->serverAddr.sin_addr.s_addr = INADDR_ANY; // 아무 IP주소로 설정 
    this->serverAddr.sin_port = htons(PORT); // 포트 설정 
}

int Server::get_serverSocket() const { return this->server_socket; }
const std::vector<ClientInfo*>& Server::get_clientPool() const { return this->ClientPool; }
std::vector<std::thread>& Server::get_client_threads() { return this->clientThreads; }

void client_accept(Server* server) {
    while(true) {
        int client_socket;
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        int server_socket = server->get_serverSocket();
        auto client_pool = server->get_clientPool();
        std::vector<std::thread>& client_threads = server->get_client_threads();

        client_socket = accept(server_socket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if(client_socket == -1) { // 연결 실패 시 이번 회차 건너뛰고 다시 기다림 
            std::cerr << "failed to accept connection of client\n";
            close(client_socket);
            continue;
        }

        std::cout << "success to connect client" << std::endl;
        std::cout << "Client IP: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
        std::cout << "Port: " << ntohs(clientAddr.sin_port) << std::endl;

        /*클라이언트의 데이터를 담은 구조체 생성 및
        vector에 담기*/

        ClientInfo* clientData = new ClientInfo;
        clientData->socket = client_socket;
        clientData->clientAddress = clientAddr;

        client_pool.push_back(clientData);

        client_threads.push_back(std::thread(&handle_client, server, clientData));
    }
}

void handle_client(Server* server, ClientInfo* data) {
    int clientSocket = data->socket;
    sockaddr_in clientAddr = data->clientAddress;

    auto ClientPool = server->get_clientPool();

    char buffer[PACKET_SIZE];
    int recvSize;
    std::string content = "";

    printf("test\n");

    do {
        memset(buffer, 0, PACKET_SIZE);
        recvSize = read(clientSocket, buffer, sizeof(buffer));
        
        content.append(buffer);
        std::cout << recvSize << "\n";
            
    } while (recvSize > 0);

    
    std::cout << content << std::endl;  

    for (size_t i = 0; i < ClientPool.size(); ++i) {
        if (ClientPool[i]->socket == clientSocket) {
            std::cout << "close to connection of client" << inet_ntoa(ClientPool[i]->clientAddress.sin_addr) 
            << ":" << ntohs(ClientPool[i]->clientAddress.sin_port) << std::endl;
            ClientPool.erase(ClientPool.begin() + i);
            break;
        }
    }

    delete data;
    close(clientSocket);
}

int Server::run(char** data) {
    int bindResult = bind(this->server_socket,(const sockaddr*) &this->serverAddr, sizeof(this->serverAddr)); // 바인딩 

    if(bindResult < 0) { // 바인딩 실패 시 
        std::cerr << "fail to bind\n";
        close(this->server_socket);

        return -1; // 예외 발생 
    }

    listen(this->server_socket, CAN_LISTEN); // 연결 요청 대기 중... 

    std::thread thread_listen(&client_accept, this);

    while(true) {
        if (this->ClientPool.empty()) continue; // 대기하는 클라이언트가 없으면 건너뛰기 
        


        for (ClientInfo* c: this->ClientPool) {
            

            

            send(c->socket, *data, strlen(*data), 0);
        }
    }

    thread_listen.join();

    /*메모리 할당 해제*/
    for (ClientInfo* c: this->ClientPool) delete c;
    for (int i = 0; i < this->clientThreads.size(); ++i) this->clientThreads[i].detach();


    close(server_socket);
    return 0;
}
