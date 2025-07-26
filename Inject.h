#include <Windows.h>
#ifndef _GMTOOL_
typedef void (WINAPI* HackChecks)();
BOOL CallDLL()
{
	HMODULE g_hAHHandle = LoadLibrary((char*)SENC("GameGuard.dll"));
	if (g_hAHHandle)
	{
		HackChecks func = (HackChecks)GetProcAddress(g_hAHHandle,(char*)SENC("Ih4SjuVF7LCUKGMDvpdymHTc8igfZWkrw9aXNYxzROJBeqE213n0s5btPoQlA6"));//Ih4SjuVF7LCUKGMDvpdymHTc8igfZWkrw9aXNYxzROJBeqE213n0s5btPoQlA6
		if (NULL!=func)
			func();
		else
		{
			MessageBox(NULL,(char*)SENC("GameGuard is not Loaded"),"Error",MB_OK);
			return 0;
		}
	}
	else
	{
		MessageBox(NULL,(char*)SENC("GameGuard is not Loaded"),"Error",MB_OK);
		return 0;
	}
	return 1;
}
#endif