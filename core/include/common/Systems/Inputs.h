#pragma once

#include "Locator/Locator.h"
#include "Common/Debug.h"
#include "Common/GameTime.h"
#include "Common/StringUtils.h"
#include "Common/Vec.h"

#include "SDL.h"

#include <memory>
#include <optional>
#include <variant>

class LoggerBase
{
	virtual void StartLogging() = 0, StopLogging() = 0;
};

class RecorderBase
{
public:
	virtual void StartRecording() = 0, StopRecording() = 0;
	virtual void StartReplaying() = 0, StopReplaying() = 0;
	virtual void SaveRecording(const std::string &filename) = 0;
	virtual void LoadRecording(const std::string &filename) = 0;
};

// TODO: Make Logger and Recorder only be created if used.
class Inputs : public LoggerBase, public RecorderBase
{
private:
	class Logger;
	class Recorder;
	std::unique_ptr<Logger> logger;
	std::unique_ptr<Recorder> recorder;

	enum NewState
	{
		WasPressed = SDL_PRESSED,
		WasReleased = SDL_RELEASED
	};

public:
	enum InputKey
	{
		A = 4,
		B = 5,
		C = 6,
		D = 7,
		E = 8,
		F = 9,
		G = 10,
		H = 11,
		I = 12,
		J = 13,
		K = 14,
		L = 15,
		M = 16,
		N = 17,
		O = 18,
		P = 19,
		Q = 20,
		R = 21,
		S = 22,
		T = 23,
		U = 24,
		V = 25,
		W = 26,
		X = 27,
		Y = 28,
		Z = 29,

		N1 = 30,
		N2 = 31,
		N3 = 32,
		N4 = 33,
		N5 = 34,
		N6 = 35,
		N7 = 36,
		N8 = 37,
		N9 = 38,
		N0 = 39,

		RETURN = 40,
		ESCAPE = 41,
		BACKSPACE = 42,
		TAB = 43,
		SPACE = 44,

		MINUS = 45,
		EQUALS = 46,
		LEFTBRACKET = 47,
		RIGHTBRACKET = 48,
		BACKSLASH = 49,

		NONUSHASH = 50,

		SEMICOLON = 51,
		APOSTROPHE = 52,
		GRAVE = 53,

		COMMA = 54,
		PERIOD = 55,
		SLASH = 56,

		CAPSLOCK = 57,

		F1 = 58,
		F2 = 59,
		F3 = 60,
		F4 = 61,
		F5 = 62,
		F6 = 63,
		F7 = 64,
		F8 = 65,
		F9 = 66,
		F10 = 67,
		F11 = 68,
		F12 = 69,

		PRINTSCREEN = 70,
		SCROLLLOCK = 71,
		PAUSE = 72,
		INSERT = 73,

		HOME = 74,
		PAGEUP = 75,
		DELETE = 76,
		END = 77,
		PAGEDOWN = 78,
		RIGHT = 79,
		LEFT = 80,
		DOWN = 81,
		UP = 82,

		NUMLOCKCLEAR = 83,

		KP_DIVIDE = 84,
		KP_MULTIPLY = 85,
		KP_MINUS = 86,
		KP_PLUS = 87,
		KP_ENTER = 88,
		KP_1 = 89,
		KP_2 = 90,
		KP_3 = 91,
		KP_4 = 92,
		KP_5 = 93,
		KP_6 = 94,
		KP_7 = 95,
		KP_8 = 96,
		KP_9 = 97,
		KP_0 = 98,
		KP_PERIOD = 99,

		NONUSBACKSLASH = 100,

		APPLICATION = 101,
		POWER = 102,

		KP_EQUALS = 103,
		F13 = 104,
		F14 = 105,
		F15 = 106,
		F16 = 107,
		F17 = 108,
		F18 = 109,
		F19 = 110,
		F20 = 111,
		F21 = 112,
		F22 = 113,
		F23 = 114,
		F24 = 115,
		EXECUTE = 116,
		HELP = 117,
		MENU = 118,
		SELECT = 119,
		STOP = 120,
		AGAIN = 121,
		UNDO = 122,
		CUT = 123,
		COPY = 124,
		PASTE = 125,
		FIND = 126,
		MUTE = 127,
		VOLUMEUP = 128,
		VOLUMEDOWN = 129,

