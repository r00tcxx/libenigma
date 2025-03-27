#include "console_sink.h"
#include <iostream>
#include "fmt/chrono.h"
#include "fmt/format.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
static FILE* stream{nullptr};
static HANDLE std_handle;
enum class color : WORD {
	black	= 0,
	red		= FOREGROUND_RED,
	green	= FOREGROUND_GREEN,
	blue	= FOREGROUND_BLUE,
	yellow	= FOREGROUND_RED | FOREGROUND_GREEN,
	cyan	= FOREGROUND_GREEN | FOREGROUND_BLUE,
	magenta = FOREGROUND_RED | FOREGROUND_BLUE,
	white	= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	bright	= FOREGROUND_INTENSITY,
};
#endif

namespace enigma::log {
	bool console_sink::init() {
#ifdef _WIN32
		AllocConsole();
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONOUT$", "w", stderr);
		freopen_s(&stream, "CONIN$", "r", stdin);
		std_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		return true;
#endif
	}

	void console_sink::uninit() {
#ifdef _WIN32
		if (stream) fclose(stream);
		FreeConsole();
#endif
	}

	bool console_sink::log(const log_level lvl, const message& msg) {
		if (static_cast<int>(msg.level()) < static_cast<int>(lvl)) return true;

		auto timestamp = fmt::format("{:%m-%d %H:%M:%S}", fmt::localtime(msg.timestamp()));
		auto prefix	   = fmt::format("{}.{:03} [{}]", timestamp, msg.timestamp() % 1000, msg.thread());
#ifdef _WIN32
		std::cout << prefix << " [";
		switch (msg.level()) {
			case log_level::debug:
				if (config_.color) SetConsoleTextAttribute(std_handle, (WORD)color::green);
				std::cout << "DEBUG";
				break;
			case log_level::info:
				if (config_.color) SetConsoleTextAttribute(std_handle, (WORD)color::blue);
				std::cout << "INFO";
				break;
			case log_level::warn:
				if (config_.color) SetConsoleTextAttribute(std_handle, (WORD)color::yellow);
				std::cout << "WARN";
				break;
			case log_level::error:
				if (config_.color) SetConsoleTextAttribute(std_handle, (WORD)color::red);
				std::cout << "ERROR";
				break;
			case log_level::fatal:
				if (config_.color) SetConsoleTextAttribute(std_handle, (WORD)color::red);
				std::cout << "FATAL";
				break;
		}
		if (config_.color) SetConsoleTextAttribute(std_handle, (WORD)color::white);
		std::cout << "]> " << msg.content() << std::endl;
#endif
		return true;
	}

}  // namespace enigma::log
