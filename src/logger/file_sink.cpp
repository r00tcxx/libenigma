#include "file_sink.h"
#include <filesystem>
#include <map>
#include <regex>
#include "format.h"

namespace ema::log {
	bool file_sink::init() {
		if (!std::filesystem::exists((std::string)_config.log_dir) &&
			std::filesystem::create_directories((std::string)_config.log_dir))
			return false;

		_file = std::make_unique<log_file>(_config.log_dir, _config.app_name, 0);
		_file->open(_config.mode == file_sink_config::mode::truncate);
		return _file->is_open();
	}

	void file_sink::uninit() {
		if (!_file || !_file->is_open()) return;
		_file->close();
	}

	bool file_sink::log(const log_level lvl, const message& msg) {
		if (!_file || !_file->is_open()) return false;
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
			case log_level::trace:
				lvl_str = "TRACE";
			case log_level::trace_error:
				lvl_str = "TRACE ERROR";
				break;
		}

		auto timestamp = format("{:%m-%d %H:%M:%S}", localtime(msg.timestamp()));
		string log;
		if (msg.module()) {
			log = format("{}.{:03} [{}] [{}] [{}]> {}", timestamp, msg.timestamp() % 1000, msg.thread(), lvl_str,
						 msg.module(), msg.content());
		}
		else {
			log = format("{}.{:03} [{}] [{}]> {}", timestamp, msg.timestamp() % 1000, msg.thread(), lvl_str,
						 msg.content());
		}
		_file->write(log);
		if (!_config.max_file_size || _file->size() < _config.max_file_size) return true;
		_file->close();
		return routing_logs();
	}

	std::map<std::size_t, log_file::ptr> file_sink::list_logs() {
		std::map<std::size_t, log_file::ptr> logs;
		const std::regex regex(format("^{}\\.(\\d+)\\.log$", _config.app_name));
		std::filesystem::directory_iterator end;
		for (std::filesystem::directory_iterator it((std::string)_config.log_dir); it != end; ++it) {
			if (!std::filesystem::is_regular_file(it->status())) continue;
			std::string filename = it->path().filename().string();
			std::smatch match;
			if (!std::regex_match(filename, match, regex)) continue;
			std::size_t index = std::stoll(match[1].str());
			logs[index]		  = std::make_unique<log_file>(_config.log_dir, _config.app_name, index);
		}
		return logs;
	}

	bool file_sink::routing_logs() {
		if (!_file) return false;
		_file->close();
		if (1 == _config.max_file_count) return _file->open(true);

		auto logs = list_logs();
		logs.emplace(0, std::move(_file));
		std::size_t index = logs.size();
		for (auto it = logs.rbegin(); it != logs.rend(); ++it) {
			log_file* log_file_ptr = it->second.get();
			if (!log_file_ptr->rename_index(index)) return false;
			index--;
		}
		if (logs.size() < _config.max_file_count) {
			_file = std::make_unique<log_file>(_config.log_dir, _config.app_name, 0);
			_file->open(true);
			return _file->is_open();
		}
		_file = std::move(logs.rbegin()->second);
		if (!_file->rename_index(0)) return false;
		return _file->open(true);
	}

}  // namespace ema::log
