#pragma once
#include "gtest/gtest.h"
#include "native/process.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tlhelp32.h>
#endif

using namespace enigma;

class TestProcess : public testing::Test {
   public:
   protected:
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

TEST_F(TestProcess, GetProcName) {
	auto proc_name = native::process::get_proc_name();
	EXPECT_TRUE("test.exe" == proc_name);
}

TEST_F(TestProcess, GetProcPath) {
	string path;
#ifdef _WIN32
	path.resize(MAX_PATH);
	wchar_t buffer[MAX_PATH]{0};
	::GetModuleFileNameW(NULL, buffer, MAX_PATH);
	path = string::from_wstring(buffer);
	path.replace("\\", "/");
#endif
	EXPECT_TRUE(path == native::process::get_proc_path());
}

TEST_F(TestProcess, GetProcId) {
	auto current_id = native::process::get_proc_id();
	auto id			= native::process::get_proc_by_name("test.exe");
	EXPECT_TRUE(id == current_id);
}
