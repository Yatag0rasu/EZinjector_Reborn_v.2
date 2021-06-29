#include <Windows.h>
#include <iostream>
#include "memory.hpp"
#include <string>

using namespace std;

char namedll[32];

char process_name[32];

Injector inj;

DWORD pid;

DWORD ProcessId(char* process_name)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	DWORD proc_id = 0;
	if (Process32First(snapshot, &process))
	{
		while (Process32Next(snapshot, &process))
		{
			if (_stricmp(process.szExeFile, process_name) == 0)
			{
				proc_id = process.th32ProcessID;
				break;
			}
		}
	}
	CloseHandle(snapshot);
	return proc_id;
}

bool DoesFileExist(const char* name) {
	if (FILE* file = fopen(name, "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

LPVOID ntOpenFile = GetProcAddress(LoadLibraryW(L"ntdll"), "NtOpenFile");

void bypass()
{
	// Restore original NtOpenFile from external process
	if (ntOpenFile) {
		char originalBytes[5];
		memcpy(originalBytes, ntOpenFile, 5);
		WriteProcessMemory(inj.process, ntOpenFile, originalBytes, 5, NULL);
	} 
	else
	{
		cout << "Unable to bypass.\n";
	}
}

void Backup()
{
	if (ntOpenFile) {
		//So, when I patching first 5 bytes I need to backup them to 0? (I think)
		char originalBytes[5];
		memcpy(originalBytes, ntOpenFile, 5);
		WriteProcessMemory(inj.process, ntOpenFile, originalBytes, 0, NULL);
	}
	else
	{
		cout << "Unable to backup :(\n";
		Sleep(2000);
		exit(-1);
	}
}

int main()
{
	SetConsoleTitle("EZinjector - modified by Yatagarasu");
	
	cout << "EZinjector Reborn v.2\n\nVisit ezcheats.ru / ezcheats.com\n Owner: Yatagarasu / Xvorost\n\n" << endl;

	cout << "Enter process name:" << endl;

	cin >> process_name;

	DWORD pid = ProcessId(process_name);

	if (pid == NULL)
	{
		cout << "Cannot find : " << process_name << endl;
		Sleep(4000);
		exit(-1);
	}

	GetWindowThreadProcessId(inj.hwndproc, &pid);
	inj.process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	cout << "Enter dll name:" << endl;

	scanf_s("%s", namedll);
	

	if (DoesFileExist(namedll)) {
		bypass();

			if (inj.inject(pid, namedll)) {
				cout << "Injection successful! You can close this window.\n\n" << endl;
					Backup();
					Sleep(5000);
					exit(0);
			}
			else
			{
				cout << "Injection failed, try run as Administrator.\n\n" << endl;
					Backup();
					system("pause");
			}

	}
	else
	{
		cout << "Cannot find : " << namedll <<"\n";
		system("pause");
	}

	return 0;
}
