#include "udp.h"
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

bool UdpClient::init() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
		error("Error in WSA startup");
		return false;
	}

	this->clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (this->clientSocket == INVALID_SOCKET) {
		error("Error creating socket");
		WSACleanup();
		return false;
	}

	// every 1000 miliseconds stops recvfrom(). It is needed to exit from program without need to wait message.
	int tm = GetTickCount64();
	DWORD val = 1000;
	setsockopt(this->clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&val, sizeof DWORD);


	this->serverAddrSize = sizeof(this->serverAddr);
	memset(&this->serverAddr, 0, this->serverAddrSize);
	this->serverAddr.sin_family = AF_INET;
	this->serverAddr.sin_addr.s_addr = inet_addr(this->ipAddr.c_str());
	this->serverAddr.sin_port = htons(this->port);

	return true;
}

bool UdpClient::deinit() {
	bool ret = closesocket(this->clientSocket) != SOCKET_ERROR;
	WSACleanup();
	return ret;
}

UdpClient::UdpClient(const std::string& ipAddr, int port) {
	this->ipAddr = ipAddr;
	this->port = port;
	this->clientSocket = INVALID_SOCKET;
}

bool UdpClient::open() {
	if (this->clientSocket!= INVALID_SOCKET) {
		return true;
	}

	return init();
}

bool UdpClient::close() {
	if (this->clientSocket == INVALID_SOCKET) {
		return true;
	}

	return deinit();
}

bool UdpClient::registration(std::string username, std::string password) {
	// send to server data for registration
	if (username.empty()) {
		return false;
	}
	std::string message = "/reg/" + username + ' ' + password;
	size_t bytesWritten = this->write((uint8_t*)message.c_str(), message.size());
	if (bytesWritten < 1) {
		return false;
	}

	// wait for confirmation from server
	// Due to the fact that now there is a timeout limit maybe it's worth to disable it here or increase somehow
	char readBuffer[256];
	memset(readBuffer, 0, 256);
	std::string success = "You are registered as \"" + username + "\".";
	if (this->read((uint8_t*)readBuffer, 256) > 0) {
		std::cout << readBuffer << std::endl;
		if (strcmp(readBuffer, success.c_str()) == 0) {
			return true;
		}
	}
	return false;
}

bool UdpClient::logIn(std::string username) {
	// Old simple version
	if (username.empty()) {
		return false;
	}
	std::string message = "/login/" + username;
	size_t bytesWritten = this->write((uint8_t*)message.c_str(), message.size());
	if (bytesWritten < 1) {
		error("Error while logging in");
		return false;
	}

	// wait for confirmation from server
	char readBuffer[256];
	memset(readBuffer, 0, 256);
	std::string success = "You are logged in as \"" + username + "\".";
	if (this->read((uint8_t*)readBuffer, 256) > 0) {
		std::cout << readBuffer << std::endl;
		if (strcmp(readBuffer, success.c_str()) == 0) {
			return true;
		}
	}
	return false;
}

bool UdpClient::logIn(std::string username, std::string password) {
	// send to server data for login
	if (username.empty() || password.empty()) {
		return false;
	}
	std::string message = "/login/" + username + ' ' + password;
	size_t bytesWritten = this->write((uint8_t*)message.c_str(), message.size());
	if (bytesWritten < 1) {
		error("Error while logging in");
		return false;
	}

	// wait for confirmation from server
	// Due to the fact that now there is a timeout limit maybe it's worth to disable it here or increase somehow
	char readBuffer[256];
	memset(readBuffer, 0, 256);
	std::string success = "You are logged in as \"" + username + "\".";
	if (this->read((uint8_t*)readBuffer, 256) > 0) {
		std::cout << readBuffer << std::endl;
		if (strcmp(readBuffer, success.c_str()) == 0) {
			return true;
		}
	}
	return false;
}

size_t UdpClient::write(const uint8_t* data, size_t size) {
	int bytesWritten = sendto(this->clientSocket, (const char*)data, size, 0, (sockaddr*)&this->serverAddr, this->serverAddrSize);
	if (bytesWritten == SOCKET_ERROR) {
		error("Cannot write data");
		return -1;
	}

	return bytesWritten;
}

size_t UdpClient::read(uint8_t* data, size_t size) {
	int bytesRead = recvfrom(this->clientSocket, (char*)data, size, 0, (sockaddr*)&this->serverAddr, &this->serverAddrSize);

	return bytesRead;
}

void UdpClient::error(const std::string& message) {
	std::cerr << message << ", code: " << GetLastError() << std::endl << std::flush;
}
