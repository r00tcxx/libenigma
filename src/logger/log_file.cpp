#include "log_file.h"
#include <filesystem>
#include "fmt/format.h"

namespace enigma::log {
	bool log_file::open(bool trunc) {
		if (file_ && file_->is_open()) return true;
		file_ = std::make_unique<std::ofstream>(
			!index_ ? fmt::format("{}/{}.log", log_dir_, app_name_)
					: fmt::format("{}/{}.{}.log", log_dir_, app_name_, index_),
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

	void log_file::write(const std::string& data) {
		if (!file_ || !file_->is_open()) return;
		*file_ << data << std::endl;
		cur_size_ += data.length();
	}

	bool log_file::rename_index(const std::size_t index) {
		std::string new_path = !index ? fmt::format("{}/{}.log", log_dir_, app_name_)
									  : fmt::format("{}/{}.{}.log", log_dir_, app_name_, index);
		std::string old_path = !index_ ? fmt::format("{}/{}.log", log_dir_, app_name_)
									   : fmt::format("{}/{}.{}.log", log_dir_, app_name_, index_);
		std::error_code ec;
		std::filesystem::rename(old_path, new_path, ec);
		if (ec) return false;
		index_ = index;
		return true;
	}

}  // namespace enigma::log
