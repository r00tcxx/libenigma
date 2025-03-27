#pragma once
#include <fstream>
#include <map>
#include <memory>
#include "log/sink.h"
#include "log_file.h"

namespace enigma::log {
	class file_sink : public sink {
	   public:
		file_sink(file_sink_config&& config) : config_(std::move(config)) {}
		~file_sink() {}
		bool init() override;
		void uninit() override;
		bool log(const log_level lvl, const message& msg) override;

	   private:
		std::map<std::size_t, log_file::ptr> list_logs();
		bool routing_logs();

	   private:
		file_sink_config config_;
		log_file::ptr file_;
	};
}  // namespace enigma::log
