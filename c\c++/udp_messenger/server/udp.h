#pragma once

#include "psInterface.h"

#include <string>
#include <map>
#include <vector>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "sha256.h"


class UdpServer: public PsInterface {
private:
	bool init();
	bool deinit();

	std::string ipAddr;
	int port;
	SOCKET serverSocket;
	sockaddr_in clientAddr;
	int clientAddrSize;
	
	const char* dbFilepath = "./../db.txt";							// path to DataBase with usernames and passwords
	std::map<std::string, std::string> logPass;						// loaded DataBase
	std::map<std::string, std::vector<sockaddr_in>> usernameAddr;	// table of "online" users; username: all addresses where user is logged in
	

public:
	UdpServer(const std::string& ipAddr, int port);

	bool open();
	bool close();

	bool loadDB(const char* filepath);
	bool saveDB(const char* filepath);

	bool registerUser(std::string username, std::string password);
	bool logIn(std::string username, std::string password);
	bool logOut(sockaddr_in address);
	bool logOutFromAllOthers(sockaddr_in address);
	bool addUser(std::string username, sockaddr_in addr);

	sockaddr_in getClientAddr();

	std::vector<sockaddr_in>* getUserAddr(std::string username);
	std::string getUsername(sockaddr_in userAddr);

	size_t write(const uint8_t* data, size_t size);
	size_t write(const uint8_t* data, size_t size, sockaddr_in userAddr);
	bool writeToUser(const uint8_t* data, size_t size, std::string username);
	
	size_t read(uint8_t* data, size_t size);

	void error(const std::string& message);
};

