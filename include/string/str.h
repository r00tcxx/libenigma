/*
 * Created on Mon Mar 24 2025
 *
 * The MIT License (MIT)
 * Copyright (c) 2025 enigma
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#pragma once
#include <string>
#include <vector>

namespace enigma {
	class string : public std::string {
	   public:
		using std::string::string;
		using std::string::operator=;
		using std::string::operator+=;
		using std::string::operator[];
		string(const std::string& other) noexcept;
		string(std::string&& other) noexcept;
		string(const string& other) noexcept;
		string(string&& other) noexcept;
		string& operator=(const std::string&) noexcept;
		string& operator=(std::string&&) noexcept;
		string& operator=(const string&) noexcept;
		string& operator=(string&&) noexcept;
		string& operator=(const char* str) noexcept;
		string& operator=(char* str) noexcept;
		inline operator std::string() const noexcept { return *this; }

	   public:
		string& to_lower();
		string& to_upper();
		std::wstring to_wstring() const;
		string& trim();
		string& replace(const string& from, const string& to);
		std::vector<string> split(const string& delimiter, bool keep_empty = false);

	   public:
		static string from_wstring(const std::wstring& wstr);
		static string from_wstring(const wchar_t* wstr, std::size_t len);
	};
}  // namespace enigma
