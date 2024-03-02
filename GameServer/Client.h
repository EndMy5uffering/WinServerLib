#pragma once

class Client
{

public:
	Client(Client&) = delete;

	Client();

	~Client();

	
	void* read();

private:

};

