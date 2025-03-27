#pragma once
#include <fstream>
#include <map>
#include <memory>
#include "log/sink.h"
#include "log_file.h"

namespace enigma::log {
	class console_sink : public sink {
	   public:
		console_sink(console_sink_config&& config) : config_(std::move(config)) {}
		~console_sink() {}

		bool init() override;
		void uninit() override;
		bool log(const log_level lvl, const message& msg) override;

	   private:
		console_sink_config config_;
	};
}  // namespace enigma::log
