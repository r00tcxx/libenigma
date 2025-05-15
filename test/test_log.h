#pragma once
#include "gtest/gtest.h"
#include "log/log.h"
#include "string/str.h"

using namespace ema;

class TestLog : public testing::Test {
   public:
   protected:
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

TEST_F(TestLog, SingleFileLog) {
}
