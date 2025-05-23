#include "gtest/gtest.h"
#include "test_event_bus.h"
//#include "test_log.h"
#include "test_process.h"
#include "test_promise.h"
#include "test_string.h"
//#include "test_thread_pool.h"

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}