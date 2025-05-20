#pragma once
#include <fstream>
#include <map>
#include "log/sink.h"
#include "log_file.h"

namespace ema::log {
	class file_sink : public sink {
	   public:
		file_sink(file_sink_config&& config) : _config(std::move(config)) {}
		~file_sink() {}
		bool init() override;
		void uninit() override;
		bool log(const log_level lvl, const message& msg) override;

	   private:
		std::map<std::size_t, log_file::ptr> list_logs();
		bool routing_logs();

	   private:
		file_sink_config _config;
		log_file::ptr _file;
	};
}  // namespace ema::log
