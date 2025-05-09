#pragma once
#include "class.h"
#include "packet.h"
#include "types.h"

namespace enigma::network {
	using websocket_client_receive_callback = func<void(packet&&)>;

	class websocket : no_copyable {
	   public:
		WebSocketClient();
		~WebSocketClient();
		result connect(const string& ip, const u16 port);
		result disconnect();
		result send(packet&& package);
		result receive(websocket_client_receive_callback&& callback);

	   private:
		class impl;
		unique_ptr<impl> impl_;
	};
}  // namespace enigma::network
