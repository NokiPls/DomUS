#include "stdafx.h"
#include "utils.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <vector>
#define ALPHABET_LEN 256
#define NOT_FOUND patlen

using namespace std;

// delta1 table: delta1[c] contains the distance between the last
// character of pat and the rightmost occurrence of c in pat.
// If c does not occur in pat, then delta1[c] = patlen.
// If c is at string[i] and c != pat[patlen-1], we can
// safely shift i over by delta1[c], which is the minimum distance
// needed to shift pat forward to get string[i] lined up 
// with some character in pat.
// this algorithm runs in alphabet_len+patlen time.
void make_delta1(int *delta1, uint8_t *pat, int32_t patlen) {
	int i;
	for (i = 0; i < ALPHABET_LEN; i++) {
		delta1[i] = NOT_FOUND;
	}
	for (i = 0; i < patlen - 1; i++) {
		delta1[pat[i]] = patlen - 1 - i;
	}
}

// true if the suffix of word starting from word[pos] is a prefix 
// of word
int is_prefix(uint8_t *word, int wordlen, int pos) {
	int i;
	int suffixlen = wordlen - pos;
	// could also use the strncmp() library function here
	for (i = 0; i < suffixlen; i++) {
		if (word[i] != word[pos + i]) {
			return 0;
		}
	}
	return 1;
}

// length of the longest suffix of word ending on word[pos].
// suffix_length("dddbcabc", 8, 4) = 2
int suffix_length(uint8_t *word, int wordlen, int pos) {
	int i;
	// increment suffix length i to the first mismatch or beginning
	// of the word
	for (i = 0; (word[pos - i] == word[wordlen - 1 - i]) && (i < pos); i++);
	return i;
}

// delta2 table: given a mismatch at pat[pos], we want to align 
// with the next possible full match could be based on what we
// know about pat[pos+1] to pat[patlen-1].
//
// In case 1:
// pat[pos+1] to pat[patlen-1] does not occur elsewhere in pat,
// the next plausible match starts at or after the mismatch.
// If, within the substring pat[pos+1 .. patlen-1], lies a prefix
// of pat, the next plausible match is here (if there are multiple
// prefixes in the substring, pick the longest). Otherwise, the
// next plausible match starts past the character aligned with 
// pat[patlen-1].
// 
// In case 2:
// pat[pos+1] to pat[patlen-1] does occur elsewhere in pat. The
// mismatch tells us that we are not looking at the end of a match.
// We may, however, be looking at the middle of a match.
// 
// The first loop, which takes care of case 1, is analogous to
// the KMP table, adapted for a 'backwards' scan order with the
// additional restriction that the substrings it considers as 
// potential prefixes are all suffixes. In the worst case scenario
// pat consists of the same letter repeated, so every suffix is
// a prefix. This loop alone is not sufficient, however:
// Suppose that pat is "ABYXCDBYX", and text is ".....ABYXCDEYX".
// We will match X, Y, and find B != E. There is no prefix of pat
// in the suffix "YX", so the first loop tells us to skip forward
// by 9 characters.
// Although superficially similar to the KMP table, the KMP table
// relies on information about the beginning of the partial match
// that the BM algorithm does not have.
//
// The second loop addresses case 2. Since suffix_length may not be
// unique, we want to take the minimum value, which will tell us
// how far away the closest potential match is.
void make_delta2(int *delta2, uint8_t *pat, int32_t patlen) {
	int p;
	int last_prefix_index = patlen - 1;

	// first loop
	for (p = patlen - 1; p >= 0; p--) {
		if (is_prefix(pat, patlen, p + 1)) {
			last_prefix_index = p + 1;
		}
		delta2[p] = last_prefix_index + (patlen - 1 - p);
	}

	// second loop
	for (p = 0; p < patlen - 1; p++) {
		int slen = suffix_length(pat, patlen, p);
		if (pat[p - slen] != pat[patlen - 1 - slen]) {
			delta2[patlen - 1 - slen] = patlen - 1 - p + slen;
		}
	}
}

uint8_t* boyer_moore(uint8_t *string, uint32_t stringlen, uint8_t *pat, uint32_t patlen) {
	int i;
	int delta1[ALPHABET_LEN];
	int *delta2 = (int *)malloc(patlen * sizeof(int));
	make_delta1(delta1, pat, patlen);
	make_delta2(delta2, pat, patlen);

	// The empty pattern must be considered specially
	if (patlen == 0) return string;

	i = patlen - 1;
	while (i < stringlen) {
		int j = patlen - 1;

		while (j >= 0 && (string[i] == pat[j])) {
			--i;
			--j;
		}
		if (j < 0) {
			free(delta2);
			return (string + i + 1);
		}

		i += max(delta1[string[i]], delta2[j]);
	}
	free(delta2);
	return NULL;
}

void scanString(uint8_t* string, uint32_t size) {

	int i = 0;

	uint8_t begin[4] = { 't', 'h','i', 's' }; // Hardcoding everywhere
	uint8_t end[7] = { 'm', 'o','t', 'h', 'e', 'r', 'f' };

	std::ofstream output("C:/Users/pc/Desktop/DOM.DMP", ios::out | ios::app);  // Hardcoded
	
	if (!output.is_open()) exit(-1);  // Can't open file.
	printf("[+] Opening DOM File ..\n");

	uint8_t *bmBegin = boyer_moore(string, size, begin, 3);
	if (bmBegin == NULL) {
		return;
	}
	uint8_t *bmEnd = boyer_moore(bmBegin, size, end, 5);
	
	if (bmBegin == NULL || bmEnd == NULL) {
		return;
	}

	std::vector<uint8_t> dom(bmBegin, bmEnd + 7); // Hardcoding
	int toRead = size - dom.size();
	int j = 1;
	while (!dom.empty()) {
		printf("Writing something..\n");
		for (int i = 0; i < dom.size(); i++)
		{
			output << dom[i];
		}
		// Move ahead in the string.
		bmBegin = boyer_moore(bmEnd + 7, toRead, &begin[0], 3);
		bmEnd = boyer_moore(bmEnd + 7, toRead, &end[0], 5);
		printf("blablabla");
		if (bmBegin != NULL || bmEnd != NULL) {
			std::vector<uint8_t> temp(bmBegin, bmEnd + 7);
			dom.swap(temp);
			toRead -= dom.size();
		}
		printf("Iteration: %d\n", j);
		j++;
	}
	output.close();

}

bool checkArchitecture(HANDLE hProcess) {
	BOOL wow64;
	// check whether we're a 64bit process
	if (IsWow64Process(GetCurrentProcess(), &wow64) && wow64)
		if (IsWow64Process(hProcess, &wow64) && !wow64)
			// 32-bit (wow) process should not touch 64-bit (non-wow) process
			return false;
	return true;
}

bool getDebugPrivilege() {
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

void scanBuffer(char* buffer, uint32_t size, uint32_t baseAddress) {
	int i = 0;
	int j = 0;

	
	printf("32 bit");

	unsigned char data[] = {
		0x3C,0x21,0x44
	};
}

//TODO: vectorBuffer ha il contenuto corretto?
void scanBuffer(unsigned char* buffer, uint64_t size, uint64_t baseAddress) {
	scanString(buffer, (uint32_t)size);
}

void scanDump(char* dump) {

	std::ifstream input(dump, std::ios::binary);
	if (!input.is_open()) exit(-1);  // Can't open file
	std::string fullDump((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	std::vector<uint8_t> vectorDump(fullDump.begin(), fullDump.end());
	scanString(&vectorDump[0], vectorDump.size());
	
}


std::ifstream::pos_type Filesize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}
