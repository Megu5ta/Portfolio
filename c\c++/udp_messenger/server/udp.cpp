#include "udp.h"
#include <iostream>
#include <fstream>
#include <sstream>
#pragma comment(lib, "Ws2_32.lib")

bool UdpServer::init() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
		error("Error in WSA startup");
		return false;
	}
	// get ip address:
	// gethostname(...);
	// gethostbyname(...);

	this->serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (this->serverSocket == INVALID_SOCKET) {
		error("Error creating socket");
		WSACleanup();
		return false;
	}

	sockaddr_in socketAddr;
	memset(&socketAddr, 0, sizeof(socketAddr));
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_addr.s_addr = inet_addr(this->ipAddr.c_str());
	socketAddr.sin_port = htons(this->port);

	if (bind(this->serverSocket, (sockaddr*)&socketAddr, sizeof(socketAddr)) == SOCKET_ERROR) {
		error("Error binding socket");
		deinit();
		return false;
	}

	// every 1000 miliseconds stops recvfrom(). It is needed to exit from program without need to wait message.
	ULONGLONG tm = GetTickCount64();
	DWORD val = 1000;
	setsockopt(this->serverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&val, sizeof DWORD);
	
	if (!this->loadDB(this->dbFilepath)) {
		error("Error while loading database.");
		deinit();
		return false;
	}

	return true;
}

bool UdpServer::deinit() {
	bool ret = closesocket(this->serverSocket) != SOCKET_ERROR;
	WSACleanup();
	return ret;
}

UdpServer::UdpServer(const std::string& ipAddr, int port) {
	this->ipAddr = ipAddr;
	this->port = port;
	this->serverSocket = INVALID_SOCKET;
}

bool UdpServer::loadDB(const char* filepath) {
	// Load "DataBase" which contains logins and hashed passwords;
	std::ifstream db;
	db.open(filepath);

	if (db.is_open()) {
		std::string login;
		std::string password;
		while (db >> login >> password) {
			this->logPass[login] = password;
		}
		db.close();
		return true;
	}
	return false;
}

bool UdpServer::saveDB(const char* filepath) {
	// Save "DataBase" which contains logins and hashed passwords;
	std::ofstream db;
	db.open(filepath);
	if (db.is_open()) {
		for (std::map<std::string, std::string>::iterator it = this->logPass.begin(); it != this->logPass.end(); it++) {
			db << it->first << ' ' << it->second << std::endl;
		}
		return true;
	}
	return false;
}

bool UdpServer::logIn(std::string username, std::string password) {
	// Check if username and password are correct. If it is then addusername and address to "online"
	// and send response to user.
	if (this->logPass.count(username)) {
		if (this->logPass[username] == sha256(password)) {
			this->addUser(username, this->clientAddr);

			std::stringstream ss;
			ss << "You are logged in as \"" << username << "\".";
			size_t size = ss.str().size();
			if (this->write((const uint8_t*)(ss.str().c_str()), size) == size) {
				std::cout << "Sent response: " << ss.str() << std::endl;
			}
			return true;
		}	
	}

	const char* response = "Wrong username or password.";
	size_t size = strlen(response);
	if (this->write((const uint8_t*)response, size) == size) {
		std::cout << "Sent response: " << response << std::endl;
	}
	return false;
}

bool isEqual(sockaddr_in addr1, sockaddr_in addr2) {
	// Checks if two variables of type sockaddr_in are equal.
	if (addr1.sin_family != addr2.sin_family) {
		return false;
	}
	if (addr1.sin_port != addr2.sin_port) {
		return false;
	}
	if (strcmp(addr1.sin_zero, addr2.sin_zero) != 0) {
		return false;
	}
	if (addr1.sin_addr.S_un.S_addr != addr2.sin_addr.S_un.S_addr) {
		return false;
	}
	return true;
}

bool UdpServer::logOut(sockaddr_in address) {
		// Logout user on address. And kepp logged on other logged addresses.
	std::string username = this->getUsername(address);
	if (username == "") {
		return false;
	}
	std::vector<sockaddr_in> newAddresses;
	for (size_t i = 0; i < this->usernameAddr[username].size(); i++) {
		if (!isEqual(address, this->usernameAddr[username][i])) {
			newAddresses.push_back(this->usernameAddr[username][i]);
		}
	}
	this->usernameAddr[username] = newAddresses;
	const char* message = "You are logged out. Press any key to exit.";
	this->write((const uint8_t*)message, strlen(message), address);
	return true;
}

