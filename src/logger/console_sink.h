#pragma once
#include <fstream>
#include "log/sink.h"

namespace ema::log {
	class console_sink : public sink {
	   public:
		console_sink(console_sink_config&& config) : _config(std::move(config)) {}
		~console_sink() {}

		bool init() override;
		void uninit() override;
		bool log(const log_level lvl, const message& msg) override;

	   private:
		console_sink_config _config;
	};
}  // namespace ema::log
