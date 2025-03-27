#pragma once
#include "gtest/gtest.h"
#include "string/str.h"

using namespace enigma;

class TestString : public testing::Test {
   public:
   protected:
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}
};

TEST_F(TestString, ContertTest) {
	string str		  = "Hello World";
	std::wstring wstr = str.to_wstring();
	EXPECT_EQ(L"Hello World", wstr);

	string from_wstring = string::from_wstring(wstr);
	EXPECT_EQ("Hello World", from_wstring);
}

TEST_F(TestString, TrimTest) {
	string str = "  Hello World  ";
	str.trim();
	EXPECT_EQ("Hello World", str);
}

TEST_F(TestString, ToLowerTest) {
	string str = "Hello World";
	str.to_lower();
	EXPECT_EQ("hello world", str);
}

TEST_F(TestString, ToUpperTest) {
	string str = "Hello World";
	str.to_upper();
	EXPECT_EQ("HELLO WORLD", str);
}

TEST_F(TestString, ReplaceTest) {
	string str = "Hello World";
	str.replace("World", "Enigma");
	EXPECT_EQ("Hello Enigma", str);
}

TEST_F(TestString, SplitTest) {
	string str = "Hello,World,Enigma";
	auto vec   = str.split(",");
	EXPECT_EQ(3, vec.size());
	EXPECT_EQ("Hello", vec[0]);
	EXPECT_EQ("World", vec[1]);
	EXPECT_EQ("Enigma", vec[2]);

	string str2 = "Hello,World,,Enigma";
	auto vec2	= str2.split(",", true);
	EXPECT_EQ(4, vec2.size());
	EXPECT_EQ("Hello", vec2[0]);
	EXPECT_EQ("World", vec2[1]);
	EXPECT_EQ("", vec2[2]);
	EXPECT_EQ("Enigma", vec2[3]);
}

TEST_F(TestString, OperatorTest) {
	string str = "Hello World";
	EXPECT_EQ("Hello World", str);
	EXPECT_EQ('H', str[0]);
	str[0] = 'h';
	EXPECT_EQ("hello World", str);
	EXPECT_EQ(0, strcmp("hello World", str.c_str()));
	EXPECT_EQ("hello World", static_cast<std::string>(str));
}

TEST_F(TestString, CopyTest) {
	string str = "Hello World";
	string str2(str);
	EXPECT_EQ("Hello World", str2);
	EXPECT_EQ("Hello World", str);
	str2 = "Enigma";
	EXPECT_EQ("Hello World", str);
	EXPECT_EQ("Enigma", str2);
	str = str2;
	EXPECT_EQ("Enigma", str);
	EXPECT_EQ("Enigma", str2);
	str	 = "abc";
	str2 = std::move(str);
	EXPECT_EQ("abc", str2);
	str = std::move(str2);
	EXPECT_EQ("abc", str);
}

TEST_F(TestString, CompareTest) {
	string str = "Hello World";
	string str2 = "Hello World";
	EXPECT_TRUE(str == str2);
	EXPECT_TRUE(str == "Hello World");
	EXPECT_TRUE(str != "Enigma");
	EXPECT_TRUE(str < "Hello Worlda");
	EXPECT_FALSE(str > "Hello World");
	EXPECT_TRUE(str <= "Hello World");
	EXPECT_TRUE(str >= "Hello World");
	EXPECT_TRUE(str <= "Hello Worlda");
	EXPECT_TRUE(str >= "Hello World");
}
