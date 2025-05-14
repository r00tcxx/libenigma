#include "log_file.h"
#include <filesystem>
#include "format.h"

namespace ema::log {
	bool log_file::open(bool trunc) {
		if (file_ && file_->is_open()) return true;
		file_ = make_unique<std::ofstream>(
			!index_ ? format("{}/{}.log", log_dir_, app_name_) : format("{}/{}.{}.log", log_dir_, app_name_, index_),
			std::ios::out | std::ios::binary | (trunc ? std::ios::trunc : std::ios::app));
		file_.get()->seekp(0, std::ios::end);
		cur_size_ = file_.get()->tellp();
		return true;
	}

	void log_file::close() {
		if (!file_ || !file_->is_open()) return;
		file_->flush();
		file_->close();
	}

	void log_file::write(const string& data) {
		if (!file_ || !file_->is_open()) return;
		*file_ << data << std::endl;
		cur_size_ += data.length();
	}

	bool log_file::rename_index(const std::size_t index) {
		string new_path =
			!index ? format("{}/{}.log", log_dir_, app_name_) : format("{}/{}.{}.log", log_dir_, app_name_, index);
		string old_path =
			!index_ ? format("{}/{}.log", log_dir_, app_name_) : format("{}/{}.{}.log", log_dir_, app_name_, index_);
		std::error_code ec;
		std::filesystem::rename(old_path.to_stdstring(), new_path.to_stdstring(), ec);
		if (ec) return false;
		index_ = index;
		return true;
	}

}  // namespace ema::log
