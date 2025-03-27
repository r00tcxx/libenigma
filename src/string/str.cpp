#include <algorithm>
#include "string/str.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace enigma {
	string::string(const std::string& other) noexcept : std::string(other) {}
	string::string(std::string&& other) noexcept : std::string(std::move(other)) {}
	string::string(const string& other) noexcept : std::string(other) {}
	string::string(string&& other) noexcept : std::string(std::move(other)) {}

	string& string::operator=(const std::string& other) noexcept {
		assign(other);
		return *this;
	}

	string& string::operator=(std::string&& other) noexcept {
		assign(std::move(other));
		return *this;
	}

	string& string::operator=(const string& other) noexcept {
		assign(other);
		return *this;
	}

	string& string::operator=(string&& other) noexcept {
		assign(std::move(other));
		return *this;
	}

	string& string::operator=(const char* other) noexcept {
		assign(other);
		return *this;
	}

	string& string::operator = (char* str) noexcept {
		assign(str);
		return *this;
	}

	string& string::to_lower() {
		std::transform(begin(), end(), begin(), ::tolower);
		return *this;
	}

	string& string::to_upper() {
		std::transform(begin(), end(), begin(), ::toupper);
		return *this;
	}

	std::wstring string::to_wstring() const {
		if (empty()) return {};
		auto size_need = MultiByteToWideChar(CP_UTF8, 0, c_str(), static_cast<int>(size()), nullptr, 0);
		if (!size_need) return {};
		std::wstring wstr(size_need, 0);
		MultiByteToWideChar(CP_UTF8, 0, c_str(), static_cast<int>(size()), wstr.data(), size_need);
		return wstr;
	}

	string& string::trim() {
		auto is_space = [](unsigned char c) {
			return std::isspace(c) != 0;
		};
		auto start_it = std::find_if_not(begin(), end(), is_space);
		auto end_it	  = std::find_if_not(std::reverse_iterator<iterator>(this->end()),
										 std::reverse_iterator<iterator>(this->begin()), ::isspace)
						  .base();
		if (start_it >= end_it) clear();
		if (end_it > start_it) assign(start_it, end_it);
		return *this;
	}

	string& string::replace(const string& from, const string& to) {
		std::size_t start_pos = 0;
		while ((start_pos = find(from, start_pos)) != string::npos) {
			std::string::replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
		return *this;
	}

	std::vector<string> string::split(const string& delimiter, bool keep_empty) {
		std::vector<string> tokens;
		if (empty()) return {};
		std::size_t prev = 0, pos = 0;
		do {
			pos = find(delimiter, prev);
			if (pos == string::npos) pos = length();
			string token = substr(prev, pos - prev);
			if (!token.empty() || keep_empty) tokens.push_back(token);
			prev = pos + delimiter.length();
		} while (pos < length() && prev < length());
		return tokens;
	}

	string string::from_wstring(const std::wstring& wstr) {
		return from_wstring(wstr.c_str(), wstr.size());
	}

	string string::from_wstring(const wchar_t* wstr, std::size_t len) {
		if (!len || !wstr) return {};
		auto size_need = WideCharToMultiByte(CP_UTF8, 0, wstr, static_cast<int>(len), nullptr, 0, nullptr, nullptr);
		if (size_need == 0) return {};
		std::string str(size_need, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr, static_cast<int>(len), str.data(), static_cast<int>(len), nullptr,
							nullptr);
		return str;
	}
}  // namespace enigma
