#include "stdafx.h"
#include "DXFramework.h"
#include "Control/TinyControl.h"

namespace DXFramework
{
	typedef HANDLE(WINAPI *CREATEREMOTETHREAD)(HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
	typedef BOOL(WINAPI *WRITEPROCESSMEMORY)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
	typedef LPVOID(WINAPI *VIRTUALALLOCEX)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD);
	typedef BOOL(WINAPI *VIRTUALFREEEX)(HANDLE, LPVOID, SIZE_T, DWORD);
	typedef HANDLE(WINAPI *LOADLIBRARY) (DWORD, BOOL, DWORD);
	typedef HANDLE(WINAPI *FREELIBRARY) (HMODULE);

	REFGUID GetWICCodec(D3DX11_IMAGE_FILE_FORMAT format)
	{
		switch (format)
		{
		case DXFramework::JPG:
			return GUID_ContainerFormatJpeg;
			break;
		case DXFramework::PNG:
			return GUID_ContainerFormatPng;
			break;
		case DXFramework::TIFF:
			return GUID_ContainerFormatTiff;
			break;
		case DXFramework::GIF:
			return GUID_ContainerFormatGif;
			break;
		case DXFramework::WMP:
			return GUID_ContainerFormatWmp;
			break;
		case DXFramework::BMP:
			return GUID_ContainerFormatBmp;
		}
		return GUID_ContainerFormatBmp;
	}

	BOOL WINAPI InjectLibrary(HANDLE hProcess, const CHAR *pszDLL)
	{
		if (!hProcess || !pszDLL)
			return FALSE;
		HMODULE hInstance = NULL;
		HANDLE	hTask = NULL;
		FARPROC	address = NULL;
		hInstance = GetModuleHandle(TEXT("KERNEL32"));
		if (!hInstance)
			return FALSE;
		CREATEREMOTETHREAD pCREATEREMOTETHREAD = (CREATEREMOTETHREAD)GetProcAddress(hInstance, TEXT("CreateRemoteThread"));
		if (!pCREATEREMOTETHREAD)
			return FALSE;
		WRITEPROCESSMEMORY pWRITEPROCESSMEMORY = (WRITEPROCESSMEMORY)GetProcAddress(hInstance, TEXT("WriteProcessMemory"));
		if (!pWRITEPROCESSMEMORY)
			return FALSE;
		VIRTUALALLOCEX pVIRTUALALLOCEX = (VIRTUALALLOCEX)GetProcAddress(hInstance, TEXT("VirtualAllocEx"));
		if (!pVIRTUALALLOCEX)
			return FALSE;
		VIRTUALFREEEX pVIRTUALFREEEX = (VIRTUALFREEEX)GetProcAddress(hInstance, TEXT("VirtualFreeEx"));
		if (!pVIRTUALFREEEX)
			return FALSE;
		DWORD dwSize = strlen(pszDLL);
		dwSize = (dwSize + 1) * sizeof(CHAR);
		LPVOID pAlloc = (LPVOID)(*pVIRTUALALLOCEX)(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!pAlloc)
			return FALSE;
		SIZE_T size = 0;
		BOOL bRes = (*pWRITEPROCESSMEMORY)(hProcess, pAlloc, (LPVOID)pszDLL, dwSize, &size);
		if (!bRes)
			goto _ERROR;
		address = (FARPROC)GetProcAddress(hInstance, TEXT("LoadLibraryA"));
		if (!address)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		hTask = (*pCREATEREMOTETHREAD)(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), pAlloc, 0, 0);
		if (!hTask)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		if (WaitForSingleObject(hTask, 2000) == WAIT_OBJECT_0)
		{
			DWORD dw;
			GetExitCodeThread(hTask, &dw);
			bRes = dw != 0;
		}
	_ERROR:
		if (hTask != NULL)
		{
			CloseHandle(hTask);
			hTask = NULL;
		}
		if (pAlloc != NULL)
		{
			(*pVIRTUALFREEEX)(hProcess, pAlloc, 0, MEM_RELEASE);
			pAlloc = NULL;
		}
		return bRes;
	}
	BOOL WINAPI UninjectLibrary(HANDLE hProcess, const CHAR *pszDLL)
	{
		if (!hProcess || !pszDLL)
			return FALSE;
		HMODULE hInstance = NULL;
		HANDLE	hTask = NULL;
		FARPROC	address = NULL;
		hInstance = GetModuleHandle(TEXT("KERNEL32"));
		if (!hInstance)
			return FALSE;
		CREATEREMOTETHREAD pCREATEREMOTETHREAD = (CREATEREMOTETHREAD)GetProcAddress(hInstance, TEXT("CreateRemoteThread"));
		if (!pCREATEREMOTETHREAD)
			return FALSE;

#if defined(_WIN64)
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(hProcess));
		if (hSnapshot == INVALID_HANDLE_VALUE)
			return FALSE;
		MODULEENTRY32 me;
		ZeroMemory(&me, sizeof(MODULEENTRY32));
		me.dwSize = sizeof(MODULEENTRY32);
		if (!Module32First(hSnapshot, &me))
			return FALSE;
		BOOL bRes = TRUE;
		do
		{
			if (strncasecmp(pszDLL, me.szExePath, strlen(pszDLL)) == 0)
			{
				bRes = TRUE;
				break;
			}
		} while (Module32Next(hSnapshot, &me));
		if (!bRes)
		{
			goto _ERROR;
		}
		address = (FARPROC)GetProcAddress(hInstance, TEXT("FreeLibrary"));
		if (!address)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		hTask = (*pCREATEREMOTETHREAD)(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), (LPVOID)me.modBaseAddr, 0, 0);
		if (!hTask)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		if (WaitForSingleObject(hTask, 2000) == WAIT_OBJECT_0)
		{
			DWORD dw;
			GetExitCodeThread(hTask, &dw);
			bRes = dw != 0;
		}
	_ERROR:
		if (hTask != NULL)
		{
			CloseHandle(hTask);
			hTask = NULL;
		}
		TRACE("UninjectLibrary\n");
		return bRes;
