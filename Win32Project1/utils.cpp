#include "stdafx.h"
#include "utils.h"
#include <stdio.h>
#include <fstream>
#include <iostream>


bool CheckArchitecture(HANDLE hProcess) {
	BOOL wow64;
	// check whether we're a 64bit process
	if (IsWow64Process(GetCurrentProcess(), &wow64) && wow64)
		if (IsWow64Process(hProcess, &wow64) && !wow64)
			// 32-bit (wow) process should not touch 64-bit (non-wow) process
			return false;
	return true;
}

bool GetDebugPrivilege() {
	TOKEN_PRIVILEGES tp;
	LUID luid;
	HANDLE hToken;
	LPCTSTR lpszPrivilege = SE_DEBUG_NAME;


	if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
		printf("[-] LookupPrivilegeValue error: 0x%x\n", GetLastError());
		return false;
	}

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		// Enable the privilege

		if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {
			printf("[-] AdjustTokenPrivileges error: 0x%x\n", GetLastError());
			return false;
		}

		if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
			printf("[-] The token does not have the specified privilege. \n");
			return false;
		}
		return true;
	}
}

bool ScanBuffer(unsigned char* buffer, uint32_t size, uint32_t baseAddress) {
	int i = 0;
	int j = 0;

	


	unsigned char data[] = {
		0x3C,0x21,0x44
	};
	
	return false;
}

bool ScanBuffer(unsigned char* buffer, uint64_t size, uint64_t baseAddress) {
	int i = 0;
	int j = 0;

	unsigned char docType[] = {
	0x3C, 0x21, 0x44
	};

	unsigned char htmlEnd[] = {
	0x3C,0x2F,0x68,0x74,0x6D,0x6C,0x3E
	};

	FILE *f = fopen("C:/Users/NcMn/Desktop/DOM.txt", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	
	for (; i < size - 2; i++) {
		if (buffer[i] == docType[0] && buffer[i + 1] == docType[1] && buffer[i + 2] == docType[2]) {
			while(!(buffer[i] == htmlEnd[0] && buffer[i + 1] == htmlEnd[1] && buffer[i + 2] == htmlEnd[2] && buffer[i + 3] == htmlEnd[3]) && i < size){
				fprintf(f, "%c", buffer[i]);
				printf("%c", buffer[i]);
				i++;
			}
		}
	}
	fclose(f);
	return true;
}


std::ifstream::pos_type Filesize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}