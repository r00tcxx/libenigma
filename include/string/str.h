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

namespace ema {
	class String : public std::string {
	   public:
		using std::string::string;
		using std::string::operator=;
		using std::string::operator+=;
		using std::string::operator[];
		String(const std::string& other) noexcept;
		String(std::string&& other) noexcept;
		String(const String& other) noexcept;
		String(String&& other) noexcept;
		String& operator=(const std::string&) noexcept;
		String& operator=(std::string&&) noexcept;
		String& operator=(const String&) noexcept;
		String& operator=(String&&) noexcept;
		String& operator=(const char* str) noexcept;
		String& operator=(char* str) noexcept;
		inline operator std::string() const noexcept {
			return *this;
		}

	   public:
		String& to_lower();
		String& to_upper();
		std::wstring to_wstring() const;
		std::string to_stdstring() const;
		String& trim();
		String& replace(const String& from, const String& to);
		std::vector<String> split(const String& delimiter, bool keep_empty = false);

	   public:
		static String from_wstring(const std::wstring& wstr);
		static String from_wstring(const wchar_t* wstr, std::size_t len);
	};

	using string_view = std::string_view;

}  // namespace ema