#else
		WRITEPROCESSMEMORY pWRITEPROCESSMEMORY = (WRITEPROCESSMEMORY)GetProcAddress(hInstance, TEXT("WriteProcessMemory"));
		if (!pWRITEPROCESSMEMORY)
			return FALSE;
		VIRTUALALLOCEX pVIRTUALALLOCEX = (VIRTUALALLOCEX)GetProcAddress(hInstance, TEXT("VirtualAllocEx"));
		if (!pVIRTUALALLOCEX)
			return FALSE;
		VIRTUALFREEEX pVIRTUALFREEEX = (VIRTUALFREEEX)GetProcAddress(hInstance, TEXT("VirtualFreeEx"));
		if (!pVIRTUALFREEEX)
			return FALSE;
		DWORD dwSize = strlen(pszDLL);
		dwSize = (dwSize + 1) * sizeof(CHAR);
		LPVOID pAlloc = (LPVOID)(*pVIRTUALALLOCEX)(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!pAlloc)
			return FALSE;
		SIZE_T size = 0;
		BOOL bRes = (*pWRITEPROCESSMEMORY)(hProcess, pAlloc, (LPVOID)pszDLL, dwSize, &size);
		if (!bRes)
			goto _ERROR;
		address = (FARPROC)GetProcAddress(hInstance, TEXT("GetModuleHandleA"));
		if (!address)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		hTask = (*pCREATEREMOTETHREAD)(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), pAlloc, 0, 0);
		if (!hTask)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		DWORD dwHandle = 0;
		if (WaitForSingleObject(hTask, 2000) == WAIT_OBJECT_0)
		{
			GetExitCodeThread(hTask, &dwHandle);
			if (dwHandle == 0)
			{
				bRes = FALSE;
				goto _ERROR;
			}
		}
		address = (FARPROC)GetProcAddress(hInstance, TEXT("FreeLibrary"));
		if (!address)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		hTask = (*pCREATEREMOTETHREAD)(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), (LPVOID)dwHandle, 0, 0);
		if (!hTask)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		if (WaitForSingleObject(hTask, 2000) == WAIT_OBJECT_0)
		{
			DWORD dw;
			GetExitCodeThread(hTask, &dw);
			bRes = dw != 0;
		}
	_ERROR:
		if (hTask != NULL)
		{
			CloseHandle(hTask);
			hTask = NULL;
		}
		if (pAlloc != NULL)
		{
			(*pVIRTUALFREEEX)(hProcess, pAlloc, 0, MEM_RELEASE);
			pAlloc = NULL;
		}
		TRACE("UninjectLibrary\n");
		return bRes;
#endif
	}
	BOOL WINAPI ProcessExists(const TinyString& exeName, PROCESSINFO& ps)
	{
		DWORD processes[1024];
		DWORD cbNeeded = 0;
		if (!EnumProcesses(processes, sizeof(processes), &cbNeeded))
			return FALSE;
		DWORD dwCount = cbNeeded / sizeof(DWORD);
		for (UINT i = 0;i < dwCount;i++)
		{
			ps.dwProcessID = processes[i];
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ps.dwProcessID);
			DWORD size = MAX_PATH;
			CHAR windowExecutable[MAX_PATH];
			if (QueryFullProcessImageName(hProcess, 0, windowExecutable, &size))
			{
				CHAR* pzName = PathFindFileName(windowExecutable);
				if (strncasecmp(pzName, exeName.STR(), strlen(pzName)) == 0)
				{
					CloseHandle(hProcess);
					return TRUE;
				}
			}
			CloseHandle(hProcess);
			hProcess = NULL;
		}
		return FALSE;
	}
}