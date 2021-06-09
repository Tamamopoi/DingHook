// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include<windows.h>
#include"mhook.h"  //加载HOOK库的头文件
#include"time.h"
#include"tchar.h"

HANDLE Mt1 = NULL;
HANDLE Mt2 = NULL;
HANDLE hThread = NULL;
DWORD  threadId;
wchar_t* OldMutexName = NULL;
wchar_t* OldMutexName2 = NULL;
wchar_t FilePath[MAX_PATH];
HMODULE GModule = NULL;
bool flg = true;

EXTERN_C _declspec(dllexport) void WINAPI Bester()
{

}

typedef int (WINAPI *P_MessageBoxTimeoutA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds);
P_MessageBoxTimeoutA MyMessageBoxTimeoutA = (P_MessageBoxTimeoutA)GetProcAddress(LoadLibrary(_T("user32.dll")), "MessageBoxTimeoutA");
static HANDLE(WINAPI *P_CreateMutexW)(_In_opt_ LPSECURITY_ATTRIBUTES lpMutexAttributes,_In_ BOOL bInitialOwner,_In_opt_ LPCWSTR lpName) = CreateMutexW;
DWORD WINAPI FreeDll(LPVOID lpParam)
{
	Sleep(5000);
	FreeLibraryAndExitThread(GModule, 0);
}
DWORD WINAPI KILL(LPVOID lpParam)
{
	while (flg)
	{
		if (Mt1!=NULL && Mt2 != NULL)

		{
			ReleaseMutex(Mt1);
			ReleaseMutex(Mt2);
			CloseHandle(Mt1);
			CloseHandle(Mt2);
			MyMessageBoxTimeoutA(0, "Welcome!", "Bester", MB_OK, 0, 500);
			Mhook_Unhook((PVOID*)&P_CreateMutexW);
			flg = false;

		}

	}
	CreateThread(NULL, 0, FreeDll, 0, 0, NULL);
	return 1;

}
void randstr(wchar_t *str, const int len)
{
	srand(time(NULL));
	int i;
	for (i = 0; i < len; ++i)
	{
		switch ((rand() % 3))
		{
		case 1:
			str[i] = _T('A') + rand() % 26;
			break;
		case 2:
			str[i] = _T('a') + rand() % 26;
			break;
		default:
			str[i] = _T('0') + rand() % 10;
			break;
		}
	}
	str[++i] = _T('\0');
}
HANDLE WINAPI MyCreateMutexW (_In_opt_ LPSECURITY_ATTRIBUTES lpMutexAttributes, _In_ BOOL bInitialOwner, _In_opt_ LPCWSTR lpName)
{
	if (lpName!=NULL && OldMutexName!=NULL && OldMutexName2 != NULL)
	{

	if (wcscmp(OldMutexName, lpName) == 0)
	{
		Mt1= P_CreateMutexW(lpMutexAttributes, bInitialOwner, lpName);
		return Mt1;

	}
	else if (wcscmp(OldMutexName2, lpName) == 0)
	{
		Mt2= P_CreateMutexW(lpMutexAttributes, bInitialOwner, lpName);
		return Mt2;

	}
	}
	return P_CreateMutexW(lpMutexAttributes, bInitialOwner, lpName);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	GModule = hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		if (hThread == NULL)
		{
			hThread = CreateThread(NULL, 0, KILL, 0, 0, &threadId);
		
		Mhook_SetHook((PVOID*)&P_CreateMutexW, MyCreateMutexW);
		GetModuleFileNameW(NULL, FilePath, MAX_PATH);
		TCHAR *Name = wcsrchr(FilePath, '\\');
		TCHAR Proc1[] = L"\\WeChat.exe";
		TCHAR Proc2[] = L"\\WXWork.exe";
		TCHAR Proc3[] = L"\\DingTalk.exe";
			if (wcscmp(Name, Proc1)==0)
			{
				OldMutexName = (wchar_t*)L"_WeChat_App_Instance_Identity_Mutex_Name";
				OldMutexName2 = (wchar_t*)L"WeChat_GlobalConfig_Multi_Process_Mutex";
			}
			else if (wcscmp(Name, Proc2)==0)
			{
				OldMutexName = (wchar_t*)L"Tencent.WeWork.ExclusiveObject";
				OldMutexName2 = (wchar_t*)L"Tencent.WeWork.ExclusiveObjectInstance1";
			}
			else if(wcscmp(Name, Proc3) == 0)
			{
				OldMutexName = (wchar_t*)L"Local\\{{239B7D43-86D5-4E5C-ADE6-CEC42155B475}}DingTalk";
				OldMutexName2 = (wchar_t*)L"Local\\{{239B7D43-86D5-4E5C-ADE6-CEC42155B475}}DingTalk_loginframe";
			}
			else
			{
				Mhook_Unhook((PVOID*)&P_CreateMutexW);
				flg = false;

			}
		}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

