#pragma once
#include <Windows.h>
VOID autorunProj(const char* kname, const char* projpath) {
	const char* kpath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	HKEY hk;
	RegOpenKeyA(HKEY_CURRENT_USER, kpath, &hk);
	RegSetKeyValueA(hk, NULL, kname, REG_SZ, projpath, strlen(projpath)*sizeof(CHAR));
	RegCloseKey(hk);
}