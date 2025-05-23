#include <filesystem>
#include "native/fs.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include "string/str.h"

namespace ema::native::fs {
	HANDLE GetUserSessionToken() {
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

	std::string GetStandardPath(const GUID guid) {
		wchar_t* buffer{nullptr};
		auto token = GetUserSessionToken();
		if (S_OK == SHGetKnownFolderPath(guid, 0, token, &buffer)) {
			String path = String::from_wstring(buffer);
			path.replace("\\", "/");
			CoTaskMemFree(buffer);
			CloseHandle(token);
			return path;
		}
		return "";
	}

	std::string GetProgramDataPath() {
		return GetStandardPath(FOLDERID_ProgramData);
	}

	std::string GetProgramFilePath() {
		return GetStandardPath(FOLDERID_ProgramFilesX64);
	}

	std::string GetTmpPath() {
		std::wstring buffer(MAX_PATH, 0);
		GetTempPath(MAX_PATH, buffer.data());
		if (buffer.empty()) return "";
		auto u8 = String::from_wstring(buffer);
		u8.replace("\\", "/");
		return u8;
	}

	bool Exists(const std::string& path) {
		if (path.empty()) return false;
		std::error_code ec;
		return std::filesystem::exists(path, ec);
	}

	bool CreateDirecties(const std::string path) {
		if (path.empty()) return false;
		std::error_code ec;
		return std::filesystem::create_directories(path, ec);
	}
}  // namespace ema::native::fs
