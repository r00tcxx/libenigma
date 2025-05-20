#include "log_file.h"
#include <filesystem>
#include "format.h"

namespace ema::log {
	bool log_file::open(bool trunc) {
		if (_file && _file->is_open()) return true;
		_file = make_unique<std::ofstream>(
			!_index ? format("{}/{}.log", _log_dir, _app_name) : format("{}/{}.{}.log", _log_dir, _app_name, _index),
			std::ios::out | std::ios::binary | (trunc ? std::ios::trunc : std::ios::app));
		_file.get()->seekp(0, std::ios::end);
		_cur_size = _file.get()->tellp();
		return true;
	}

	void log_file::close() {
		if (!_file || !_file->is_open()) return;
		_file->flush();
		_file->close();
	}

	void log_file::write(const string& data) {
		if (!_file || !_file->is_open()) return;
		*_file << data << std::endl;
		_cur_size += data.length();
	}

	bool log_file::rename_index(const std::size_t index) {
		string new_path =
			!index ? format("{}/{}.log", _log_dir, _app_name) : format("{}/{}.{}.log", _log_dir, _app_name, index);
		string old_path =
			!_index ? format("{}/{}.log", _log_dir, _app_name) : format("{}/{}.{}.log", _log_dir, _app_name, _index);
		std::error_code ec;
		std::filesystem::rename(old_path.to_stdstring(), new_path.to_stdstring(), ec);
		if (ec) return false;
		_index = index;
		return true;
	}

}  // namespace ema::log
