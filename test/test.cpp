#include "gtest/gtest.h"
#include "test_string.h"
#include "test_process.h"
#include "test_log.h"

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}