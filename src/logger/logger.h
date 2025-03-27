#pragma once
#include <fstream>
#include <thread>
#include <vector>

#include "log/log.h"
#include "singleton.h"
#include "sync_queue.h"

namespace enigma::log {
	class logger : public singleton<logger> {
		friend singleton<logger>;

	   public:
		~logger() = default;
		bool init(const log_level lvl, std::vector<sink::ptr>&& sinks, formatter&& formatter);
		void uninit();
		void log_it(message&& msg);

	   private:
		logger() = default;

	   private:
		formatter formatter_;
		sync_queue<message> queue_;
		std::vector<sink::ptr> sinks_;
		std::jthread thread_;
		log_level lvl_;
	};

}  // namespace enigma::log
