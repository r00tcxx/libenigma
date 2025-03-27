#pragma once
#include "gtest/gtest.h"
#include "log/log.h"
#include "string/str.h"

using namespace enigma;

class TestLog : public testing::Test {
   public:
   protected:
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

TEST_F(TestLog, SingleFileLog) {
	using namespace log;
	std::vector<sink::ptr> sinks;
	sinks.emplace_back(make_file_sink(file_sink_config{
		.log_dir  = native::process::get_proc_dir() + "/log",
		.app_name = "test",
		.max_file_size = 128,
		.max_file_count = 3
	}));
	sinks.emplace_back(make_console_sink(console_sink_config{}));

	init_logger(log_level::debug, std::move(sinks));

	char buffer[128]{'a'};
	memset(buffer, 'a', 126);
	buffer[127] = 0;

	log_debug(buffer);
	log_debug("231212");

	getchar();

	uninit_logger();
}
