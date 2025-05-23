#pragma once
#include <fstream>
#include "log/sink.h"

namespace ema::log {
	class ConsoleSink : public Sink {
	   public:
		ConsoleSink(ConsoleSinkConfig&& config) : _config(std::move(config)) {
		}
		~ConsoleSink() {
		}

		bool Init() override;
		void Uninit() override;
		bool Log(const LogLevel lvl, const Message& msg) override;

	   private:
		ConsoleSinkConfig _config;
	};
}  // namespace ema::log
