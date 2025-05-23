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
	bool ConsoleSink::Init() {
#ifdef _WIN32
		std_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		return true;
#endif
	}

	void ConsoleSink::Uninit() {
#ifdef _WIN32
#endif
	}

	bool ConsoleSink::Log(const LogLevel lvl, const Message& msg) {
		if (static_cast<int>(msg.Level()) < static_cast<int>(lvl)) return true;

		auto timestamp = format("{:%m-%d %H:%M:%S}", localtime(msg.Timestamp()));
		auto prefix	   = format("{}.{:03} [{}]", timestamp, msg.Timestamp() % 1000, msg.Thread());
#ifdef _WIN32
		std::cout << prefix << " [";
		switch (msg.Level()) {
			case LogLevel::Debug:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::green);
				std::cout << "DEBUG";
				break;
			case LogLevel::Info:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::blue);
				std::cout << "INFO";
				break;
			case LogLevel::Warn:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::yellow);
				std::cout << "WARN";
				break;
			case LogLevel::Error:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::red);
				std::cout << "ERROR";
				break;
			case LogLevel::Fatal:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::red);
				std::cout << "FATAL";
				break;
			case LogLevel::Trace:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::magenta);
				std::cout << "TRACE";
				break;
			case LogLevel::TraceError:
				if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::red);
				std::cout << "TRACE ERROR";
				break;
		}
		if (_config.color) SetConsoleTextAttribute(std_handle, (WORD)color::white);
		std::cout << "] ";
		if (msg.Module()) std::cout << "[" << msg.Module() << "]";
		std::cout << "> " << msg.Content() << std::endl;
#endif
		return true;
	}

}  // namespace ema::log
