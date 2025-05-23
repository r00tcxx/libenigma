#include "logger.h"
#include "console_sink.h"
#include "file_sink.h"

namespace ema::log {
	bool InitLogger(const LogLevel lvl, std::vector<Sink::Ptr>&& sinks) {
		return Logger::instance().Init(lvl, std::move(sinks));
	}

	void InitLogger() {
		Logger::instance().Uninit();
	}

	void LogIt(Message&& msg) {
		Logger::instance().LogIt(std::move(msg));
	}

	std::unique_ptr<Sink> MakeFileSink(FileSinkConfig&& config) {
		return std::make_unique<FileSink>(std::move(config));
	}

	std::unique_ptr<Sink> MakeConsoleSink(ConsoleSinkConfig&& config) {
		return std::make_unique<ConsoleSink>(std::move(config));
	}

	bool Logger::Init(const LogLevel lvl, std::vector<Sink::Ptr>&& sinks) {
		if (sinks.empty()) return false;
		_sinks = std::move(sinks);
		_lvl   = lvl;

		bool all_success{true};
		for (auto& sink : _sinks)
			all_success &= sink->Init();
		if (!all_success) return false;

		_thread = std::jthread([this](std::stop_token token) {
			while (!token.stop_requested()) {
				auto msg = _queue.Pop();
				if (!msg.has_value()) continue;
				for (auto& sink : _sinks)
					sink->Log(_lvl, msg.value());
			}
		});
		return true;
	}

	void Logger::Uninit() {
		_queue.Stop();
		_thread.request_stop();
		_thread.join();
		for (auto& sink : _sinks)
			sink->Uninit();
	}

	void Logger::LogIt(Message&& msg) {
		_queue.Push(std::move(msg));
	}

}  // namespace ema::log
