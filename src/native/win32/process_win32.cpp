#ifdef _WIN32
#include "win32.h"

#include <tlhelp32.h>
#include "native/process.h"
#include "string/str.h"

namespace ema::native::process {
	std::string GetProcName() {
		wchar_t buffer[MAX_PATH]{0};
		GetModuleFileName(NULL, buffer, MAX_PATH);
		String name = String::from_wstring(buffer);
		return name.substr(name.find_last_of("\\") + 1);
	}

	std::string GetProcPath() {
		wchar_t buffer[MAX_PATH]{0};
		GetModuleFileName(NULL, buffer, MAX_PATH);
		String path = String::from_wstring(buffer);
		path.replace("\\", "/");
		return path;
	}

	std::string GetProcDir() {
		auto path = GetProcPath();
		return path.substr(0, path.find_last_of("/"));
	}

	unsigned int GetProcByName(const std::string& name) {
		PROCESSENTRY32 entry;
		entry.dwSize	   = sizeof(PROCESSENTRY32);
		std::wstring wname = String(name).to_wstring();
		HANDLE snapshot	   = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (Process32First(snapshot, &entry)) {
			do {
				if (std::wstring(entry.szExeFile) == wname) {
					CloseHandle(snapshot);
					return (unsigned int)entry.th32ProcessID;
				}
			} while (Process32Next(snapshot, &entry));
		}
		CloseHandle(snapshot);
		return 0;
	}

	unsigned int GetProcID() {
		return (unsigned int)GetCurrentProcessId();
	}

	std::size_t GetCurrentThreadID() {
		return (std::size_t)GetCurrentThreadId();
	}

}  // namespace ema::native::process
#endif