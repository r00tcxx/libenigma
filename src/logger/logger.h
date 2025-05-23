#pragma once
#include "container/sync_queue.h"
#include "log/log.h"
#include "singleton.h"

namespace ema::log {
	class Logger : public Singleton<Logger> {
		friend Singleton<Logger>;

	   public:
		~Logger() = default;
		bool Init(const LogLevel lvl, std::vector<Sink::Ptr>&& sinks);
		void Uninit();
		void LogIt(Message&& msg);

	   private:
		Logger() = default;

	   private:
		SyncQueue<Message> _queue;
		std::vector<Sink::Ptr> _sinks;
		std::jthread _thread;
		LogLevel _lvl;
	};

}  // namespace ema::log
