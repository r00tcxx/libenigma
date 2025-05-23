#include "file_sink.h"
#include <filesystem>
#include <map>
#include <regex>
#include "format.h"

namespace ema::log {
	bool FileSink::Init() {
		if (!std::filesystem::exists((std::string)_config.log_dir) &&
			std::filesystem::create_directories((std::string)_config.log_dir))
			return false;

		_file = std::make_unique<LogFile>(_config.log_dir, _config.app_name, 0);
		_file->Open(_config.mode == FileSinkConfig::Mode::Truncate);
		return _file->IsOpen();
	}

	void FileSink::Uninit() {
		if (!_file || !_file->IsOpen()) return;
		_file->Close();
	}

	bool FileSink::Log(const LogLevel lvl, const Message& msg) {
		if (!_file || !_file->IsOpen()) return false;
		if (static_cast<int>(msg.Level()) < static_cast<int>(lvl)) return true;

		std::string lvl_str;
		switch (msg.Level()) {
			case LogLevel::Debug:
				lvl_str = "DEBUG";
				break;
			case LogLevel::Info:
				lvl_str = "INFO";
				break;
			case LogLevel::Warn:
				lvl_str = "WARN";
				break;
			case LogLevel::Error:
				lvl_str = "ERROR";
				break;
			case LogLevel::Fatal:
				lvl_str = "FATAL";
			case LogLevel::Trace:
				lvl_str = "TRACE";
			case LogLevel::TraceError:
				lvl_str = "TRACE ERROR";
				break;
		}

		auto timestamp = format("{:%m-%d %H:%M:%S}", localtime(msg.Timestamp()));
		std::string log;
		if (msg.Module()) {
			log = format("{}.{:03} [{}] [{}] [{}]> {}", timestamp, msg.Timestamp() % 1000, msg.Thread(), lvl_str,
						 msg.Module(), msg.Content());
		}
		else {
			log = format("{}.{:03} [{}] [{}]> {}", timestamp, msg.Timestamp() % 1000, msg.Thread(), lvl_str,
						 msg.Content());
		}
		_file->Write(log);
		if (!_config.max_file_size || _file->Size() < _config.max_file_size) return true;
		_file->Close();
		return RoutingLogs();
	}

	std::map<std::size_t, LogFile::Ptr> FileSink::ListLogs() {
		std::map<std::size_t, LogFile::Ptr> logs;
		const std::regex regex(format("^{}\\.(\\d+)\\.log$", _config.app_name));
		std::filesystem::directory_iterator end;
		for (std::filesystem::directory_iterator it((std::string)_config.log_dir); it != end; ++it) {
			if (!std::filesystem::is_regular_file(it->status())) continue;
			std::string filename = it->path().filename().string();
			std::smatch match;
			if (!std::regex_match(filename, match, regex)) continue;
			std::size_t index = std::stoll(match[1].str());
			logs[index]		  = std::make_unique<LogFile>(_config.log_dir, _config.app_name, index);
		}
		return logs;
	}

	bool FileSink::RoutingLogs() {
		if (!_file) return false;
		_file->Close();
		if (1 == _config.max_file_count) return _file->Open(true);

		auto logs = ListLogs();
		logs.emplace(0, std::move(_file));
		std::size_t index = logs.size();
		for (auto it = logs.rbegin(); it != logs.rend(); ++it) {
			LogFile* log_file_ptr = it->second.get();
			if (!log_file_ptr->RenameIndex(index)) return false;
			index--;
		}
		if (logs.size() < _config.max_file_count) {
			_file = std::make_unique<LogFile>(_config.log_dir, _config.app_name, 0);
			_file->Open(true);
			return _file->IsOpen();
		}
		_file = std::move(logs.rbegin()->second);
		if (!_file->RenameIndex(0)) return false;
		return _file->Open(true);
	}

}  // namespace ema::log
