#pragma once
#include "gtest/gtest.h"
#include "native/process.h"
#include "string/str.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tlhelp32.h>
#endif

using namespace ema;

class TestProcess : public testing::Test {
   public:
   protected:
	static void SetUpTestSuite() {
	}
	static void TearDownTestSuite() {
	}
};

TEST_F(TestProcess, GetProcName) {
	auto procName = native::process::GetProcName();
	EXPECT_TRUE("test.exe" == procName);
}

TEST_F(TestProcess, GetProcPath) {
	std::string path;
#ifdef _WIN32
	path.resize(MAX_PATH);
	wchar_t buffer[MAX_PATH]{0};
	::GetModuleFileNameW(NULL, buffer, MAX_PATH);
	auto path2 = String::from_wstring(buffer);
	path2.replace("\\", "/");
#endif
	EXPECT_TRUE(path2 == native::process::GetProcPath());
}

TEST_F(TestProcess, GetProcId) {
	auto currentId = native::process::GetProcID();
	auto id		   = native::process::GetProcByName("test.exe");
	EXPECT_TRUE(id == currentId);
}
