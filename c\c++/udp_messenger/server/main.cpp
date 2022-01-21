#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "udp.h"
#include "sha256.h"

bool running = true;
const size_t bufferSize = 1024;

static DWORD _stdcall parallelInput(void* params) {
	std::string inp;
	while (running) {
		std::getline(std::cin, inp);
		if (inp == "/shutdown") {
			running = false;
		}
	}
	return 0;
}

int main(int argc, char* argv[]) {
	std::string ipAddr = "127.0.0.1";
	if (argc > 1) {
		ipAddr = argv[1];
	}
	std::cout << ipAddr << std::endl;
	//std::cout << "Enter ip address: ";
	//std::cin >> ipAddr;

	int port = 27000;
	//std::cout << "Enter port: ";
	//std::cin >> port;
	UdpServer udpServer(ipAddr, port);
	if (!udpServer.open()) {
		return -1;
	}

	HANDLE inputThread = CreateThread(NULL, 0, parallelInput, NULL, 0, NULL);
	if (inputThread== NULL) {
		udpServer.error("Cannot create input thread");
		udpServer.close();
		return -1;
	}

	std::cout << "Ready!" << std::endl;


	char message[bufferSize];
	size_t bytesRead;
	while (running) {
		memset(message, 0, bufferSize);
		bytesRead = udpServer.read((uint8_t*)message, bufferSize);
		if (bytesRead == SOCKET_ERROR) {
			continue;
		}
		std::cout << "Received: " << message << std::endl;

		/* "/reg/username password" */
		if (((std::string)message).rfind("/reg/", 0) == 0) {
			if (udpServer.getUsername(udpServer.getClientAddr()) != "") {
				continue;
			}
			std::string msg = (std::string)(message + 5);
			size_t pos = msg.find(' ');

			std::string username = msg.substr(0, pos);
			std::string password = msg.substr(pos + 1);
			std::stringstream ss;
			udpServer.registerUser(username, password);
			continue;

		}

		/* "/login/username password" */
		if (((std::string)message).rfind("/login/", 0) == 0) {
			if (udpServer.getUsername(udpServer.getClientAddr()) != "") {
				continue;
			}
			std::string msg = (std::string)(message + 7);
			size_t pos = msg.find(' ');

			std::string username = msg.substr(0, pos);
			std::string password = msg.substr(pos + 1);
			std::stringstream ss;
			udpServer.logIn(username, password);
			continue;
		}

		if (((std::string)message).rfind("To:", 0) == 0) {
			std::string msg = (std::string)(message + 3);
			size_t pos = msg.find(':');

			std::string username = msg.substr(0, pos);
			std::string user_message = msg.substr(pos+1);
			
			std::stringstream ss;
			std::string fromUsername = udpServer.getUsername(udpServer.getClientAddr());
			if (fromUsername == "") {
				std::cout << "Message from not logged user." << std::endl;
				continue;
			}
			ss << "From:" << fromUsername << ": " << user_message;
			size_t length = ss.str().size();
			if (udpServer.writeToUser((const uint8_t*)(ss.str().c_str()), length + 1, username)) {
				std::cout << "Sent message: " << ss.str() << std::endl;
			}
			continue;
		}

		if ((std::string)message == "/logout") {
			udpServer.logOut(udpServer.getClientAddr());
		}
		if ((std::string)message == "/logoutall") {
			udpServer.logOutFromAllOthers(udpServer.getClientAddr());
		}
	}

	running = false;
	WaitForSingleObject(inputThread, INFINITE);
	CloseHandle(inputThread);

	if (!udpServer.close())
	{
		return -1;
	}

	return 0;
}