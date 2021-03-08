#pragma once

//All numbers are from the official windows documentation: https://docs.microsoft.com/en-us/dotnet/api/system.windows.input.key?view=netframework-4.8
enum class Keys {
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,
	Num0 = 34,
	Num1 = 35,
	Num2 = 36,
	Num3 = 37,
	Num4 = 38,
	Num5 = 39,
	Num6 = 40,
	Num7 = 41,
	Num8 = 42,
	Num9 = 43,
	Space = 32,
	Enter = 13,
	Back = 8,
	Up = 38,
	Right = 39,
	Down = 40,
	Left = 37,
	Shift = 16,
	Ctrl = 17,
};

enum class MouseKeys {
	LButton = 0,
	RButton = 1,
	Middle = 2,
};

enum class MouseAxis {
	PositionHorizontal = 0,
	PositionVertical = 1,
	ScrollDelta = 2
};

enum class JoystickButtons {
	A = 0,
	B = 1,
	X = 2,
	Y = 3,
	LB = 4,
	RB = 5,
	Back = 6,
	Start = 7,
	L3 = 8,
	R3 = 9,
	Up = 10,
	Right = 11,
	Down = 12,
	Left = 13
};

enum class JoystickThumb {
	ThumbL = 0,
	ThumbR = 1
};

enum class JoystickBack {
	BackL = 0,
	BackR = 1
};

enum class JoystickAxis {
	LThumbHorizontal = 0,
	LThumbVertical = 1,
	RThumbHorizontal = 2,
	RThumbVertical = 3,
	L2 = 4,
	R2 = 5
};