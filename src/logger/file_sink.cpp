#include "file_sink.h"
#include <filesystem>
#include <map>
#include <regex>

#include "fmt/chrono.h"
#include "fmt/format.h"

namespace enigma::log {
	bool file_sink::init() {
		if (!std::filesystem::exists((std::string)config_.log_dir) &&
			std::filesystem::create_directories((std::string)config_.log_dir))
			return false;

		file_ = std::make_unique<log_file>(config_.log_dir, config_.app_name, 0);
		file_->open(config_.mode == file_sink_config::mode::truncate);
		return file_->is_open();
	}

	void file_sink::uninit() {
		if (!file_ || !file_->is_open()) return;
		file_->close();
	}

	bool file_sink::log(const log_level lvl, const message& msg) {
		if (!file_ || !file_->is_open()) return false;
		if (static_cast<int>(msg.level()) < static_cast<int>(lvl)) return true;

		std::string lvl_str;
		switch (msg.level()) {
			case log_level::debug:
				lvl_str = "DEBUG";
				break;
			case log_level::info:
				lvl_str = "INFO";
				break;
			case log_level::warn:
				lvl_str = "WARN";
				break;
			case log_level::error:
				lvl_str = "ERROR";
				break;
			case log_level::fatal:
				lvl_str = "FATAL";
				break;
		}
		auto timestamp = fmt::format("{:%m-%d %H:%M:%S}", fmt::localtime(msg.timestamp()));
		auto log	   = fmt::format("{}.{:03} [{}] [{}]> {}", timestamp, msg.timestamp() % 1000, msg.thread(), lvl_str,
									 msg.content());
		file_->write(log);
		if (!config_.max_file_size || file_->size() < config_.max_file_size) return true;
		file_->close();
		return routing_logs();
	}

	std::map<std::size_t, log_file::ptr> file_sink::list_logs() {
		std::map<std::size_t, log_file::ptr> logs;
		const std::regex regex(fmt::format("^{}\\.(\\d+)\\.log$", config_.app_name));
		std::filesystem::directory_iterator end;
		for (std::filesystem::directory_iterator it((std::string)config_.log_dir); it != end; ++it) {
			if (!std::filesystem::is_regular_file(it->status())) continue;
			std::string filename = it->path().filename().string();
			std::smatch match;
			if (!std::regex_match(filename, match, regex)) continue;
			std::size_t index = std::stoll(match[1].str());
			logs[index]		  = std::make_unique<log_file>(config_.log_dir, config_.app_name, index);
		}
		return logs;
	}

	bool file_sink::routing_logs() {
		if (!file_) return false;
		file_->close();
		if (1 == config_.max_file_count) return file_->open(true);

		auto logs = list_logs();
		logs.emplace(0, std::move(file_));
		std::size_t index = logs.size();
		for (auto it = logs.rbegin(); it != logs.rend(); ++it) {
			log_file* log_file_ptr = it->second.get();
			if (!log_file_ptr->rename_index(index)) return false;
			index--;
		}
		if (logs.size() < config_.max_file_count) {
			file_ = std::make_unique<log_file>(config_.log_dir, config_.app_name, 0);
			file_->open(true);
			return file_->is_open();
		}
		file_ = std::move(logs.rbegin()->second);
		if (!file_->rename_index(0)) return false;
		return file_->open(true);
	}

}  // namespace enigma::log
