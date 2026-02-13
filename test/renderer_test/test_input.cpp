#include "pch.h"
#include "Input.h"

TEST(InputParseKeyState, ParsesSingleKey)
{
	const Input::KeyState state = Input::ParseKeyState("a");
	EXPECT_EQ(state.special, Input::NONE);
	EXPECT_EQ(state.normal, 1);
}

TEST(InputParseKeyState, ParsesModifiersAndKey)
{
	const Input::KeyState state = Input::ParseKeyState("ctrl+shift+b");
	EXPECT_EQ(state.special, Input::CTRL | Input::SHIFT);
	EXPECT_EQ(state.normal, 2);
}

TEST(InputParseKeyState, ParsesAllModifiers)
{
	const Input::KeyState state = Input::ParseKeyState("ctrl+shift+alt+z");
	EXPECT_EQ(state.special, Input::CTRL | Input::SHIFT | Input::ALT);
	EXPECT_EQ(state.normal, 26);
}

TEST(InputParseKeyState, TrimsWhitespace)
{
	const Input::KeyState state = Input::ParseKeyState("  ctrl + alt + c  ");
	EXPECT_EQ(state.special, Input::CTRL | Input::ALT);
	EXPECT_EQ(state.normal, 3);
}

TEST(InputParseKeyState, Duplication)
{
	const Input::KeyState state = Input::ParseKeyState("ctrl+ctrl+ctrl+ctrl+shift+z");
	EXPECT_EQ(state.special, Input::CTRL | Input::SHIFT);
	EXPECT_EQ(state.normal, 26);
}
