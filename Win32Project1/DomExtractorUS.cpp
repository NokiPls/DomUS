// Win32Project1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <fstream>
#include <string>
#include "utils.h"
#include <vector>
#include <iostream>

using namespace std;


void scanMemory(HANDLE hProcess) {
	MEMORY_BASIC_INFORMATION memInfo;
	SYSTEM_INFO sysInfo;
	unsigned long minimumAddress;
	unsigned long maximumAddress;
	unsigned long currentAddress;
	std::vector<unsigned char> memoryBuffer;

	// MEMORY MAPPING
	GetSystemInfo(&sysInfo);

	// Getting user mode memory ranges
	minimumAddress = (unsigned long)sysInfo.lpMinimumApplicationAddress;
	maximumAddress = (unsigned long)sysInfo.lpMaximumApplicationAddress;
	currentAddress = minimumAddress;

	// Scanning the module memory
	while (currentAddress < maximumAddress) {
		// getting page info
		if (VirtualQueryEx(hProcess, (LPCVOID)currentAddress, &memInfo, sizeof(memInfo))) {
			// checking if current address is in the module address range
			if ((memInfo.Protect & (PAGE_READONLY | PAGE_READWRITE))) {
				memoryBuffer.resize(memInfo.RegionSize);
				if (ReadProcessMemory(hProcess, (LPCVOID)currentAddress, &memoryBuffer[0], memInfo.RegionSize, NULL)) {
					scanBuffer(&memoryBuffer[0], memInfo.RegionSize, currentAddress);
				}
			}
		}
		currentAddress += memInfo.RegionSize;
	}
}

void scanProcess(int processID) {
	TCHAR szProcessName[MAX_PATH];
	DWORD cbNeeded;
	HANDLE hProcess;

	if (!getDebugPrivilege()) {
		printf("[-] Couldn't get debug privileges.");
		return;
	}

	// Get a handle to the process.
	hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, TRUE, processID);
	if (hProcess == NULL) {
		printf("[-] PID %d not found.\n", processID);
		return;
	}

	if (!checkArchitecture(hProcess)) {
		printf("[-] 32 bit process can't examine 64 bit ones...\n");
		return;
	}

	printf("[+] Opened process handle.\n");
	printf("[+] Scanning memory...\n");
	scanMemory(hProcess);
	CloseHandle(hProcess);
}


int main(int argc, char * argv[])
{
	int pid;
	if (argc == 2) {
		// we're going to search in a process
		if (argv[1][0] == '-') {
			pid = atoi(argv[1] + 1);
			scanProcess(pid);
		}
		else
			scanDump(argv[1]);
	}
	else {
		printf("Usage: %s -PID | dumpfile\n", argv[0]);
	}
	std::cin.get();
	return 0;
}
