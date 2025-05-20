#include "console_sink.h"
#include <iostream>
#include "format.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
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

namespace ema::log {
	bool console_sink::init() {
#ifdef _WIN32
		std_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		return true;
#endif
	}

	void console_sink::uninit() {
#ifdef _WIN32
#endif
	}

	bool console_sink::log(const log_level lvl, const message& msg) {
		if (static_cast<int>(msg.level()) < static_cast<int>(lvl)) return true;

		auto timestamp = format("{:%m-%d %H:%M:%S}", localtime(msg.timestamp()));
		auto prefix	   = format("{}.{:03} [{}]", timestamp, msg.timestamp() % 1000, msg.thread());
#ifdef _WIN32
		std::cout << prefix << " [";
		switch (msg.level()) {
			case log_level::debug:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::green);
				std::cout << "DEBUG";
				break;
			case log_level::info:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::blue);
				std::cout << "INFO";
				break;
			case log_level::warn:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::yellow);
				std::cout << "WARN";
				break;
			case log_level::error:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::red);
				std::cout << "ERROR";
				break;
			case log_level::fatal:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::red);
				std::cout << "FATAL";
				break;
			case log_level::trace:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::magenta);
				std::cout << "TRACE";
				break;
			case log_level::trace_error:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::red);
				std::cout << "TRACE ERROR";
				break;
		}
		if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::white);
		std::cout << "] ";
		if (msg.module()) std::cout << "[" << msg.module() << "]";
		std::cout << "> " << msg.content() << std::endl;
#endif
		return true;
	}

}  // namespace ema::log
