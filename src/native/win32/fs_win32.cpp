#include "native/fs.h"
#include <filesystem>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <shlobj.h>

namespace enigma::native::fs {
	HANDLE get_user_session_token() {
		HANDLE token{INVALID_HANDLE_VALUE};
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY, &token))
			return token;
		HANDLE new_token;
		if (!DuplicateTokenEx(token, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &new_token)) {
			CloseHandle(token);
			token = INVALID_HANDLE_VALUE;
		}
		return token;
	}

	string get_standard_path(const GUID guid) {
		wchar_t* buffer{nullptr};
		auto token = get_user_session_token();
		if (S_OK == SHGetKnownFolderPath(guid, 0, token, &buffer)) {
			string path = string::from_wstring(buffer);
			path.replace("\\", "/");
			CoTaskMemFree(buffer);
			CloseHandle(token);
			return path;
		}
		return "";
	}

	string get_program_data_path() {
		return get_standard_path(FOLDERID_ProgramData);
	}

	string get_program_file_path() {
		return get_standard_path(FOLDERID_ProgramFilesX64);
	}

	string get_tmp_path() {
		std::wstring buffer(MAX_PATH, 0);
		GetTempPath(MAX_PATH, buffer.data());
		if (buffer.empty()) return "";
		auto u8 = string::from_wstring(buffer);
		u8.replace("\\", "/");
		return u8;
	}

	bool exists(const string& path) {
		if (path.empty()) return false;
		std::error_code ec;
		return std::filesystem::exists(path.to_stdstring(), ec);
	}

	bool create_directories(const string path) {
		if (path.empty()) return false;
		std::error_code ec;
		return std::filesystem::create_directories(path.to_stdstring(), ec);
	}
}
