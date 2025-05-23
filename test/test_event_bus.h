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
	static void SetUpTestSuite() {
	}
	static void TearDownTestSuite() {
	}

	void SetUp() override {
		bus		 = std::make_shared<EventBus>();
		handler1 = std::make_shared<EventHandler>();
		handler2 = std::make_shared<EventHandler>();
	}

	void TearDown() override {
		handler1->Stop();
		handler2->Stop();
	}

	void WaitForEvents() {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	std::shared_ptr<EventBus> bus;
	EventHandler::Ptr handler1;
	EventHandler::Ptr handler2;
};

// Test subscribing handlers to the event bus
TEST_F(TestEventBus, Subscribe) {
	EXPECT_TRUE(bus->Subscribe(handler1));
	EXPECT_TRUE(bus->Subscribe(handler2));
	// Subscribe the same handler again should return false
	EXPECT_FALSE(bus->Subscribe(handler1));

	// Test with null handler
	EXPECT_FALSE(bus->Subscribe(nullptr));
}

// Test unsubscribing handlers from the event bus
TEST_F(TestEventBus, Unsubscribe) {
	EXPECT_TRUE(bus->Subscribe(handler1));
	EXPECT_TRUE(bus->Subscribe(handler2));

	bus->Unsubscribe(handler1);
	// Should be able to subscribe the handler again after unsubscribing
	EXPECT_TRUE(bus->Subscribe(handler1));

	// Unsubscribe with null handler should not crash
	bus->Unsubscribe(nullptr);
}

// Test handler's start and stop functionality
TEST_F(TestEventBus, HandlerStartStop) {
	ASSERT_FALSE(handler1->IsStart());
	handler1->Start();
	EXPECT_TRUE(handler1->IsStart());

	// Starting an already started handler should not change its state
	handler1->Start();
	EXPECT_TRUE(handler1->IsStart());

	handler1->Stop();
	EXPECT_FALSE(handler1->IsStart());

	// Stopping an already stopped handler should not change its state
	handler1->Stop();
	EXPECT_FALSE(handler1->IsStart());
}

// Test publishing messages with no subscribers
TEST_F(TestEventBus, PublishWithNoSubscribers) {
	TestMessage1 msg{42, "test"};
	EXPECT_FALSE(bus->Publish(msg));
}

// Test basic message publishing and handling
TEST_F(TestEventBus, BasicPublishAndHandle) {
	bool received = false;
	TestMessage1 original{42, "test"};

	handler1->Start();
	handler1->On<TestMessage1>([&received, &original](const TestMessage1& msg) {
		received = true;
		EXPECT_EQ(msg.value, original.value);
		EXPECT_EQ(msg.text, original.text);
	});

	EXPECT_TRUE(bus->Subscribe(handler1));
	EXPECT_TRUE(bus->Publish(original));

	// Give some time for the message to be processed
	WaitForEvents();
	EXPECT_TRUE(received);
}

// Test multiple handlers receiving the same message
TEST_F(TestEventBus, MultipleHandlers) {
	int handler1Count = 0;
	int handler2Count = 0;
	TestMessage1 msg{42, "test"};

	handler1->Start();
	handler2->Start();

	handler1->On<TestMessage1>([&handler1Count](const TestMessage1&) { handler1Count++; });

	handler2->On<TestMessage1>([&handler2Count](const TestMessage1&) { handler2Count++; });

	EXPECT_TRUE(bus->Subscribe(handler1));
	EXPECT_TRUE(bus->Subscribe(handler2));
	EXPECT_TRUE(bus->Publish(msg));

	// Give some time for the message to be processed
	WaitForEvents();
	EXPECT_EQ(handler1Count, 1);
	EXPECT_EQ(handler2Count, 1);
}

// Test different message types
TEST_F(TestEventBus, DifferentMessageTypes) {
	bool received1 = false;
	bool received2 = false;
	TestMessage1 msg1{42, "test"};
	TestMessage2 msg2{3.14};

	handler1->Start();

	handler1->On<TestMessage1>([&received1](const TestMessage1&) { received1 = true; });

	handler1->On<TestMessage2>([&received2](const TestMessage2&) { received2 = true; });

	EXPECT_TRUE(bus->Subscribe(handler1));
	EXPECT_TRUE(bus->Publish(msg1));
	EXPECT_TRUE(bus->Publish(msg2));

	// Give some time for the messages to be processed
	WaitForEvents();
	EXPECT_TRUE(received1);
	EXPECT_TRUE(received2);
}

// Test acknowledgment callback
TEST_F(TestEventBus, AcknowledgmentCallback) {
	bool messageHandled = false;
	bool ackCalled = false;
	TestMessage1 msg{42, "test"};

	handler1->Start();

	handler1->On<TestMessage1>([&messageHandled](const TestMessage1&) { messageHandled = true; });

	EXPECT_TRUE(bus->Subscribe(handler1));
	EXPECT_TRUE(bus->Publish(msg, [&ackCalled]() { ackCalled = true; }));

	// Give some time for the message to be processed
	WaitForEvents();
	EXPECT_TRUE(messageHandled);
	EXPECT_TRUE(ackCalled);
}

// Test canceling a specific message type subscription
TEST_F(TestEventBus, CancelSubscription) {
	int count1 = 0;
	int count2 = 0;
	TestMessage1 msg1{42, "test"};
	TestMessage2 msg2{3.14};

	handler1->Start();

	handler1->On<TestMessage1>([&count1](const TestMessage1&) { count1++; });

	handler1->On<TestMessage2>([&count2](const TestMessage2&) { count2++; });

	EXPECT_TRUE(bus->Subscribe(handler1));

	// First verify both messages are received
	EXPECT_TRUE(bus->Publish(msg1));
	EXPECT_TRUE(bus->Publish(msg2));
	WaitForEvents();
	EXPECT_EQ(count1, 1);
	EXPECT_EQ(count2, 1);

	// Now cancel TestMessage1 subscription
	handler1->Cancel<TestMessage1>();

	// Publish again
	EXPECT_FALSE(bus->Publish(msg1));  // Should return false since no handlers for this type
	EXPECT_TRUE(bus->Publish(msg2));
	WaitForEvents();

	// count1 should still be 1, count2 should be 2
	EXPECT_EQ(count1, 1);
	EXPECT_EQ(count2, 2);
}
