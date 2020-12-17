#include "winsockfuncs.h"
#include <iostream>
#include <string>
#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "8850"
#define DEFAULT_IP "127.0.0.1"
#define BIG_BUFLEN 65536

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
				CHAR buff[BIG_BUFLEN];
				CHAR path[DEFAULT_BUFLEN];
				CHAR txt_path[DEFAULT_BUFLEN];
				DWORD fin_size;
				scanf_s("%s", path, DEFAULT_BUFLEN);
				HANDLE fin = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				ReadFile(fin, buff, BIG_BUFLEN, &fin_size, NULL);
				PCHAR fin_size_str = (PCHAR)malloc(DEFAULT_BUFLEN);
				sprintf_s(fin_size_str,DEFAULT_BUFLEN, "%d", fin_size);
				sendMessage(buff, socket,BIG_BUFLEN);
				sendMessage(fin_size_str, socket);
			}
			char* recvmsg = recvMessage(socket);
			if (strcmp(recvmsg, "ENTER_PARAM") == 0) {
				scanf_s("%s", sendmsg, DEFAULT_BUFLEN);
				sendMessage(sendmsg, socket);
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