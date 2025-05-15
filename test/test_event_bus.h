#pragma once
#include "event_bus/event_bus.h"
#include "gtest/gtest.h"

using namespace ema;

// Test message types
struct TestMessage1 {
	int value;
	std::string text;
};

struct TestMessage2 {
	double value;
};

class TestEventBus : public testing::Test {
   public:
   protected:
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	void SetUp() override {
		bus		 = std::make_shared<event_bus>();
		handler1 = std::make_shared<event_handler>();
		handler2 = std::make_shared<event_handler>();
	}

	void TearDown() override {
		handler1->stop();
		handler2->stop();
	}

	void wait_for_events() { std::this_thread::sleep_for(std::chrono::seconds(1)); }

	std::shared_ptr<event_bus> bus;
	event_handler::ptr handler1;
	event_handler::ptr handler2;
};

// Test subscribing handlers to the event bus
TEST_F(TestEventBus, Subscribe) {
	EXPECT_TRUE(bus->subscribe(handler1));
	EXPECT_TRUE(bus->subscribe(handler2));
	// Subscribe the same handler again should return false
	EXPECT_FALSE(bus->subscribe(handler1));

	// Test with null handler
	EXPECT_FALSE(bus->subscribe(nullptr));
}

// Test unsubscribing handlers from the event bus
TEST_F(TestEventBus, Unsubscribe) {
	EXPECT_TRUE(bus->subscribe(handler1));
	EXPECT_TRUE(bus->subscribe(handler2));

	bus->unsubscribe(handler1);
	// Should be able to subscribe the handler again after unsubscribing
	EXPECT_TRUE(bus->subscribe(handler1));

	// Unsubscribe with null handler should not crash
	bus->unsubscribe(nullptr);
}

// Test handler's start and stop functionality
TEST_F(TestEventBus, HandlerStartStop) {
	ASSERT_FALSE(handler1->is_start());
	handler1->start();
	EXPECT_TRUE(handler1->is_start());

	// Starting an already started handler should not change its state
	handler1->start();
	EXPECT_TRUE(handler1->is_start());

	handler1->stop();
	EXPECT_FALSE(handler1->is_start());

	// Stopping an already stopped handler should not change its state
	handler1->stop();
	EXPECT_FALSE(handler1->is_start());
}

// Test publishing messages with no subscribers
TEST_F(TestEventBus, PublishWithNoSubscribers) {
	TestMessage1 msg{42, "test"};
	EXPECT_FALSE(bus->publish(msg));
}

// Test basic message publishing and handling
TEST_F(TestEventBus, BasicPublishAndHandle) {
	bool received = false;
	TestMessage1 original{42, "test"};

	handler1->start();
	handler1->on<TestMessage1>([&received, &original](const TestMessage1& msg) {
		received = true;
		EXPECT_EQ(msg.value, original.value);
		EXPECT_EQ(msg.text, original.text);
	});

	EXPECT_TRUE(bus->subscribe(handler1));
	EXPECT_TRUE(bus->publish(original));

	// Give some time for the message to be processed
	wait_for_events();
	EXPECT_TRUE(received);
}

// Test multiple handlers receiving the same message
TEST_F(TestEventBus, MultipleHandlers) {
	int handler1_count = 0;
	int handler2_count = 0;
	TestMessage1 msg{42, "test"};

	handler1->start();
	handler2->start();

	handler1->on<TestMessage1>([&handler1_count](const TestMessage1&) { handler1_count++; });

	handler2->on<TestMessage1>([&handler2_count](const TestMessage1&) { handler2_count++; });

	EXPECT_TRUE(bus->subscribe(handler1));
	EXPECT_TRUE(bus->subscribe(handler2));
	EXPECT_TRUE(bus->publish(msg));

	// Give some time for the message to be processed
	wait_for_events();
	EXPECT_EQ(handler1_count, 1);
	EXPECT_EQ(handler2_count, 1);
}

// Test different message types
TEST_F(TestEventBus, DifferentMessageTypes) {
	bool received1 = false;
	bool received2 = false;
	TestMessage1 msg1{42, "test"};
	TestMessage2 msg2{3.14};

	handler1->start();

	handler1->on<TestMessage1>([&received1](const TestMessage1&) { received1 = true; });

	handler1->on<TestMessage2>([&received2](const TestMessage2&) { received2 = true; });

	EXPECT_TRUE(bus->subscribe(handler1));
	EXPECT_TRUE(bus->publish(msg1));
	EXPECT_TRUE(bus->publish(msg2));

	// Give some time for the messages to be processed
	wait_for_events();
	EXPECT_TRUE(received1);
	EXPECT_TRUE(received2);
}

// Test acknowledgment callback
TEST_F(TestEventBus, AcknowledgmentCallback) {
	bool message_handled = false;
	bool ack_called		 = false;
	TestMessage1 msg{42, "test"};

	handler1->start();

	handler1->on<TestMessage1>([&message_handled](const TestMessage1&) { message_handled = true; });

	EXPECT_TRUE(bus->subscribe(handler1));
	EXPECT_TRUE(bus->publish(msg, [&ack_called]() { ack_called = true; }));

	// Give some time for the message to be processed
	wait_for_events();
	EXPECT_TRUE(message_handled);
	EXPECT_TRUE(ack_called);
}

// Test canceling a specific message type subscription
TEST_F(TestEventBus, CancelSubscription) {
	int count1 = 0;
	int count2 = 0;
	TestMessage1 msg1{42, "test"};
	TestMessage2 msg2{3.14};

	handler1->start();

	handler1->on<TestMessage1>([&count1](const TestMessage1&) { count1++; });

	handler1->on<TestMessage2>([&count2](const TestMessage2&) { count2++; });

	EXPECT_TRUE(bus->subscribe(handler1));

	// First verify both messages are received
	EXPECT_TRUE(bus->publish(msg1));
	EXPECT_TRUE(bus->publish(msg2));
	wait_for_events();
	EXPECT_EQ(count1, 1);
	EXPECT_EQ(count2, 1);

	// Now cancel TestMessage1 subscription
	handler1->cancel<TestMessage1>();

	// Publish again
	EXPECT_FALSE(bus->publish(msg1));  // Should return false since no handlers for this type
	EXPECT_TRUE(bus->publish(msg2));
	wait_for_events();

	// count1 should still be 1, count2 should be 2
	EXPECT_EQ(count1, 1);
	EXPECT_EQ(count2, 2);
}