		KP_COMMA = 133,
		KP_EQUALSAS400 = 134,

		INTERNATIONAL1 = 135,

		INTERNATIONAL2 = 136,
		INTERNATIONAL3 = 137,
		INTERNATIONAL4 = 138,
		INTERNATIONAL5 = 139,
		INTERNATIONAL6 = 140,
		INTERNATIONAL7 = 141,
		INTERNATIONAL8 = 142,
		INTERNATIONAL9 = 143,
		LANG1 = 144,
		LANG2 = 145,
		LANG3 = 146,
		LANG4 = 147,
		LANG5 = 148,
		LANG6 = 149,
		LANG7 = 150,
		LANG8 = 151,
		LANG9 = 152,

		ALTERASE = 153,
		SYSREQ = 154,
		CANCEL = 155,
		CLEAR = 156,
		PRIOR = 157,
		RETURN2 = 158,
		SEPARATOR = 159,
		OUT = 160,
		OPER = 161,
		CLEARAGAIN = 162,
		CRSEL = 163,
		EXSEL = 164,

		KP_00 = 176,
		KP_000 = 177,
		THOUSANDSSEPARATOR = 178,
		DECIMALSEPARATOR = 179,
		CURRENCYUNIT = 180,
		CURRENCYSUBUNIT = 181,
		KP_LEFTPAREN = 182,
		KP_RIGHTPAREN = 183,
		KP_LEFTBRACE = 184,
		KP_RIGHTBRACE = 185,
		KP_TAB = 186,
		KP_BACKSPACE = 187,
		KP_A = 188,
		KP_B = 189,
		KP_C = 190,
		KP_D = 191,
		KP_E = 192,
		KP_F = 193,
		KP_XOR = 194,
		KP_POWER = 195,
		KP_PERCENT = 196,
		KP_LESS = 197,
		KP_GREATER = 198,
		KP_AMPERSAND = 199,
		KP_DBLAMPERSAND = 200,
		KP_VERTICALBAR = 201,
		KP_DBLVERTICALBAR = 202,
		KP_COLON = 203,
		KP_HASH = 204,
		KP_SPACE = 205,
		KP_AT = 206,
		KP_EXCLAM = 207,
		KP_MEMSTORE = 208,
		KP_MEMRECALL = 209,
		KP_MEMCLEAR = 210,
		KP_MEMADD = 211,
		KP_MEMSUBTRACT = 212,
		KP_MEMMULTIPLY = 213,
		KP_MEMDIVIDE = 214,
		KP_PLUSMINUS = 215,
		KP_CLEAR = 216,
		KP_CLEARENTRY = 217,
		KP_BINARY = 218,
		KP_OCTAL = 219,
		KP_DECIMAL = 220,
		KP_HEXADECIMAL = 221,

		LCTRL = 224,
		LSHIFT = 225,
		LALT = 226,
		LGUI = 227,
		RCTRL = 228,
		RSHIFT = 229,
		RALT = 230,
		RGUI = 231,

		MODE = 257,

		AUDIONEXT = 258,
		AUDIOPREV = 259,
		AUDIOSTOP = 260,
		AUDIOPLAY = 261,
		AUDIOMUTE = 262,
		MEDIASELECT = 263,
		WWW = 264,
		MAIL = 265,
		CALCULATOR = 266,
		COMPUTER = 267,
		AC_SEARCH = 268,
		AC_HOME = 269,
		AC_BACK = 270,
		AC_FORWARD = 271,
		AC_STOP = 272,
		AC_REFRESH = 273,
		AC_BOOKMARKS = 274,

		BRIGHTNESSDOWN = 275,
		BRIGHTNESSUP = 276,
		DISPLAYSWITCH = 277,

