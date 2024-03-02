#pragma once
#include <thread>
#include <WinSock2.h>
#include <iostream>


enum Server_Error_Type
{
	SERVER_NO_ERROR,
	SERVER_WASDATA_ERROR,
	SERVER_SOCKET_INIT_ERROR,
	SERVER_BINDING_FAILD,
	SERVER_NOT_CREATED,
	SERVER_FAILED_TO_LISTEN,
	SERVER_FAILED_ACCEPTING_CLIENT,
};

class Server
{

private:
	Server_Error_Type Accept(SOCKET& socket, sockaddr_in& addr);
	Server_Error_Type Run();


	WSADATA m_WASData;
	sockaddr_in m_serverAddr;
	SOCKET m_serverSocket = INVALID_SOCKET;

	std::thread m_worker;

	bool m_is_running = false;
	bool m_created = false;

	unsigned short m_port;

public:
	Server(Server&) = delete;

	Server(unsigned short port);
	~Server();

	Server_Error_Type Start();
	void Stop();
	Server_Error_Type Create();


	bool IsRunning();

};

