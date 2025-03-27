#pragma once
#include <fstream>
#include <memory>

namespace enigma::log {
	class log_file {
	   public:
		using ptr = std::unique_ptr<log_file>;
		log_file(const std::string& log_dir, const std::string& app_name,
				 const std::size_t index)
			: log_dir_(log_dir), app_name_(app_name), index_(index) {}
		~log_file() {}

		bool open(bool trunc);
		void close();
		void write(const std::string& data);
		bool rename_index(const std::size_t index);
		inline bool is_open() { return file_ && file_->is_open(); }
		inline std::size_t size() const { return cur_size_; }

	   private:
		std::unique_ptr<std::ofstream> file_;
		std::size_t cur_size_{0};
		std::size_t index_{0};
		std::string log_dir_, app_name_;
	};
}  // namespace enigma::log
