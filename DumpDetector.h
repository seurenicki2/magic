#pragma once


//max cheat list
#define MAX_MEMORY_DUMP_OFFSETS 28
#define MAX_MEMORY_DUMP_SIZE 32
#define MAX_PROCESS_DATABASE 28

class CGameClientAntiCheat
{

public:

typedef struct ANTIHACK_DBDUMP
{
	unsigned int m_aOffset;
	unsigned char m_aMemDump[MAX_MEMORY_DUMP_SIZE];

} *PANTIHACK_DBDUMP;

ANTIHACK_DBDUMP g_ProcessesDumps[MAX_PROCESS_DATABASE];

	void SystemProcessesScanClient();
	bool ScanProcessMemoryClient(HANDLE hProcess);

	int ProtectionDump1();
	//void D_scan();
	void ClasseCheckPross1();
	void GetProcId1(char* ProcName);

public:
	CGameClientAntiCheat ()		{}
	~CGameClientAntiCheat ()		{}
};
