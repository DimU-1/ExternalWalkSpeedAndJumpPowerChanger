#pragma once

#include <Windows.h>
#include <string>
#include <iostream>
#include <TlHelp32.h>

HANDLE handle = nullptr;

DWORD getPID(std::string windowName)
{
	DWORD pid;
	GetWindowThreadProcessId(FindWindowA(nullptr, windowName.c_str()), &pid);
	return pid;
}

uintptr_t getModuleBaseAddress(const char* moduleName)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, getPID("Roblox"));

	MODULEENTRY32 currentModule{};
	currentModule.dwSize = sizeof(currentModule);

	if (Module32First(hSnap, &currentModule))
	{
		do
		{
			if (strcmp(currentModule.szModule, moduleName) == 0)
			{
				return (uintptr_t)currentModule.modBaseAddr;
			}
		} while (Module32Next(hSnap, &currentModule));
	}

	std::cout << "failed to find module" << std::endl;
	return 0;
}

void openHandle()
{
	handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, getPID("Roblox"));
}

template<typename T>
T read(uintptr_t address)
{
	if (!handle)
	{
		std::cout << "No handle is opened" << std::endl;
	}

	T buffer;
	ReadProcessMemory(handle, (LPCVOID)address, &buffer, sizeof(buffer), nullptr);

	return buffer;
}

template<typename T>
void write(uintptr_t address, T value)
{
	if (!handle)
	{
		std::cout << "No handle is opened" << std::endl;
	}

	WriteProcessMemory(handle, (LPVOID)address, &value, sizeof(value), nullptr);
}
