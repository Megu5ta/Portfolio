#include <iostream>
#include <string>
#include "udp.h"

static bool running = true;
const size_t bufferSize = 1024;

static DWORD _stdcall parallelRead(void* params) {
	UdpClient& udpClient = *(UdpClient*)params;
	char readBuffer[bufferSize];

	while (running) {
		memset(readBuffer, 0, bufferSize);
		size_t bytesRead = udpClient.read((uint8_t*)readBuffer, bufferSize);
		if (bytesRead != SOCKET_ERROR) {
			std::cout << "-> " << readBuffer << std::endl;
		}
		if (strcmp(readBuffer, "You are logged out. Press any key to exit.") == 0) {
			running = false;
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	std::string ipAddr = "127.0.0.1";
	if (argc > 1) {
		ipAddr = argv[1];
	}
	//std::cout << "Enter ip address: ";
	//std::cin >> ipAddr;

	int port = 27000;
	//std::cout << "Enter port: ";
	//std::cin >> port;


	UdpClient udpClient(ipAddr, port);
	if (!udpClient.open()) {
		return -1;
	}

	std::string cmd;
	bool logged = false;
	while (!logged) {
		std::cout << "Login (/login) or Registration (/reg)? /exit to exit the program." << std::endl;
		std::cin >> cmd;
		bool result = false;
		if (cmd == "/login") {
			std::string username;
			std::string password;
			while (!result) {
				std::cout << "Enter your username: ";
				std::cin >> username;
				if (username == "/exit") {
					break;
				}

				std::cout << "Enter your password: ";
				std::cin >> password;
				if (password == "/exit") {
					break;
				}
				result = udpClient.logIn(username, password);
				logged = result;
			}
		}
		else if (cmd == "/reg") {
			std::string username;
			std::string password;
			bool result = false;
			while (!result) {
				std::cout << "Enter username: ";
				std::cin >> username;
				if (username == "/exit") {
					break;
				}

				std::cout << "Enter password: ";
				std::cin >> password;
				if (password == "/exit") {
					break;
				}
				result = udpClient.registration(username, password);
			}
		}
		else if (cmd == "/exit") {
			udpClient.close();
			return 0;
		}
	}

	HANDLE readThread = CreateThread(NULL, 0, parallelRead, &udpClient, 0, NULL);
	if (readThread == NULL) {
		udpClient.error("Cannot create read thread");
		udpClient.close();
		return -1;
	}


	std::string input;
	while (running) {
		std::getline(std::cin, input);

		if (input.length() == 0) continue;

		udpClient.write((const uint8_t*)input.c_str(), input.length() + 1);

		if (input == "/exit") {
			running = false;
			break;
		}	
	}

	running = false;
	WaitForSingleObject(readThread, INFINITE);
	CloseHandle(readThread);

	if (!udpClient.close())
	{
		return -1;
	}

	return 0;
}