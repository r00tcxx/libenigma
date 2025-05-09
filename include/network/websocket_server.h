#pragma once
#include "class.h"
#include "packet.h"
#include "types.h"

namespace enigma::network {
	using receive_callback = func<void(packet&&)>;

	class Session {
	};

	class WebSocketServer : no_copyable {
	   public:
		WebSocketServer();
		~WebSocketServer();
		result startup(const string& ip, const u16 port);
		result send(packet&& package);
		result receive(receive_callback&& callback);
	};
}  // namespace enigma::network