		KBDILLUMTOGGLE = 278,
		KBDILLUMDOWN = 279,
		KBDILLUMUP = 280,
		EJECT = 281,
		SLEEP = 282,

		APP1 = 283,
		APP2 = 284,

		AUDIOREWIND = 285,
		AUDIOFASTFORWARD = 286,

		MAX = 512
	};

	using Key = InputKey;
	using Button = byte;

	struct AnyInput;

	enum State
	{
		Undefined = 0,
		Pressed,
		Held,
		Released,
		Free
	};

	enum AxisValueMode { Raw, Smooth };

	Inputs();
	~Inputs();
	void StartLogging(), StopLogging();
	void StartRecording(), StopRecording();
	void StartReplaying(), StopReplaying();
	void SaveRecording(const std::string &filename);
	void LoadRecording(const std::string &filename);


	State GetKey(Key code);
	State GetMouseButton(Button button);
	dVec2 GetMousePos();
	dVec2 GetMouseMove();
	dVec2 GetMouseWheel();

	bool IsKeyPressed(Key code);
	bool IsKeyReleased(Key code);
	bool IsKeyDown(Key code);
	bool IsKeyUp(Key code);
		 
	bool IsMouseButtonPressed(Button button);
	bool IsMouseButtonReleased(Button button);
	bool IsMouseButtonDown(Button button);
	bool IsMouseButtonUp(Button button);

	void AddAxis(const std::string &name, AnyInput positive, AnyInput negative);
	double GetAxis(const std::string &name, AxisValueMode mode = Raw);

private:
	using stringHash = uint;
	struct AxisInfo;

	void ignoreInputs(bool ignore);

	void setKey(SDL_Event &);
	void setMouseButton(SDL_Event &);
	void setMousePos(SDL_Event &);
	void setMouseWheel(SDL_Event &);
	void update();

	void GetKey_(Key code, State &state);
	void GetMouseButton_(Button button, State &state);
	void GetMousePos_(dVec2 &v);
	void GetMouseMove_(dVec2 &v);
	void GetMouseWheel_(dVec2 &v);


	std::optional<Inputs::AxisInfo> getAxisInfo(const std::string &str);
	stringHash hash(const std::string &str);

	std::array<State, SDL_NUM_SCANCODES> keyStates;
	std::array<State, 5> buttonStates;
	dVec2 mouseMove;
	dVec2 mousePos;
	dVec2 mouseWheelMove;
	bool isIgnored;

	Locator::CmdNode inputFuncsTree, pollFuncTree;

	std::unordered_map<stringHash, AxisInfo> axes;
	const double smoothAxisSharpness = 0.99875;
};

struct Inputs::AnyInput
{
	using ValueType = std::variant<Key, Button>;

	AnyInput() : value(0) {}
	AnyInput(ValueType value) : value(value) {}
	AnyInput(Key value) : value(value) {}
	AnyInput(Button value) : value(value) {}

	bool IsKey() { return value.index() == 0; }

	ValueType value;
};

struct Inputs::AxisInfo
{
	AnyInput positive, negative;
	double value, smoothValue;
};

class Inputs::Logger : private LoggerBase
{
public:
	Logger(Inputs &inputs);

private:
	friend Inputs;

	virtual void StartLogging(), StopLogging();

	void setKey(SDL_Event &);
	void setMouseButton(SDL_Event &);
	void setMousePos(SDL_Event &);
	void setMouseWheel(SDL_Event &);


	Locator::CmdNode tree;
	Inputs &inputs;
};

class Inputs::Recorder : private RecorderBase
{
public:
	Recorder(Inputs &inputs);

private:
	friend Inputs;

	void StartRecording(), StopRecording();
	void StartReplaying(), StopReplaying();
	void SaveRecording(const std::string &filename), LoadRecording(const std::string &filename);

	void whileRecording(SDL_Event &);
	void whileReplaying();

	void simulateNextInput();