bool UdpServer::logOutFromAllOthers(sockaddr_in address) {
	// Logout user on all addresses that are not "sockaddr_in address". Keep user on "address" logged in.
	std::string username = this->getUsername(address);
	if (username == "") {
		return false;
	}
	const char* message = "You are logged out. Press any key to exit.";
	for (size_t i = 0; i < this->usernameAddr[username].size(); i++) {
		if (!isEqual(address, this->usernameAddr[username][i])) {
			this->write((const uint8_t*)message, strlen(message), this->usernameAddr[username][i]);
		}
	}

	this->usernameAddr[username].clear();
	this->addUser(username, address);
	return true;
}

bool UdpServer::registerUser(std::string username, std::string password) {
	// Register new user if username does not exist in DB yet
	if (username.empty() || password.empty()) {
		char response[] = "Empty username or password";
		size_t size = strlen(response);
		if (this->write((const uint8_t*)response, size) == size) {
			std::cout << "Sent response: " << response << std::endl;
		}
		return false;
	}
	if (this->logPass.count(username)) {
		char response[] = "Try another username";
		size_t size = strlen(response);
		if (this->write((const uint8_t*)response, size) == size) {
			std::cout << "Sent response: " << response << std::endl;
		}
		return false;
	}
	this->logPass[username] = sha256(password);
	this->saveDB(this->dbFilepath);
	std::stringstream ss;
	ss << "You are registered as \"" + username + "\".";
	size_t size = ss.str().size();
	if (this->write((const uint8_t*)(ss.str().c_str()), size) == size) {
		std::cout << "Sent response: " << ss.str() << std::endl;
	}
	return true;
}

bool UdpServer::open() {
	if (this->serverSocket != INVALID_SOCKET) {
		return true;
	}

	return init();
}

bool UdpServer::close() {
	if (this->serverSocket == INVALID_SOCKET) {
		return true;
	}

	return deinit();
}

sockaddr_in UdpServer::getClientAddr() {
	return this->clientAddr;
}

bool UdpServer::addUser(std::string username, sockaddr_in addr) {
	// add User to "online". Added users are able to send and receive messages.
	if (username.empty()) {
		return false;
	}
	this->usernameAddr[username].push_back(addr);
	
	return true;
}

std::vector<sockaddr_in>* UdpServer::getUserAddr(std::string username) {
	// get Addresse associated to username.
	if (this->usernameAddr.count(username)) {
		return &this->usernameAddr[username];
	}
	return NULL;
}

std::string UdpServer::getUsername(sockaddr_in userAddr) {
	// return username of userAddr if userAddr is "online". Otherwise returns empty string;
	for (std::map<std::string, std::vector<sockaddr_in>>::iterator it = this->usernameAddr.begin();
		it != this->usernameAddr.end(); it++) {
		for (size_t i = 0; i < it->second.size(); i++) {
			if (isEqual(userAddr, it->second[i])) {
				return it->first;
			}
		}
	}

	return "";
}

size_t UdpServer::write(const uint8_t* data, size_t size) {
	int bytesWritten = sendto(this->serverSocket, (const char*)data, size, 0, (sockaddr*)&this->clientAddr, this->clientAddrSize);
	if (bytesWritten == SOCKET_ERROR) {
		error("Cannot write data");
		return -1;
	}

	return bytesWritten;
}

size_t UdpServer::write(const uint8_t* data, size_t size, sockaddr_in userAddr) {
	// Send message to userAddr
	size_t userAddrSize = sizeof(userAddr);
	int bytesWritten = sendto(this->serverSocket, (const char*)data, size, 0, (sockaddr*)&userAddr, userAddrSize);
	if (bytesWritten == SOCKET_ERROR) {
		this->error("Cannot write data");
		return -1;
	}

	return bytesWritten;
}

bool UdpServer::writeToUser(const uint8_t* data, size_t size, std::string username) {
	// Send message to all addresses associated with this username
	if (this->usernameAddr.count(username) == 0) {
		error("Username does not exists");
		return false;
	}
	bool sent = false;
	for (size_t i = 0; i < this->usernameAddr[username].size(); i++) {
		if (this->write(data, size, this->usernameAddr[username][i]) == size) {
			sent = true;
		}
	}
	return sent;
}

size_t UdpServer::read(uint8_t* data, size_t size) {
	this->clientAddrSize = sizeof(this->clientAddr);
	memset(&this->clientAddr, 0, this->clientAddrSize);
	int bytesRead = recvfrom(this->serverSocket, (char*)data, size, 0, (sockaddr*)&this->clientAddr, &this->clientAddrSize);
	
	return bytesRead;
}

void UdpServer::error(const std::string& message) {
	std::cerr << message << ", code: " << GetLastError() << std::endl << std::flush;
}
