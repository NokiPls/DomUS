#pragma once
#ifndef __UTILS_H
#define __UTILS_H
#include <windows.h>
#include <stdint.h>
#include <fstream>

// Tries to escalate privileges to debug other processes.
bool getDebugPrivilege();

#ifdef _WIN32 | _WIN64
// Scans a buffer.
void scanBuffer(unsigned char* buffer, uint32_t size, uint32_t baseAddress);
#ifdef _WIN64
// Scans a buffer.
void scanBuffer(unsigned char* buffer, uint64_t size, uint64_t baseAddress);
// Scans a memory dump.
void scanDump(char* buffer);
#else
// Scans a buffer for valid key schedules. Prints the found keys.
void ScanBuffer(char* buffer, uint32_t size, uint32_t baseAddress);
// Prints a key found in a buffer.
#endif
#endif


// Returns the filesize of a file.
std::ifstream::pos_type Filesize(const char* filename);

// Checks whether we are running as a 32 bit application
// and trying to analyze a 64 bit one.
bool checkArchitecture(HANDLE hProcess);
uint8_t* boyer_moore(uint8_t *string, uint32_t stringlen, uint8_t *pat, uint32_t patlen);
#endif#pragma once
