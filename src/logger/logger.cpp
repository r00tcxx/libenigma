#include "logger.h"
#include "file_sink.h"
#include "console_sink.h"

namespace enigma::log {
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
		sinks_	   = std::move(sinks);
		lvl_	   = lvl;

		bool all_success{true};
		for (auto& sink : sinks_)
			all_success &= sink->init();
		if (!all_success) return false;

		thread_ = std::jthread([this](std::stop_token token) {
			while (!token.stop_requested()) {
				auto msg = queue_.pop();
				if (!msg.has_value()) continue;
				for (auto& sink : sinks_)
					sink->log(lvl_, msg.value());
			}
		});
		return true;
	}

	void logger::uninit() {
		queue_.stop();
		thread_.request_stop();
		thread_.join();
		for (auto& sink : sinks_)
			sink->uninit();
	}

	void logger::log_it(message&& msg) {
		queue_.push(std::move(msg));
	}

}  // namespace enigma::log
