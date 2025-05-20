#include "logger.h"
#include "console_sink.h"
#include "file_sink.h"

namespace ema::log {
	bool init_logger(const log_level lvl, std::vector<sink::ptr>&& sinks) {
		return logger::instance().init(lvl, std::move(sinks));
	}

	void uninit_logger() {
		logger::instance().uninit();
	}

	void log_it(message&& msg) {
		logger::instance().log_it(std::move(msg));
	}

	unique_ptr<sink> make_file_sink(file_sink_config&& config) {
		return std::make_unique<file_sink>(std::move(config));
	}

	unique_ptr<sink> make_console_sink(console_sink_config&& config) {
		return std::make_unique<console_sink>(std::move(config));
	}

	bool logger::init(const log_level lvl, std::vector<sink::ptr>&& sinks) {
		if (sinks.empty()) return false;
		_sinks = std::move(sinks);
		_lvl   = lvl;

		bool all_success{true};
		for (auto& sink : _sinks)
			all_success &= sink->init();
		if (!all_success) return false;

		_thread = std::jthread([this](std::stop_token token) {
			while (!token.stop_requested()) {
				auto msg = _queue.pop();
				if (!msg.has_value()) continue;
				for (auto& sink : _sinks)
					sink->log(_lvl, msg.value());
			}
		});
		return true;
	}

	void logger::uninit() {
		_queue.stop();
		_thread.request_stop();
		_thread.join();
		for (auto& sink : _sinks)
			sink->uninit();
	}

	void logger::log_it(message&& msg) {
		_queue.push(std::move(msg));
	}

}  // namespace ema::log
