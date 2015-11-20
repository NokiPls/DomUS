#pragma once
#ifndef __UTILS_H
#define __UTILS_H
#include <windows.h>
#include <stdint.h>
#include <fstream>

// Tries to escalate privileges to debug other processes.
bool GetDebugPrivilege();

#ifdef _WIN32 | _WIN64
#ifdef _WIN64
// Scans a buffer for valid key schedules. Prints the found keys.
bool ScanBuffer(unsigned char* buffer, uint64_t size, uint64_t baseAddress);
// Prints a key found in a buffer.
void PrintKey(unsigned char* key, uint64_t size, uint64_t baseAddress);
#else
// Scans a buffer for valid key schedules. Prints the found keys.
bool ScanBuffer(unsigned char* buffer, uint32_t size, uint32_t baseAddress);
// Prints a key found in a buffer.
void PrintKey(unsigned char* key, uint32_t size, uint32_t baseAddress);
#endif
#endif


// Returns the filesize of a file.
std::ifstream::pos_type Filesize(const char* filename);

// Checks whether we are running as a 32 bit application
// and trying to analyze a 64 bit one.
bool CheckArchitecture(HANDLE hProcess);

#endif#pragma once
