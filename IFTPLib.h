#pragma once
/*
	������ ftp ���̺귯��.

	������ : ������
	��ģ�� : ������
	���� ��ģ ��¥ : 2010-03-08

	lib�� �����پ� ��� 
	�����ƴԿ��� ������ �⵵�� �ø���,
	�ش� ���̺귯���� ��ũ��Ű��,
	CreateFTPInterface() �� �������̽��� ���´�.
*/


enum eRETURN_CODE
{
	eRETURN_CODE_INVALID,
	eRETURN_CODE_OK,
	eRETURN_CODE_USERNAME_
};

// �ݹ�.
typedef void (_stdcall *FileCallBack)(int iNow, int iMax, char* pszFileName);

// ftp ��������.
struct IFtpLib
{
	void	virtual Initialize() = 0;
	void	virtual	Cleanup() = 0;							// delete this.

	BOOL	virtual Connect(char* pszIP, WORD wPort) = 0;
	void	virtual Disconnect() = 0;
	BOOL	virtual GetIP_FromDNS(char* pszDNS, char* pszOutIP) = 0;

	BOOL	virtual CD_Down(char* pszName) = 0;				// change directory.
	BOOL	virtual CD_Up() = 0;

	BOOL	virtual Login(char* pszUser, char* pszPass) = 0;

	BOOL	virtual Download_File(char* pszDownloadRootPath, char* pszFileName, BOOL bActiveMode, FileCallBack pcbPart) = 0;
	BOOL	virtual Upload_File(char* pszUploadRootPath, char* pszFileName, BOOL bActiveMode, FileCallBack pcbPart) = 0;
	//bActiveMode (0:Passive, 1:Active)
};




HRESULT __stdcall CreateFTPInterface(IFtpLib** ppv);




// CreateFTPInterface ��.
HRESULT typedef (__stdcall *CREATE_FTP_INSTANCE_FUNC)(IFtpLib** ppv);

// dll �� ���ܾ� �� �Ʒ� �ڵ带 ���� ��.
inline IFtpLib* CreateFTPLib(void)
{
	IFtpLib*	pFtp = NULL;
	HMODULE		hDLL = NULL;

#ifdef _WIN64
	#ifdef _DEBUG
		hDLL = LoadLibraryW(L"FTPLib_x64d.dll");
	#else
		hDLL = LoadLibraryW(L"FTPLib_x64.dll");
	#endif
#else
	#ifdef _DEBUG
		hDLL = LoadLibraryW(L"FTPLib_x86d.dll");
	#else
		hDLL = LoadLibraryW(L"FTPLib_x86.dll");
	#endif
#endif

	if( hDLL)
	{
		CREATE_FTP_INSTANCE_FUNC pFunc = (CREATE_FTP_INSTANCE_FUNC)GetProcAddress( hDLL, "CreateFTPInterface");
		if (pFunc)
		{
			HRESULT hr = pFunc((IFtpLib**)&pFtp);
			if (hr == S_OK)
			{
				pFtp->Initialize();
			}
		}	
	}

	return pFtp;
}


// ���̺귯���� �� ���.
// �Ʒ� �ڵ带 �ܾ�� ����.
//#include "../include/IErrorReportLib.h"
//#ifdef _DEBUG
//#ifdef _WIN64
//#pragma comment( lib, "../lib/ErrorReportLib_x64d.lib")
//#else
//#pragma comment( lib, "../lib/ErrorReportLib_x86d.lib")
//#endif
//#else
//#ifdef _WIN64
//#pragma comment( lib, "../lib/ErrorReportLib_x64.lib")
//#else
//#pragma comment( lib, "../lib/ErrorReportLib_x86.lib")
//#endif
//#endif
