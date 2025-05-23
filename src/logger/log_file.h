#pragma once
#include <fstream>
#include <memory>

namespace ema::log {
	class LogFile {
	   public:
		using Ptr = std::unique_ptr<LogFile>;
		LogFile(const std::string& log_dir, const std::string& app_name, const std::size_t index)
			: _log_dir(log_dir), _app_name(app_name), _index(index) {
		}

		~LogFile() {
		}

		bool Open(bool trunc);
		void Close();
		void Write(const std::string& data);
		bool RenameIndex(const std::size_t index);

		inline bool IsOpen() {
			return _file && _file->is_open();
		}

		inline std::size_t Size() const {
			return _cur_size;
		}

	   private:
		std::unique_ptr<std::ofstream> _file;
		std::size_t _cur_size{0};
		std::size_t _index{0};
		std::string _log_dir, _app_name;
	};
}  // namespace ema::log
