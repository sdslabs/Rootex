#include "input_interface.h"

#undef interface
#include "RmlUi/Debugger.h"
#define interface __STRUCT__

static int GetKeyModifierState();
static void InitialiseKeymap();

#define KEYMAP_SIZE 256
static Rml::Core::Input::KeyIdentifier KeyIdentifierMap[KEYMAP_SIZE];

bool InputInterface::Initialise()
{
	InitialiseKeymap();
	return true;
}

void InputInterface::Shutdown()
{
}

char ascii_map[4][51] = {
	// shift off and capslock off
	{
	    0,
	    ' ',
	    '0',
	    '1',
	    '2',
	    '3',
	    '4',
	    '5',
	    '6',
	    '7',
	    '8',
	    '9',
	    'a',
	    'b',
	    'c',
	    'd',
	    'e',
	    'f',
	    'g',
	    'h',
	    'i',
	    'j',
	    'k',
	    'l',
	    'm',
	    'n',
	    'o',
	    'p',
	    'q',
	    'r',
	    's',
	    't',
	    'u',
	    'v',
	    'w',
	    'x',
	    'y',
	    'z',
	    ';',
	    '=',
	    ',',
	    '-',
	    '.',
	    '/',
	    '`',
	    '[',
	    '\\',
	    ']',
	    '\'',
	    0,
	    0 },

	// shift on and capslock off
	{
	    0,
	    ' ',
	    ')',
	    '!',
	    '@',
	    '#',
	    '$',
	    '%',
	    '^',
	    '&',
	    '*',
	    '(',
	    'A',
	    'B',
	    'C',
	    'D',
	    'E',
	    'F',
	    'G',
	    'H',
	    'I',
	    'J',
	    'K',
	    'L',
	    'M',
	    'N',
	    'O',
	    'P',
	    'Q',
	    'R',
	    'S',
	    'T',
	    'U',
	    'V',
	    'W',
	    'X',
	    'Y',
	    'Z',
	    ':',
	    '+',
	    '<',
	    '_',
	    '>',
	    '?',
	    '~',
	    '{',
	    '|',
	    '}',
	    '"',
	    0,
	    0 },

	// shift on and capslock on
	{
	    0,
	    ' ',
	    ')',
	    '!',
	    '@',
	    '#',
	    '$',
	    '%',
	    '^',
	    '&',
	    '*',
	    '(',
	    'a',
	    'b',
	    'c',
	    'd',
	    'e',
	    'f',
	    'g',
	    'h',
	    'i',
	    'j',
	    'k',
	    'l',
	    'm',
	    'n',
	    'o',
	    'p',
	    'q',
	    'r',
	    's',
	    't',
	    'u',
	    'v',
	    'w',
	    'x',
	    'y',
	    'z',
	    ':',
	    '+',
	    '<',
	    '_',
	    '>',
	    '?',
	    '~',
	    '{',
	    '|',
	    '}',
	    '"',
	    0,
	    0 },

	// shift off and capslock on
	{
	    0,
	    ' ',
	    '1',
	    '2',
	    '3',
	    '4',
	    '5',
	    '6',
	    '7',
	    '8',
	    '9',
	    '0',
	    'A',
	    'B',
	    'C',
	    'D',
	    'E',
	    'F',
	    'G',
	    'H',
	    'I',
	    'J',
	    'K',
	    'L',
	    'M',
	    'N',
	    'O',
	    'P',
	    'Q',
	    'R',
	    'S',
	    'T',
	    'U',
	    'V',
	    'W',
	    'X',
	    'Y',
	    'Z',
	    ';',
	    '=',
	    ',',
	    '-',
	    '.',
	    '/',
	    '`',
	    '[',
	    '\\',
	    ']',
	    '\'',
	    0,
	    0 }
};

