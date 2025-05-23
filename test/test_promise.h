#pragma once
#include "gtest/gtest.h"
#include "thread/thread.h"

using namespace ema;

class TestPromise : public testing::Test {
   protected:
	static void SetUpTestSuite() {
	}
	static void TearDownTestSuite() {
	}

	void SetUp() override {
	}

	void TearDown() override {
	}
};

// Test basic success case with void types
TEST_F(TestPromise, BasicSuccessVoid) {
	Promise<void, void> promise;
	bool successCalled = false;
	bool failCalled	   = false;

	auto future = promise.GetFuture();
	future.Then([&successCalled]() { successCalled = true; });
	future.Fail([&failCalled]() { failCalled = true; });

	EXPECT_FALSE(future.IsFinished());
	EXPECT_FALSE(future.IsSuccessed());

	promise.SetSuccessed();

	EXPECT_TRUE(future.IsFinished());
	EXPECT_TRUE(future.IsSuccessed());
	EXPECT_TRUE(successCalled);
	EXPECT_FALSE(failCalled);
}

// Test basic failure case with void types
TEST_F(TestPromise, BasicFailVoid) {
	Promise<void, void> promise;
	bool successCalled = false;
	bool failCalled	   = false;

	auto future = promise.GetFuture();
	future.Then([&successCalled]() { successCalled = true; });
	future.Fail([&failCalled]() { failCalled = true; });

	EXPECT_FALSE(future.IsFinished());
	EXPECT_FALSE(future.IsSuccessed());

	promise.SetFailed();

	EXPECT_TRUE(future.IsFinished());
	EXPECT_FALSE(future.IsSuccessed());
	EXPECT_FALSE(successCalled);
	EXPECT_TRUE(failCalled);
}

// Test with non-void success type
TEST_F(TestPromise, SuccessWithValue) {
	Promise<void, int> promise;
	int receivedValue = 0;

	auto future = promise.GetFuture();
	future.Then([&receivedValue](int&& value) { receivedValue = value; });

	promise.SetSuccessed(42);

	EXPECT_TRUE(future.IsFinished());
	EXPECT_TRUE(future.IsSuccessed());
	EXPECT_EQ(receivedValue, 42);
}

// Test with non-void failure type
TEST_F(TestPromise, FailWithValue) {
	Promise<std::string, void> promise;
	std::string errorMessage;

	auto future = promise.GetFuture();
	future.Fail([&errorMessage](std::string&& msg) { errorMessage = msg; });

	promise.SetFailed("Error occurred");

	EXPECT_TRUE(future.IsFinished());
	EXPECT_FALSE(future.IsSuccessed());
	EXPECT_EQ(errorMessage, "Error occurred");
}

// Test chaining then/fail callbacks
TEST_F(TestPromise, ChainCallbacks) {
	Promise<std::string, int> promise;
	std::string errorMessage;
	int successValue = 0;

	auto future = promise.GetFuture();
	future.Then([&successValue](int&& value) { successValue = value; }).Fail([&errorMessage](std::string&& msg) {
		errorMessage = msg;
	});

	// Test success path
	promise.SetSuccessed(100);

	EXPECT_TRUE(future.IsFinished());
	EXPECT_TRUE(future.IsSuccessed());
	EXPECT_EQ(successValue, 100);
	EXPECT_TRUE(errorMessage.empty());
}

// Test multiple futures from same promise
TEST_F(TestPromise, MultipleFutures) {
	Promise<void, int> promise;
	int value1 = 0;
	int value2 = 0;

	auto future1 = promise.GetFuture();
	auto future2 = promise.GetFuture();

	future1.Then([&value1](int&& val) { value1 = val; });

	future2.Then([&value2](int&& val) { value2 = val; });

	promise.SetSuccessed(42);

	EXPECT_TRUE(future1.IsFinished());
	EXPECT_TRUE(future1.IsSuccessed());
	EXPECT_TRUE(future2.IsFinished());
	EXPECT_TRUE(future2.IsSuccessed());
	EXPECT_EQ(value1, 0);
	EXPECT_EQ(value2, 42);
}

// 测试同一个Future上多个回调，验证只有最后注册的回调会生效
TEST_F(TestPromise, LastCallbackOnlyEffective) {
	Promise<void, int> promise;
	int firstCallbackValue	= 0;
	int secondCallbackValue = 0;

	auto future = promise.GetFuture();

	// 注册第一个回调
	future.Then([&firstCallbackValue](int&& val) { firstCallbackValue = val; });

	// 注册第二个回调 - 这会替换第一个回调
	future.Then([&secondCallbackValue](int&& val) { secondCallbackValue = val; });

	promise.SetSuccessed(42);

	// 只有第二个回调应该被执行
	EXPECT_EQ(firstCallbackValue, 0);	 // 第一个回调被替换，值不变
	EXPECT_EQ(secondCallbackValue, 42);	 // 第二个回调被执行

	// 测试失败回调也是一样的情况
	Promise<std::string, void> promiseFail;
	std::string firstErrorMessage;
	std::string secondErrorMessage;

	auto futureFail = promiseFail.GetFuture();

	futureFail.Fail([&firstErrorMessage](std::string&& msg) { firstErrorMessage = msg; });

	futureFail.Fail([&secondErrorMessage](std::string&& msg) { secondErrorMessage = msg; });

	promiseFail.SetFailed("Error occurred");

	// 只有第二个回调应该被执行
	EXPECT_TRUE(firstErrorMessage.empty());			  // 第一个回调被替换
	EXPECT_EQ(secondErrorMessage, "Error occurred");  // 第二个回调被执行
}
