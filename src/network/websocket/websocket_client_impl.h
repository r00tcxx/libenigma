#pragma once
#include "network/websocket_client.h"

namespace enigma::network {
	class WebSocketClient::impl {
	   public:
		impl()	= default;
		~impl() = default;
		result connect(const string& ip, const u16 port);
		result disconnect();
		result send(packet&& package);
		result receive(receive_callback&& callback);

	   private:
		   
	};
}
