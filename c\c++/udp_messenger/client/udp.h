#pragma once

#include "psInterface.h"

#include <string>
#include <WinSock2.h>

class UdpClient: public PsInterface {
private:
	bool init();
	bool deinit();

	std::string ipAddr;
	int port;
	SOCKET clientSocket;
	sockaddr_in serverAddr;
	int serverAddrSize;

public:
	UdpClient(const std::string& ipAddr, int port);

	bool open();
	bool close();

	bool logIn(std::string username);
	bool logIn(std::string username, std::string password);

	bool registration(std::string username, std::string password);

	size_t write(const uint8_t* data, size_t size);
	size_t read(uint8_t* data, size_t size);

	void error(const std::string& message);
};

