#ifdef _WIN32
#include "native/process.h"
#include "win32.h"
#include <tlhelp32.h>

namespace enigma::native::process {
	string get_proc_name() {
		wchar_t buffer[MAX_PATH]{0};
		GetModuleFileName(NULL, buffer, MAX_PATH);
		string name = string::from_wstring(buffer);
		return name.substr(name.find_last_of("\\") + 1);
	}

	string get_proc_path() {
		wchar_t buffer[MAX_PATH]{0};
		GetModuleFileName(NULL, buffer, MAX_PATH);
		string path = string::from_wstring(buffer);
		path.replace("\\", "/");
		return path;
	}

	string get_proc_dir() {
		auto path = get_proc_path();
		return path.substr(0, path.find_last_of("/"));
	}

	u32 get_proc_by_name(const string& name) {
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);
        std::wstring wname = name.to_wstring();
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (Process32First(snapshot, &entry)) {
			do {
				if (std::wstring(entry.szExeFile) == wname) {
					CloseHandle(snapshot);
					return (u32)entry.th32ProcessID;
				}
			} while (Process32Next(snapshot, &entry));
		}
		CloseHandle(snapshot);
		return 0;
	}

    u32 get_proc_id() {
		return (u32)GetCurrentProcessId();        
    }

	u64 get_current_thread_id() {
		return (u64)GetCurrentThreadId();
	}

}  // namespace enigma::native::process
#endif