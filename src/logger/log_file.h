#pragma once
#include <fstream>
#include "types.h"

namespace enigma::log {
	class log_file {
	   public:
		using ptr = unique_ptr<log_file>;
		log_file(const string& log_dir, const string& app_name, const u64 index)
			: log_dir_(log_dir), app_name_(app_name), index_(index) {}
		~log_file() {}

		bool open(bool trunc);
		void close();
		void write(const string& data);
		bool rename_index(const u64 index);
		inline bool is_open() { return file_ && file_->is_open(); }
		inline u64 size() const { return cur_size_; }

	   private:
		unique_ptr<std::ofstream> file_;
		u64 cur_size_{0};
		u64 index_{0};
		string log_dir_, app_name_;
	};
}  // namespace enigma::log
