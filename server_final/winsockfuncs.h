#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <Windows.h>
#define DEFAULT_BUFLEN 1024


#pragma comment(lib, "Ws2_32.lib")

SOCKET connectServerSocket(const char* port)
{
	const char* port_num = port;
	// Initialize Winsock
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		char* err = (char*)malloc(DEFAULT_BUFLEN);
		sprintf_s(err, DEFAULT_BUFLEN, "WSAStartup failed: %d\n", iResult);
		throw(err);
	}
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, port_num, &hints, &result);
	if (iResult != 0) {
		char* err = (char*)malloc(DEFAULT_BUFLEN);
		sprintf_s(err, DEFAULT_BUFLEN, "getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		throw(err);
	}
	SOCKET ListenSocket = INVALID_SOCKET;
	// Create a SOCKET for the server to listen for client connections

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		char* err = (char*)malloc(DEFAULT_BUFLEN);
		sprintf_s(err, DEFAULT_BUFLEN, "Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		throw(err);
	}
	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		char* err = (char*)malloc(DEFAULT_BUFLEN);
		sprintf_s(err, DEFAULT_BUFLEN, "bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		throw(err);
	}
	else {
		printf("server online\n");
	}
	freeaddrinfo(result);
	//listen
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		char* err = (char*)malloc(DEFAULT_BUFLEN);
		sprintf_s(err, DEFAULT_BUFLEN, "Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		throw(err);
	}
	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		char* err = (char*)malloc(DEFAULT_BUFLEN);
		sprintf_s(err, DEFAULT_BUFLEN, "accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		throw(err);
	}
	printf("connection established\n");
	return ClientSocket;
}

char* recvMessage(SOCKET socket, int size = DEFAULT_BUFLEN) {
	int msglen = 1;
	char* recvbuf = (char*)malloc(size + 1);
	int iSendResult;
	int recvbuflen = size;
	msglen = recv(socket, recvbuf, recvbuflen, 0);
	return recvbuf;
}

void sendMessage(const char* msg, SOCKET socket, int size = DEFAULT_BUFLEN) {
	int iSendResult = send(socket, msg, size, 0);
	if (iSendResult == SOCKET_ERROR) {
		char* err = (char*)malloc(DEFAULT_BUFLEN);
		sprintf_s(err, DEFAULT_BUFLEN, "send failed: %d\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		throw (err);
	}
}

void closeSocket(SOCKET& socket) {
	int iResult = shutdown(socket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		char* err = (char*)malloc(DEFAULT_BUFLEN);
		sprintf_s(err, DEFAULT_BUFLEN, "shutdown failed: %d\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		throw (err);
	}
	// cleanup
	closesocket(socket);
	WSACleanup();
}

SOCKET connectClientSocket(const char* port, const char* ip) {
	const char* port_num = port;
	const char* ip_address = ip;
	// Initialize Winsock
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		char* err = (char*)malloc(DEFAULT_BUFLEN);
		sprintf_s(err, DEFAULT_BUFLEN, "WSAStartup failed: %d\n", iResult);
		throw(err);
	}
	// Initialize Socket
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	// Resolve the server address and port
	iResult = getaddrinfo(ip_address, port_num, &hints, &result);
	if (iResult != 0) {
		char* err = (char*)malloc(DEFAULT_BUFLEN);
		sprintf_s(err, DEFAULT_BUFLEN, "getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		throw(err);
	}
	SOCKET ConnectSocket = INVALID_SOCKET;
	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;
	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		char* err = (char*)malloc(DEFAULT_BUFLEN);
		sprintf_s(err, DEFAULT_BUFLEN, "Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		throw(err);
	}
	// Connect to server.
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}


	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		WSACleanup();
		throw("Unable to connect to server!\n");
	}
	printf("connection established\n");
	return ConnectSocket;
}

char* recvFile(SOCKET socket, int size) {
	const int reg_size = 1;
	int current_size = 0;
	int rounds = 0;
	char* buff = (char*)malloc(size);
	char* out = buff;
	if (size % reg_size == 0) {
		rounds = size / reg_size;
	}
	else {
		rounds = size / reg_size + 1;
	}
	for (int i = 0; i < rounds; i++) {
		if (size < reg_size) {
			current_size = size;
		}
		else {
			current_size = reg_size;
		}
		recv(socket, buff, current_size, 0);
		buff += reg_size;
		size -= reg_size;
	}
	return out;
}

void sendFile(SOCKET socket, char* file, int size) {
	const int reg_size = 1;
	int current_size = 0;
	int rounds = 0;
	if (size % reg_size == 0) {
		rounds = size / reg_size;
	}
	else {
		rounds = size / reg_size + 1;
	}
	for (int i = 0; i < rounds; i++) {

		if (size < reg_size) {
			current_size = size;
		}
		else {
			current_size = reg_size;
		}
		send(socket, file, current_size, 0);
		file += reg_size;
		size -= reg_size;
	}
}