#ifndef DUMP_DETECTOR_H
#define DUMP_DETECTOR_H

//max cheat list
#define MAX_MEMORY_DUMP_OFFSETS 500
#define MAX_MEMORY_DUMP_SIZE 16
#define MAX_PROCESS_DATABASE 500

typedef struct ANTIHACK_DBDUMP
{
	unsigned int m_aOffset;
	unsigned char m_aMemDump[MAX_MEMORY_DUMP_SIZE];

} *PANTIHACK_DBDUMP;

extern ANTIHACK_DBDUMP g_ProcessesDumps[MAX_PROCESS_DATABASE];

void SystemProcessesScanClient();
bool ScanProcessMemoryClient(HANDLE hProcess);
/*
typedef struct {
	unsigned long dwAddress;
	unsigned char pBytes[32];
} MDumpNew;
bool MCheckDumpNew(void);
*/

#endif