char KeypadMap[2][18] = {
	{ '0',
	    '1',
	    '2',
	    '3',
	    '4',
	    '5',
	    '6',
	    '7',
	    '8',
	    '9',
	    '\n',
	    '*',
	    '+',
	    0,
	    '-',
	    '.',
	    '/',
	    '=' },

	{ 0,
	    0,
	    0,
	    0,
	    0,
	    0,
	    0,
	    0,
	    0,
	    0,
	    '\n',
	    '*',
	    '+',
	    0,
	    '-',
	    0,
	    '/',
	    '=' }
};

Rml::Core::Context* InputInterface::s_Context = nullptr;

// Sets the context to send input events to.
void InputInterface::SetContext(Rml::Core::Context* context)
{
	s_Context = context;
}

// Returns the character code for a key identifer / key modifier combination.
Rml::Core::Character InputInterface::GetCharacterCode(Rml::Core::Input::KeyIdentifier keyIdentifier, int keyModifierState)
{
	using Rml::Core::Character;

	// Check if we have a keycode capable of generating characters on the main keyboard (ie, not on the numeric
	// keypad; that is dealt with below).
	if (keyIdentifier <= Rml::Core::Input::KI_OEM_102)
	{
		// Get modifier states
		bool shift = (keyModifierState & Rml::Core::Input::KM_SHIFT) > 0;
		bool capslock = (keyModifierState & Rml::Core::Input::KM_CAPSLOCK) > 0;

		// Return character code based on identifier and modifiers
		if (shift && !capslock)
			return (Character)ascii_map[1][keyIdentifier];

		if (shift && capslock)
			return (Character)ascii_map[2][keyIdentifier];

		if (!shift && capslock)
			return (Character)ascii_map[3][keyIdentifier];

		return (Character)ascii_map[0][keyIdentifier];
	}

	// Check if we have a keycode from the numeric keypad.
	else if (keyIdentifier <= Rml::Core::Input::KI_OEM_NEC_EQUAL)
	{
		if (keyModifierState & Rml::Core::Input::KM_NUMLOCK)
			return (Character)KeypadMap[0][keyIdentifier - Rml::Core::Input::KI_NUMPAD0];
		else
			return (Character)KeypadMap[1][keyIdentifier - Rml::Core::Input::KI_NUMPAD0];
	}

	else if (keyIdentifier == Rml::Core::Input::KI_RETURN)
		return (Character)'\n';

	return Character::Null;
}

void InputInterface::ProcessWindowsEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (s_Context == nullptr)
	{
		return;
	}

	switch (message)
	{
	case WM_LBUTTONDOWN:
		s_Context->ProcessMouseButtonDown(0, GetKeyModifierState());
		break;

	case WM_LBUTTONUP:
		s_Context->ProcessMouseButtonUp(0, GetKeyModifierState());
		break;

	case WM_RBUTTONDOWN:
		s_Context->ProcessMouseButtonDown(1, GetKeyModifierState());
		break;

	case WM_RBUTTONUP:
		s_Context->ProcessMouseButtonUp(1, GetKeyModifierState());
		break;

	case WM_MBUTTONDOWN:
		s_Context->ProcessMouseButtonDown(2, GetKeyModifierState());
		break;

	case WM_MBUTTONUP:
		s_Context->ProcessMouseButtonUp(2, GetKeyModifierState());
		break;

	case WM_MOUSEMOVE:
		s_Context->ProcessMouseMove(LOWORD(lParam), HIWORD(lParam), GetKeyModifierState());
		break;

	case WM_MOUSEWHEEL:
		s_Context->ProcessMouseWheel(static_cast<float>((short)HIWORD(wParam)) / static_cast<float>(-WHEEL_DELTA), GetKeyModifierState());
		break;

	case WM_KEYDOWN:
	{
		Rml::Core::Input::KeyIdentifier key_identifier = KeyIdentifierMap[wParam];
		int keyModifierState = GetKeyModifierState();

		// Check for F8 to toggle the debugger.
		if (key_identifier == Rml::Core::Input::KI_F8)
		{
			Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
			break;
		}

		s_Context->ProcessKeyDown(key_identifier, keyModifierState);
	}
	break;

	case WM_CHAR:
	{
		static char16_t firstU16CodeUnit = 0;

		char16_t c = (char16_t)wParam;
		Rml::Core::Character character = (Rml::Core::Character)c;

		// Windows sends two-wide characters as two messages.
		if (c >= 0xD800 && c < 0xDC00)
		{
			// First 16-bit code unit of a two-wide character.
			firstU16CodeUnit = c;
		}
		else
		{
			if (c >= 0xDC00 && c < 0xE000 && firstU16CodeUnit != 0)
			{
				// Second 16-bit code unit of a two-wide character.
				Rml::Core::String utf8 = Rml::Core::StringUtilities::ToUTF8({ firstU16CodeUnit, c });
				character = Rml::Core::StringUtilities::ToCharacter(utf8.data());
			}
			else if (c == '\r')
			{
				// Windows sends new-lines as carriage returns, convert to endlines.
				character = (Rml::Core::Character)'\n';
			}

			firstU16CodeUnit = 0;

			// Only send through printable characters.
			if (((char32_t)character >= 32 || character == (Rml::Core::Character)'\n') && character != (Rml::Core::Character)127)
				s_Context->ProcessTextInput(character);
		}
	}
	break;

	case WM_KEYUP:
		s_Context->ProcessKeyUp(KeyIdentifierMap[wParam], GetKeyModifierState());
		break;
	}
}

