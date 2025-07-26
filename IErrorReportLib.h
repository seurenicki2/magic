/*
	���� ����Ʈ ���̺귯��.
	������ : ������
	���� ������ ��¥ : 2010-03-08


	ftp�� �нú���θ� �����Ѵ�.
*/
#include <dbghelp.h>

struct INIT_DUMP_HANDLER
{
	int				iBuildNum;				// ���������� �̸��� ������ �� ���� ����ѹ�.
	MINIDUMP_TYPE	dumpType;				// MiniDumpNormal : ���� �⺻���� ����. ���� ũ�Ⱑ �����۴�. �⺻ ���ø� ���.
											// MiniDumpWithFullMemory : ���� ū ����. �½�ũ�޴����� ������ �޸𸮻�뷮��ŭ�� ũ�⸦ ������. ��� �޸������� ����.
	char			szFtpServerIP[16];		// ftp ������ �ּ�. �������� ���� ��� dump�� ����.
	int				iFtpServerPort;			// ftp ������ ��Ʈ.
	char			szUserName[16];			// ftp ���� ����.		15�� ����.
	char			szPass[16];				// ftp ���� ��ȣ.		15�� ����.

	HINSTANCE		hApplicationInstance;
	BOOL			bUseDescriptionDialog;		// ��Ȳ ���� â�� ����. ������ Ǯ��ũ������ ���ų� �� �� �ҿ� �����Ƿ� FALSE�� �α� ��õ.

};

// �ʱ�ȭ �Լ�.
void InitErrorDumpHandler( INIT_DUMP_HANDLER* pInitHandler);

// �ͼ��� ���� �ڵ鷯.
// SetUnhandledExceptionFilter( ErrorDumpHandler); // �̷��� �����ϸ� �ȴ�.
LONG WINAPI ErrorDumpHandler( EXCEPTION_POINTERS *pE);

// ���Ƿ� ������.
//void SendErrorDumpToServer(void);			// �ڵ� �߰��� ���Ƿ� ������ ���� ��� ����.

//
//// �Ʒ��� ���� �Լ�.
//// �䷸�� �ɾ�����ø� �˴ϴ�.
//void inline InitExceptionFilter( HINSTANCE hInstance)
//{
//	INIT_DUMP_HANDLER init;
//	memset( &init, 0, sizeof(init));
//	init.iBuildNum	=	1;
//	init.dumpType	=	MiniDumpNormal;
//	strcpy_s( init.szFtpServerIP, 16, "211.232.63.107");
////	strcpy_s( init.szFtpServerIP, 16, "127.0.0.1");
//	init.iFtpServerPort	=	4321;
//	strcpy_s( init.szUserName, 16, "dumpdemo");
//	strcpy_s( init.szPass, 16, "dumpdemo");
//	init.hApplicationInstance = hInstance;
//	init.bUseDescriptionDialog = TRUE;
//
//	InitErrorDumpHandler( &init);		// szFtpServerIP == NULL �� ��� dump �� ����.
//	SetUnhandledExceptionFilter( ErrorDumpHandler);
//}
