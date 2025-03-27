#include "console_sink.h"
#include <iostream>
#include "default_formatter.h"

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

		std::string prefix, log;
		if (config_.fmt) prefix = config_.fmt(msg.level(), msg.timestamp(), msg.thread(), msg.file(), msg.line());
		else prefix = default_formatter()(msg.level(), msg.timestamp(), msg.thread(), msg.file(), msg.line());
		log = fmt::format("{} {}", prefix, msg.content());
		if (config_.color) {
#ifdef _WIN32
			switch (msg.level()) {
				case log_level::debug:
					SetConsoleTextAttribute(std_handle, (WORD)color::green);
					std::cout << log << std::endl;
					break;
				case log_level::info:
					SetConsoleTextAttribute(std_handle, (WORD)color::white);
					std::cout << log << std::endl;
					break;
				case log_level::warn:
					SetConsoleTextAttribute(std_handle, (WORD)color::yellow);
					std::cout << log << std::endl;
					break;
				case log_level::error:
					SetConsoleTextAttribute(std_handle, (WORD)color::red);
					std::cout << log << std::endl;
					break;
				case log_level::fatal:
					SetConsoleTextAttribute(std_handle, (WORD)color::red);
					std::cout << log << std::endl;
					break;
			}
#endif
		}
		else {
			std::cout << log << std::endl;
		}
		return true;
	}

}  // namespace enigma::log
