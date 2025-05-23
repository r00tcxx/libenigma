#pragma once
#include <fstream>
#include <map>
#include "log/sink.h"
#include "log_file.h"

namespace ema::log {
	class FileSink : public Sink {
	   public:
		FileSink(FileSinkConfig&& config) : _config(std::move(config)) {
		}
		~FileSink() {
		}
		bool Init() override;
		void Uninit() override;
		bool Log(const LogLevel lvl, const Message& msg) override;

	   private:
		std::map<std::size_t, LogFile::Ptr> ListLogs();
		bool RoutingLogs();

	   private:
		FileSinkConfig _config;
		LogFile::Ptr _file;
	};
}  // namespace ema::log