	struct InputInfo
	{
		Time::TimePoint Time;
		SDL_EventType Type; 
		std::variant<Key, Uint8, dVec2> Value; 

	//template<class Archive>
	//void serialize(Archive &archive)
	//{
	//	//archive((int)Type); 
	//}

		
		friend std::ostream &operator<<(std::ostream &os, const Inputs::Recorder::InputInfo &info)
		{ 
			os << '(' << std::to_string(info.Time) << ',' << std::to_string(info.Type) << ',';
			os << '<' << info.Value.index() << ',';
			switch (info.Value.index())
			{
			case 0: { os << std::to_string(std::get<Key>(info.Value)); } break;
			case 1: { os << std::to_string(std::get<Uint8>(info.Value)); } break;
			case 2: { os << std::get<dVec2>(info.Value); } break;
			}
			os << '>';

			os << ')';

			return os;
		}

		friend std::istream &operator>>(std::istream &is, Inputs::Recorder::InputInfo &info)
		{
			if(is.peek() == '(')
			{
				int variantIndex;
				double time;
				int eventType;

				is.get(); // (
				is >> time;
				info.Time = time;
				is.get(); // ,
				is >> eventType;
				info.Type = static_cast<SDL_EventType>(eventType);
				is.get(); // ,
				is.get(); // <
				is >> variantIndex;
				is.get(); // ,

				switch (variantIndex)
				{
				case 0: 
				{ 
					int key;
					is >> key;
					info.Value = static_cast<Inputs::Key>(key);
				} break;
				case 1: 
				{ 
					int n;
					is >> n;
					info.Value = static_cast<Uint8>(n);
				} break;
				case 2: 
				{ 
					dVec2 v;
					is >> v;
					info.Value = v;
				} break;
				}

				is.get(); // >
				is.get(); // )
			}

			return is;
		}
	};

private:
	std::vector<InputInfo> recordedInputs;

	bool isRecording = false, isReplaying = false;
	Time::TimePoint actionStartTime;
	int replayCurrentInput = -1;

	Locator::CmdNode recordTree;
	Locator::CmdNode replayTree;
	Inputs &inputs;
};

template<class Archive>
void save(Archive &archive, 
	std::variant<Inputs::Key, Uint8, dVec2> const &var)
{ 
	switch (var.index())
	{
	case 0: { archive(var.index(), static_cast<int>(std::get<Inputs::Key>(var))); } break;
	case 1: { archive(var.index(), std::get<Uint8>(var)); } break;
	case 2: { archive(var.index(), std::get<dVec2>(var)); } break;
	}
}

template<class Archive>
void load(Archive &archive, 
	std::variant<Inputs::Key, Uint8, dVec2> &var)
{ 
	switch (var.index())
	{
	case 0: { archive(var.index(), static_cast<int>(std::get<Inputs::Key>(var))); } break;
	case 1: { archive(var.index(), std::get<Uint8>(var)); } break;
	case 2: { archive(var.index(), std::get<dVec2>(var)); } break;
	}
}

//friend std::istream &operator>>(std::istream &is, Inputs::Recorder::InputInfo &info)
//{
//	if(is.peek() == '(')
//	{
//		int variantIndex;
//		double time;
//		int eventType;
//
//		is.get(); // (
//		is >> time;
//		info.Time = time;
//		is.get(); // ,
//		is >> eventType;
//		info.Type = static_cast<SDL_EventType>(eventType);
//		is.get(); // ,
//		is.get(); // <
//		is >> variantIndex;
//		is.get(); // ,
//
//		switch (variantIndex)
//		{
//		case 0: 
//		{ 
//			int key;
//			is >> key;
//			info.Value = static_cast<Inputs::Key>(key);
//		} break;
//		case 1: 
//		{ 
//			Uint8 n;
//			is >> n;
//			info.Value = n;
//		} break;
//		case 2: 
//		{ 
//			dVec2 v;
//			is >> v;
//			info.Value = v;
//		} break;
//		}
//
//		is.get(); // >
//		is.get(); // )
//	}
//
//	return is;
//}