static int GetKeyModifierState()
{
	int key_modifier_state = 0;

	// Query the state of all modifier keys
	if (GetKeyState(VK_CAPITAL) & 1)
	{
		key_modifier_state |= Rml::Core::Input::KM_CAPSLOCK;
	}

	if (HIWORD(GetKeyState(VK_SHIFT)) & 1)
	{
		key_modifier_state |= Rml::Core::Input::KM_SHIFT;
	}

	if (GetKeyState(VK_NUMLOCK) & 1)
	{
		key_modifier_state |= Rml::Core::Input::KM_NUMLOCK;
	}

	if (HIWORD(GetKeyState(VK_CONTROL)) & 1)
	{
		key_modifier_state |= Rml::Core::Input::KM_CTRL;
	}

	if (HIWORD(GetKeyState(VK_MENU)) & 1)
	{
		key_modifier_state |= Rml::Core::Input::KM_ALT;
	}

	return key_modifier_state;
}

static void InitialiseKeymap()
{
	// Initialise the key map with default values.
	memset(KeyIdentifierMap, 0, sizeof(KeyIdentifierMap));

	// Assign individual values.
	KeyIdentifierMap['A'] = Rml::Core::Input::KI_A;
	KeyIdentifierMap['B'] = Rml::Core::Input::KI_B;
	KeyIdentifierMap['C'] = Rml::Core::Input::KI_C;
	KeyIdentifierMap['D'] = Rml::Core::Input::KI_D;
	KeyIdentifierMap['E'] = Rml::Core::Input::KI_E;
	KeyIdentifierMap['F'] = Rml::Core::Input::KI_F;
	KeyIdentifierMap['G'] = Rml::Core::Input::KI_G;
	KeyIdentifierMap['H'] = Rml::Core::Input::KI_H;
	KeyIdentifierMap['I'] = Rml::Core::Input::KI_I;
	KeyIdentifierMap['J'] = Rml::Core::Input::KI_J;
	KeyIdentifierMap['K'] = Rml::Core::Input::KI_K;
	KeyIdentifierMap['L'] = Rml::Core::Input::KI_L;
	KeyIdentifierMap['M'] = Rml::Core::Input::KI_M;
	KeyIdentifierMap['N'] = Rml::Core::Input::KI_N;
	KeyIdentifierMap['O'] = Rml::Core::Input::KI_O;
	KeyIdentifierMap['P'] = Rml::Core::Input::KI_P;
	KeyIdentifierMap['Q'] = Rml::Core::Input::KI_Q;
	KeyIdentifierMap['R'] = Rml::Core::Input::KI_R;
	KeyIdentifierMap['S'] = Rml::Core::Input::KI_S;
	KeyIdentifierMap['T'] = Rml::Core::Input::KI_T;
	KeyIdentifierMap['U'] = Rml::Core::Input::KI_U;
	KeyIdentifierMap['V'] = Rml::Core::Input::KI_V;
	KeyIdentifierMap['W'] = Rml::Core::Input::KI_W;
	KeyIdentifierMap['X'] = Rml::Core::Input::KI_X;
	KeyIdentifierMap['Y'] = Rml::Core::Input::KI_Y;
	KeyIdentifierMap['Z'] = Rml::Core::Input::KI_Z;

	KeyIdentifierMap['0'] = Rml::Core::Input::KI_0;
	KeyIdentifierMap['1'] = Rml::Core::Input::KI_1;
	KeyIdentifierMap['2'] = Rml::Core::Input::KI_2;
	KeyIdentifierMap['3'] = Rml::Core::Input::KI_3;
	KeyIdentifierMap['4'] = Rml::Core::Input::KI_4;
	KeyIdentifierMap['5'] = Rml::Core::Input::KI_5;
	KeyIdentifierMap['6'] = Rml::Core::Input::KI_6;
	KeyIdentifierMap['7'] = Rml::Core::Input::KI_7;
	KeyIdentifierMap['8'] = Rml::Core::Input::KI_8;
	KeyIdentifierMap['9'] = Rml::Core::Input::KI_9;

	KeyIdentifierMap[VK_BACK] = Rml::Core::Input::KI_BACK;
	KeyIdentifierMap[VK_TAB] = Rml::Core::Input::KI_TAB;

	KeyIdentifierMap[VK_CLEAR] = Rml::Core::Input::KI_CLEAR;
	KeyIdentifierMap[VK_RETURN] = Rml::Core::Input::KI_RETURN;

	KeyIdentifierMap[VK_PAUSE] = Rml::Core::Input::KI_PAUSE;
	KeyIdentifierMap[VK_CAPITAL] = Rml::Core::Input::KI_CAPITAL;

	KeyIdentifierMap[VK_KANA] = Rml::Core::Input::KI_KANA;
	KeyIdentifierMap[VK_HANGUL] = Rml::Core::Input::KI_HANGUL;
	KeyIdentifierMap[VK_JUNJA] = Rml::Core::Input::KI_JUNJA;
	KeyIdentifierMap[VK_FINAL] = Rml::Core::Input::KI_FINAL;
	KeyIdentifierMap[VK_HANJA] = Rml::Core::Input::KI_HANJA;
	KeyIdentifierMap[VK_KANJI] = Rml::Core::Input::KI_KANJI;

	KeyIdentifierMap[VK_ESCAPE] = Rml::Core::Input::KI_ESCAPE;

	KeyIdentifierMap[VK_CONVERT] = Rml::Core::Input::KI_CONVERT;
	KeyIdentifierMap[VK_NONCONVERT] = Rml::Core::Input::KI_NONCONVERT;
	KeyIdentifierMap[VK_ACCEPT] = Rml::Core::Input::KI_ACCEPT;
	KeyIdentifierMap[VK_MODECHANGE] = Rml::Core::Input::KI_MODECHANGE;

	KeyIdentifierMap[VK_SPACE] = Rml::Core::Input::KI_SPACE;
	KeyIdentifierMap[VK_PRIOR] = Rml::Core::Input::KI_PRIOR;
	KeyIdentifierMap[VK_NEXT] = Rml::Core::Input::KI_NEXT;
	KeyIdentifierMap[VK_END] = Rml::Core::Input::KI_END;
	KeyIdentifierMap[VK_HOME] = Rml::Core::Input::KI_HOME;
	KeyIdentifierMap[VK_LEFT] = Rml::Core::Input::KI_LEFT;
	KeyIdentifierMap[VK_UP] = Rml::Core::Input::KI_UP;
	KeyIdentifierMap[VK_RIGHT] = Rml::Core::Input::KI_RIGHT;
	KeyIdentifierMap[VK_DOWN] = Rml::Core::Input::KI_DOWN;
	KeyIdentifierMap[VK_SELECT] = Rml::Core::Input::KI_SELECT;
	KeyIdentifierMap[VK_PRINT] = Rml::Core::Input::KI_PRINT;
	KeyIdentifierMap[VK_EXECUTE] = Rml::Core::Input::KI_EXECUTE;
	KeyIdentifierMap[VK_SNAPSHOT] = Rml::Core::Input::KI_SNAPSHOT;
	KeyIdentifierMap[VK_INSERT] = Rml::Core::Input::KI_INSERT;
	KeyIdentifierMap[VK_DELETE] = Rml::Core::Input::KI_DELETE;
	KeyIdentifierMap[VK_HELP] = Rml::Core::Input::KI_HELP;

	KeyIdentifierMap[VK_LWIN] = Rml::Core::Input::KI_LWIN;
	KeyIdentifierMap[VK_RWIN] = Rml::Core::Input::KI_RWIN;
	KeyIdentifierMap[VK_APPS] = Rml::Core::Input::KI_APPS;

	KeyIdentifierMap[VK_SLEEP] = Rml::Core::Input::KI_SLEEP;

	KeyIdentifierMap[VK_NUMPAD0] = Rml::Core::Input::KI_NUMPAD0;
	KeyIdentifierMap[VK_NUMPAD1] = Rml::Core::Input::KI_NUMPAD1;
	KeyIdentifierMap[VK_NUMPAD2] = Rml::Core::Input::KI_NUMPAD2;
	KeyIdentifierMap[VK_NUMPAD3] = Rml::Core::Input::KI_NUMPAD3;
	KeyIdentifierMap[VK_NUMPAD4] = Rml::Core::Input::KI_NUMPAD4;
	KeyIdentifierMap[VK_NUMPAD5] = Rml::Core::Input::KI_NUMPAD5;
	KeyIdentifierMap[VK_NUMPAD6] = Rml::Core::Input::KI_NUMPAD6;
	KeyIdentifierMap[VK_NUMPAD7] = Rml::Core::Input::KI_NUMPAD7;
	KeyIdentifierMap[VK_NUMPAD8] = Rml::Core::Input::KI_NUMPAD8;
	KeyIdentifierMap[VK_NUMPAD9] = Rml::Core::Input::KI_NUMPAD9;
	KeyIdentifierMap[VK_MULTIPLY] = Rml::Core::Input::KI_MULTIPLY;
	KeyIdentifierMap[VK_ADD] = Rml::Core::Input::KI_ADD;
	KeyIdentifierMap[VK_SEPARATOR] = Rml::Core::Input::KI_SEPARATOR;
	KeyIdentifierMap[VK_SUBTRACT] = Rml::Core::Input::KI_SUBTRACT;
	KeyIdentifierMap[VK_DECIMAL] = Rml::Core::Input::KI_DECIMAL;
	KeyIdentifierMap[VK_DIVIDE] = Rml::Core::Input::KI_DIVIDE;
	KeyIdentifierMap[VK_F1] = Rml::Core::Input::KI_F1;
	KeyIdentifierMap[VK_F2] = Rml::Core::Input::KI_F2;
	KeyIdentifierMap[VK_F3] = Rml::Core::Input::KI_F3;
	KeyIdentifierMap[VK_F4] = Rml::Core::Input::KI_F4;
	KeyIdentifierMap[VK_F5] = Rml::Core::Input::KI_F5;
	KeyIdentifierMap[VK_F6] = Rml::Core::Input::KI_F6;
	KeyIdentifierMap[VK_F7] = Rml::Core::Input::KI_F7;
	KeyIdentifierMap[VK_F8] = Rml::Core::Input::KI_F8;
	KeyIdentifierMap[VK_F9] = Rml::Core::Input::KI_F9;
	KeyIdentifierMap[VK_F10] = Rml::Core::Input::KI_F10;
	KeyIdentifierMap[VK_F11] = Rml::Core::Input::KI_F11;
	KeyIdentifierMap[VK_F12] = Rml::Core::Input::KI_F12;
	KeyIdentifierMap[VK_F13] = Rml::Core::Input::KI_F13;
	KeyIdentifierMap[VK_F14] = Rml::Core::Input::KI_F14;
	KeyIdentifierMap[VK_F15] = Rml::Core::Input::KI_F15;
	KeyIdentifierMap[VK_F16] = Rml::Core::Input::KI_F16;
	KeyIdentifierMap[VK_F17] = Rml::Core::Input::KI_F17;
	KeyIdentifierMap[VK_F18] = Rml::Core::Input::KI_F18;
	KeyIdentifierMap[VK_F19] = Rml::Core::Input::KI_F19;
	KeyIdentifierMap[VK_F20] = Rml::Core::Input::KI_F20;
	KeyIdentifierMap[VK_F21] = Rml::Core::Input::KI_F21;
	KeyIdentifierMap[VK_F22] = Rml::Core::Input::KI_F22;
	KeyIdentifierMap[VK_F23] = Rml::Core::Input::KI_F23;
	KeyIdentifierMap[VK_F24] = Rml::Core::Input::KI_F24;

	KeyIdentifierMap[VK_NUMLOCK] = Rml::Core::Input::KI_NUMLOCK;
	KeyIdentifierMap[VK_SCROLL] = Rml::Core::Input::KI_SCROLL;

	KeyIdentifierMap[VK_OEM_NEC_EQUAL] = Rml::Core::Input::KI_OEM_NEC_EQUAL;

	KeyIdentifierMap[VK_OEM_FJ_JISHO] = Rml::Core::Input::KI_OEM_FJ_JISHO;
	KeyIdentifierMap[VK_OEM_FJ_MASSHOU] = Rml::Core::Input::KI_OEM_FJ_MASSHOU;
	KeyIdentifierMap[VK_OEM_FJ_TOUROKU] = Rml::Core::Input::KI_OEM_FJ_TOUROKU;
	KeyIdentifierMap[VK_OEM_FJ_LOYA] = Rml::Core::Input::KI_OEM_FJ_LOYA;
	KeyIdentifierMap[VK_OEM_FJ_ROYA] = Rml::Core::Input::KI_OEM_FJ_ROYA;

	KeyIdentifierMap[VK_SHIFT] = Rml::Core::Input::KI_LSHIFT;
	KeyIdentifierMap[VK_CONTROL] = Rml::Core::Input::KI_LCONTROL;
	KeyIdentifierMap[VK_MENU] = Rml::Core::Input::KI_LMENU;

	KeyIdentifierMap[VK_BROWSER_BACK] = Rml::Core::Input::KI_BROWSER_BACK;
	KeyIdentifierMap[VK_BROWSER_FORWARD] = Rml::Core::Input::KI_BROWSER_FORWARD;
	KeyIdentifierMap[VK_BROWSER_REFRESH] = Rml::Core::Input::KI_BROWSER_REFRESH;
	KeyIdentifierMap[VK_BROWSER_STOP] = Rml::Core::Input::KI_BROWSER_STOP;
	KeyIdentifierMap[VK_BROWSER_SEARCH] = Rml::Core::Input::KI_BROWSER_SEARCH;
	KeyIdentifierMap[VK_BROWSER_FAVORITES] = Rml::Core::Input::KI_BROWSER_FAVORITES;
	KeyIdentifierMap[VK_BROWSER_HOME] = Rml::Core::Input::KI_BROWSER_HOME;

	KeyIdentifierMap[VK_VOLUME_MUTE] = Rml::Core::Input::KI_VOLUME_MUTE;
	KeyIdentifierMap[VK_VOLUME_DOWN] = Rml::Core::Input::KI_VOLUME_DOWN;
	KeyIdentifierMap[VK_VOLUME_UP] = Rml::Core::Input::KI_VOLUME_UP;
	KeyIdentifierMap[VK_MEDIA_NEXT_TRACK] = Rml::Core::Input::KI_MEDIA_NEXT_TRACK;
	KeyIdentifierMap[VK_MEDIA_PREV_TRACK] = Rml::Core::Input::KI_MEDIA_PREV_TRACK;
	KeyIdentifierMap[VK_MEDIA_STOP] = Rml::Core::Input::KI_MEDIA_STOP;
	KeyIdentifierMap[VK_MEDIA_PLAY_PAUSE] = Rml::Core::Input::KI_MEDIA_PLAY_PAUSE;
	KeyIdentifierMap[VK_LAUNCH_MAIL] = Rml::Core::Input::KI_LAUNCH_MAIL;
	KeyIdentifierMap[VK_LAUNCH_MEDIA_SELECT] = Rml::Core::Input::KI_LAUNCH_MEDIA_SELECT;
	KeyIdentifierMap[VK_LAUNCH_APP1] = Rml::Core::Input::KI_LAUNCH_APP1;
	KeyIdentifierMap[VK_LAUNCH_APP2] = Rml::Core::Input::KI_LAUNCH_APP2;

	KeyIdentifierMap[VK_OEM_1] = Rml::Core::Input::KI_OEM_1;
	KeyIdentifierMap[VK_OEM_PLUS] = Rml::Core::Input::KI_OEM_PLUS;
	KeyIdentifierMap[VK_OEM_COMMA] = Rml::Core::Input::KI_OEM_COMMA;
	KeyIdentifierMap[VK_OEM_MINUS] = Rml::Core::Input::KI_OEM_MINUS;
	KeyIdentifierMap[VK_OEM_PERIOD] = Rml::Core::Input::KI_OEM_PERIOD;
	KeyIdentifierMap[VK_OEM_2] = Rml::Core::Input::KI_OEM_2;
	KeyIdentifierMap[VK_OEM_3] = Rml::Core::Input::KI_OEM_3;

	KeyIdentifierMap[VK_OEM_4] = Rml::Core::Input::KI_OEM_4;
	KeyIdentifierMap[VK_OEM_5] = Rml::Core::Input::KI_OEM_5;
	KeyIdentifierMap[VK_OEM_6] = Rml::Core::Input::KI_OEM_6;
	KeyIdentifierMap[VK_OEM_7] = Rml::Core::Input::KI_OEM_7;
	KeyIdentifierMap[VK_OEM_8] = Rml::Core::Input::KI_OEM_8;

	KeyIdentifierMap[VK_OEM_AX] = Rml::Core::Input::KI_OEM_AX;
	KeyIdentifierMap[VK_OEM_102] = Rml::Core::Input::KI_OEM_102;
	KeyIdentifierMap[VK_ICO_HELP] = Rml::Core::Input::KI_ICO_HELP;
	KeyIdentifierMap[VK_ICO_00] = Rml::Core::Input::KI_ICO_00;

	KeyIdentifierMap[VK_PROCESSKEY] = Rml::Core::Input::KI_PROCESSKEY;

	KeyIdentifierMap[VK_ICO_CLEAR] = Rml::Core::Input::KI_ICO_CLEAR;

	KeyIdentifierMap[VK_ATTN] = Rml::Core::Input::KI_ATTN;
	KeyIdentifierMap[VK_CRSEL] = Rml::Core::Input::KI_CRSEL;
	KeyIdentifierMap[VK_EXSEL] = Rml::Core::Input::KI_EXSEL;
	KeyIdentifierMap[VK_EREOF] = Rml::Core::Input::KI_EREOF;
	KeyIdentifierMap[VK_PLAY] = Rml::Core::Input::KI_PLAY;
	KeyIdentifierMap[VK_ZOOM] = Rml::Core::Input::KI_ZOOM;
	KeyIdentifierMap[VK_PA1] = Rml::Core::Input::KI_PA1;
	KeyIdentifierMap[VK_OEM_CLEAR] = Rml::Core::Input::KI_OEM_CLEAR;
}
