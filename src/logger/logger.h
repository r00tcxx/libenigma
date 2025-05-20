#pragma once
#include "log/log.h"
#include "singleton.h"
#include "sync_queue.h"
#include "types.h"

namespace ema::log {
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
		sync_queue<message> _queue;
		vector<sink::ptr> _sinks;
		jthread _thread;
		log_level _lvl;
	};

}  // namespace ema::log
