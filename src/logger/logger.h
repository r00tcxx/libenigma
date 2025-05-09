#pragma once
#include "log/log.h"
#include "singleton.h"
#include "sync_queue.h"
#include "types.h"

namespace enigma::log {
	class logger : public singleton<logger> {
		friend singleton<logger>;

	   public:
		~logger() = default;
		bool init(const log_level lvl, vector<sink::ptr>&& sinks);
		void uninit();
		void log_it(message&& msg);

	   private:
		logger() = default;

	   private:
		sync_queue<message> queue_;
		vector<sink::ptr> sinks_;
		jthread thread_;
		log_level lvl_;
	};

}  // namespace enigma::log
