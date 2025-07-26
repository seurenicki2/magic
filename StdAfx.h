#pragma once

#define _WIN32_WINNT	0x0501
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _FOR_MUKHYANG
#define _CLIENT_
#define _CRYPTCHECK_

#ifdef _GMTOOL_
#define _CHEATENABLE_
#endif
#define _ASSERTENABLE_

#define _ITERATOR_DEBUG_LEVEL 0
#pragma warning(disable:4100) // 참조하지 않는 매개변수
#pragma warning(disable:4127) // 조건식이 상수인 경우
#pragma warning(disable:4201) // 비표준 확장을 사용한경우 내부에 Struct 만들때 이름을 적지 않았음.
#pragma warning(disable:4512) // 대입 연산자를 생성하지 못했습니다. 
//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#define new DEBUG_NEW
//#endif
#include <windows.h>
#include <wingdi.h>
#include <ole2.h>
#include <initguid.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <crtdbg.h>
#include <vfw.h>
#include <dbghelp.h>
#include <time.h>
#include <shellapi.h.>
#include <direct.h>
#include <conio.h>
#include <map>
#include <set>
#include <hash_map>
#include <hash_set>
#include <list>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <functional>
#include <queue>
#include "StringCryptor.h"
#include "resource.h"

#include "..\[lib]yhlibrary\yhlibrary.h"
#include "..\[lib]yhlibrary\cLinkedList.h"
#include "..\[lib]yhlibrary\PtrList.h"
#include "..\[lib]yhlibrary\IndexGenerator.h"
#include "..\4DyuchiGXGFunc\global.h"
#include "..\[CC]Header\protocol.h"
#include "..\[CC]Header\CommonDefine.h"
#include "..\[CC]Header\CommonGameDefine.h"
#include "..\[CC]Header\ClientGameDefine.h"
#include "..\[CC]Header\CommonStruct.h"
#include "..\[CC]Header\CommonGameFunc.h"

#include "Motion.h"
#include "MHNetwork.h"
#include "ErrorMsg.h"
#include "DirectoryManager.h"

#import <shdocvw.dll>

#ifdef _TL_LOCAL_
#include "ThaiBreaker.h"
#endif

extern BOOL g_bUsingEnginePack;
//Protect
#pragma comment(lib, "Urlmon.lib")  //download da internet
#include <urlmon.h>                 //download da internet
#include <iterator>                 // for copy and past
#include <windows.h>
#include <tlhelp32.h>
#include <process.h>
#include <fstream>
#include "winsock.h"
#include <string>
#include "windowsx.h"
#include <iostream>   // coat << <<
#include <tchar.h>    //to php POST
#include <map>        //compare words ban txt
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "iphlpapi")    //to get mac address
#include <iphlpapi.h>               //to get mac address
#include <assert.h>                 //to get mac address 

#include <wininet.h>                //to ftp server / php
#pragma comment(lib, "Wininet")     //to ftp server
#pragma comment(lib,"wininet.lib")  //to php POST
//#include "se.h"
#include "START.h"
#include "Classe.h"
#include "CRC.h"

#define WIN_32_LEAN_AND_MEAN
#define SECURITY_WIN32
#pragma warning (disable: 4996 4091 4101 4018 4309)

using namespace std;