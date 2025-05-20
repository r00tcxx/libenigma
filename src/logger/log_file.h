#pragma once
#include <fstream>
#include "types.h"

namespace ema::log {
	class log_file {
	   public:
		using ptr = unique_ptr<log_file>;
		log_file(const string& log_dir, const string& app_name, const u64 index)
			: _log_dir(log_dir), _app_name(app_name), _index(index) {}
		~log_file() {}

		bool open(bool trunc);
		void close();
		void write(const string& data);
		bool rename_index(const u64 index);
		inline bool is_open() { return _file && _file->is_open(); }
		inline u64 size() const { return _cur_size; }

	   private:
		unique_ptr<std::ofstream> _file;
		u64 _cur_size{0};
		u64 _index{0};
		string _log_dir, _app_name;
	};
}  // namespace ema::log
