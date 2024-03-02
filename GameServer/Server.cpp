#include "Server.h"

Server::Server(unsigned short port)
    :
    m_WASData{ 0 },
    m_serverAddr{ 0 },
    m_serverSocket{ 0 },
    m_port{ port }
{
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_addr.s_addr = INADDR_ANY;
    m_serverAddr.sin_port = htons(port);
}

Server::~Server()
{
    m_worker.join();

    if (m_created)
    {
        closesocket(m_serverSocket);
        WSACleanup();
    }
}

Server_Error_Type Server::Start()
{
    if (!m_created) return SERVER_NOT_CREATED;
    m_is_running = true;
    m_worker = std::thread{ &Server::Run, this };

    return SERVER_NO_ERROR;
}

void Server::Stop()
{
    m_is_running = false;
}

Server_Error_Type Server::Create()
{
    //Creating windows server socket data struct
    if (WSAStartup(MAKEWORD(2, 2), &m_WASData) != 0) {
        std::cerr << "WSA Startup failed.\n";
        return SERVER_WASDATA_ERROR;
    }

    // Create socket
    m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_serverSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return SERVER_SOCKET_INIT_ERROR;
    }

    //Binding socket
    if (bind(m_serverSocket, (SOCKADDR*)&m_serverAddr, sizeof(m_serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(m_serverSocket);
        WSACleanup();
        return SERVER_BINDING_FAILD;
    }

    if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        m_is_running = false;
        return SERVER_FAILED_TO_LISTEN;
    }

    std::cout << "Server is listening on port " << m_port << std::endl;

    m_created = true;
    return SERVER_NO_ERROR;
}

bool Server::IsRunning()
{
    return m_is_running;
}

Server_Error_Type Server::Accept(SOCKET& o_socket, sockaddr_in& o_addr)
{
    if (!m_created) 
    {
        m_is_running = false;
        return SERVER_NOT_CREATED;
    }

    // Accept incoming connections
    int clientAddrSize = sizeof(o_addr);
    o_socket = accept(m_serverSocket, (SOCKADDR*)&o_addr, &clientAddrSize);
    if (o_socket == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        return SERVER_FAILED_ACCEPTING_CLIENT;
    }

	return SERVER_NO_ERROR;
}

Server_Error_Type Server::Run()
{
    if (!m_created) return SERVER_NOT_CREATED;

    while (m_is_running)
    {
        std::cout << "Waiting for connection...\n";
        SOCKET client = INVALID_SOCKET;
        sockaddr_in addr;
        if (Accept(client, addr) == SERVER_NO_ERROR) 
        {
            std::cout << "Accepted " << client << "\n";
        }
        else 
        {
            std::cerr << "FAILD TO ACCEPT CLIENT\n";
        }
        Stop();
    }
    return SERVER_NO_ERROR;
}
