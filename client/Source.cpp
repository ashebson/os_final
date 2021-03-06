#include "winsockfuncs.h"
#include <iostream>
#include <string>
#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "8850"
#define DEFAULT_IP "127.0.0.1"

int main() {
	try {
		SOCKET socket = connectClientSocket(DEFAULT_PORT,DEFAULT_IP);
		char sendmsg[DEFAULT_BUFLEN];
		while (true) {
			scanf_s("%s", sendmsg, DEFAULT_BUFLEN);
			sendMessage(sendmsg, socket);
			if (strcmp(sendmsg, "CLOSE") == 0) {
				break;
			}
			if (strcmp(sendmsg, "UPDATE") == 0) {
				PCHAR buff;
				CHAR path[DEFAULT_BUFLEN];
				CHAR txt_path[DEFAULT_BUFLEN];
				DWORD fin_size;
				fgets(path, DEFAULT_BUFLEN, stdin);
				path[strlen(path) - 1] = '\0';
				char* help = path + 1;
				HANDLE fin = CreateFileA(help, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				fin_size = GetFileSize(fin, NULL);
				buff = (PCHAR)malloc(fin_size);
				ReadFile(fin, buff, fin_size, NULL, NULL);
				PCHAR fin_size_str = (PCHAR)malloc(DEFAULT_BUFLEN);
				sprintf_s(fin_size_str,DEFAULT_BUFLEN, "%d", fin_size);
				sendMessage(fin_size_str, socket);
				sendMessage(buff, socket,fin_size);
			}
			char* recvmsg = recvMessage(socket);
			if (strcmp(recvmsg, "ENTER_PARAM") == 0) {
				fgets(sendmsg, DEFAULT_BUFLEN, stdin);
				sendmsg[strlen(sendmsg) - 1] = '\0';
				char* help = sendmsg+1;
				sendMessage(help, socket);
			}
			else {
				printf("server sent: %s\n", recvmsg);
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