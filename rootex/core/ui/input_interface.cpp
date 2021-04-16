#include "input_interface.h"

#include "event_manager.h"

#undef interface
#include "RmlUi/Debugger.h"
#define interface __STRUCT__

static int GetKeyModifierState();
static void InitialiseKeymap();

#define KEYMAP_SIZE 256
static Rml::Input::KeyIdentifier KeyIdentifierMap[KEYMAP_SIZE];

InputInterface::InputInterface()
    : m_Binder(this)
{
	m_Binder.bind(RootexEvents::WindowResized, &InputInterface::WindowResized);
}

InputInterface* InputInterface::GetSingleton()
{
	static InputInterface singleton;
	return &singleton;
}

bool InputInterface::Initialise()
{
	m_Left = 0;
	m_Right = OS::GetDisplayWidth() + m_Left;
	m_Top = 0;
	m_Bottom = OS::GetDisplayHeight() + m_Top;

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

// Sets the context to send input events to.
void InputInterface::SetContext(Rml::Context* context)
{
	m_Context = context;
}

// Returns the character code for a key identifer / key modifier combination.
Rml::Character InputInterface::GetCharacterCode(Rml::Input::KeyIdentifier keyIdentifier, int keyModifierState)
{
	using Rml::Character;

	// Check if we have a keycode capable of generating characters on the main keyboard (ie, not on the numeric
	// keypad; that is dealt with below).
	if (keyIdentifier <= Rml::Input::KI_OEM_102)
	{
		// Get modifier states
		bool shift = (keyModifierState & Rml::Input::KM_SHIFT) > 0;
		bool capslock = (keyModifierState & Rml::Input::KM_CAPSLOCK) > 0;

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
	else if (keyIdentifier <= Rml::Input::KI_OEM_NEC_EQUAL)
	{
		if (keyModifierState & Rml::Input::KM_NUMLOCK)
			return (Character)KeypadMap[0][keyIdentifier - Rml::Input::KI_NUMPAD0];
		else
			return (Character)KeypadMap[1][keyIdentifier - Rml::Input::KI_NUMPAD0];
	}

	else if (keyIdentifier == Rml::Input::KI_RETURN)
		return (Character)'\n';

	return Character::Null;
}

Variant InputInterface::WindowResized(const Event* event)
{
	Vector2 size = Extract<Vector2>(event->getData());
	m_Left = 0;
	m_Top = 0;
	m_Right = size.x;
	m_Bottom = size.y;

	return true;
}

void InputInterface::ProcessWindowsEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_Context == nullptr)
	{
		return;
	}

	if (message == WM_MOUSEMOVE)
	{
		const int& x = LOWORD(lParam);
		const int& y = HIWORD(lParam);

		m_IsMouseOver = (x >= m_Left && x < m_Right) && (y >= m_Top && y <= m_Bottom);
		if (m_IsMouseOver)
		{
			m_Context->ProcessMouseMove((x - m_Left) * m_ScaleX, (y - m_Top) * m_ScaleY, GetKeyModifierState());
		}
	}

	if (!m_IsMouseOver || !m_IsEnabled)
	{
		return;
	}

	switch (message)
	{
	case WM_LBUTTONDOWN:
		m_Context->ProcessMouseButtonDown(0, GetKeyModifierState());
		break;

	case WM_LBUTTONUP:
		m_Context->ProcessMouseButtonUp(0, GetKeyModifierState());
		break;

	case WM_RBUTTONDOWN:
		m_Context->ProcessMouseButtonDown(1, GetKeyModifierState());
		break;

	case WM_RBUTTONUP:
		m_Context->ProcessMouseButtonUp(1, GetKeyModifierState());
		break;

	case WM_MBUTTONDOWN:
		m_Context->ProcessMouseButtonDown(2, GetKeyModifierState());
		break;

	case WM_MBUTTONUP:
		m_Context->ProcessMouseButtonUp(2, GetKeyModifierState());
		break;

	case WM_MOUSEWHEEL:
		m_Context->ProcessMouseWheel(static_cast<float>((short)HIWORD(wParam)) / static_cast<float>(-WHEEL_DELTA), GetKeyModifierState());
		break;

	case WM_KEYDOWN:
	{
		Rml::Input::KeyIdentifier key_identifier = KeyIdentifierMap[wParam];
		int keyModifierState = GetKeyModifierState();

		// Check for F8 to toggle the debugger.
		if (key_identifier == Rml::Input::KI_F8)
		{
			Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
			break;
		}

		m_Context->ProcessKeyDown(key_identifier, keyModifierState);
	}
	break;

	case WM_CHAR:
	{
		static char16_t firstU16CodeUnit = 0;

		char16_t c = (char16_t)wParam;
		Rml::Character character = (Rml::Character)c;

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
				Rml::String utf8 = Rml::StringUtilities::ToUTF8({ firstU16CodeUnit, c });
				character = Rml::StringUtilities::ToCharacter(utf8.data());
			}
			else if (c == '\r')
			{
				// Windows sends new-lines as carriage returns, convert to endlines.
				character = (Rml::Character)'\n';
			}

			firstU16CodeUnit = 0;

			// Only send through printable characters.
			if (((char32_t)character >= 32 || character == (Rml::Character)'\n') && character != (Rml::Character)127)
				m_Context->ProcessTextInput(character);
		}
	}
	break;

	case WM_KEYUP:
		m_Context->ProcessKeyUp(KeyIdentifierMap[wParam], GetKeyModifierState());
		break;
	}
}

static int GetKeyModifierState()
{
	int key_modifier_state = 0;

	// Query the state of all modifier keys
	if (GetKeyState(VK_CAPITAL) & 1)
	{
		key_modifier_state |= Rml::Input::KM_CAPSLOCK;
	}

	if (HIWORD(GetKeyState(VK_SHIFT)) & 1)
	{
		key_modifier_state |= Rml::Input::KM_SHIFT;
	}

	if (GetKeyState(VK_NUMLOCK) & 1)
	{
		key_modifier_state |= Rml::Input::KM_NUMLOCK;
	}

	if (HIWORD(GetKeyState(VK_CONTROL)) & 1)
	{
		key_modifier_state |= Rml::Input::KM_CTRL;
	}

	if (HIWORD(GetKeyState(VK_MENU)) & 1)
	{
		key_modifier_state |= Rml::Input::KM_ALT;
	}

	return key_modifier_state;
}

static void InitialiseKeymap()
{
	// Initialise the key map with default values.
	memset(KeyIdentifierMap, 0, sizeof(KeyIdentifierMap));

	// Assign individual values.
	KeyIdentifierMap['A'] = Rml::Input::KI_A;
	KeyIdentifierMap['B'] = Rml::Input::KI_B;
	KeyIdentifierMap['C'] = Rml::Input::KI_C;
	KeyIdentifierMap['D'] = Rml::Input::KI_D;
	KeyIdentifierMap['E'] = Rml::Input::KI_E;
	KeyIdentifierMap['F'] = Rml::Input::KI_F;
	KeyIdentifierMap['G'] = Rml::Input::KI_G;
	KeyIdentifierMap['H'] = Rml::Input::KI_H;
	KeyIdentifierMap['I'] = Rml::Input::KI_I;
	KeyIdentifierMap['J'] = Rml::Input::KI_J;
	KeyIdentifierMap['K'] = Rml::Input::KI_K;
	KeyIdentifierMap['L'] = Rml::Input::KI_L;
	KeyIdentifierMap['M'] = Rml::Input::KI_M;
	KeyIdentifierMap['N'] = Rml::Input::KI_N;
	KeyIdentifierMap['O'] = Rml::Input::KI_O;
	KeyIdentifierMap['P'] = Rml::Input::KI_P;
	KeyIdentifierMap['Q'] = Rml::Input::KI_Q;
	KeyIdentifierMap['R'] = Rml::Input::KI_R;
	KeyIdentifierMap['S'] = Rml::Input::KI_S;
	KeyIdentifierMap['T'] = Rml::Input::KI_T;
	KeyIdentifierMap['U'] = Rml::Input::KI_U;
	KeyIdentifierMap['V'] = Rml::Input::KI_V;
	KeyIdentifierMap['W'] = Rml::Input::KI_W;
	KeyIdentifierMap['X'] = Rml::Input::KI_X;
	KeyIdentifierMap['Y'] = Rml::Input::KI_Y;
	KeyIdentifierMap['Z'] = Rml::Input::KI_Z;

	KeyIdentifierMap['0'] = Rml::Input::KI_0;
	KeyIdentifierMap['1'] = Rml::Input::KI_1;
	KeyIdentifierMap['2'] = Rml::Input::KI_2;
	KeyIdentifierMap['3'] = Rml::Input::KI_3;
	KeyIdentifierMap['4'] = Rml::Input::KI_4;
	KeyIdentifierMap['5'] = Rml::Input::KI_5;
	KeyIdentifierMap['6'] = Rml::Input::KI_6;
	KeyIdentifierMap['7'] = Rml::Input::KI_7;
	KeyIdentifierMap['8'] = Rml::Input::KI_8;
	KeyIdentifierMap['9'] = Rml::Input::KI_9;

	KeyIdentifierMap[VK_BACK] = Rml::Input::KI_BACK;
	KeyIdentifierMap[VK_TAB] = Rml::Input::KI_TAB;

	KeyIdentifierMap[VK_CLEAR] = Rml::Input::KI_CLEAR;
	KeyIdentifierMap[VK_RETURN] = Rml::Input::KI_RETURN;

	KeyIdentifierMap[VK_PAUSE] = Rml::Input::KI_PAUSE;
	KeyIdentifierMap[VK_CAPITAL] = Rml::Input::KI_CAPITAL;

	KeyIdentifierMap[VK_KANA] = Rml::Input::KI_KANA;
	KeyIdentifierMap[VK_HANGUL] = Rml::Input::KI_HANGUL;
	KeyIdentifierMap[VK_JUNJA] = Rml::Input::KI_JUNJA;
	KeyIdentifierMap[VK_FINAL] = Rml::Input::KI_FINAL;
	KeyIdentifierMap[VK_HANJA] = Rml::Input::KI_HANJA;
	KeyIdentifierMap[VK_KANJI] = Rml::Input::KI_KANJI;

	KeyIdentifierMap[VK_ESCAPE] = Rml::Input::KI_ESCAPE;

	KeyIdentifierMap[VK_CONVERT] = Rml::Input::KI_CONVERT;
	KeyIdentifierMap[VK_NONCONVERT] = Rml::Input::KI_NONCONVERT;
	KeyIdentifierMap[VK_ACCEPT] = Rml::Input::KI_ACCEPT;
	KeyIdentifierMap[VK_MODECHANGE] = Rml::Input::KI_MODECHANGE;

	KeyIdentifierMap[VK_SPACE] = Rml::Input::KI_SPACE;
	KeyIdentifierMap[VK_PRIOR] = Rml::Input::KI_PRIOR;
	KeyIdentifierMap[VK_NEXT] = Rml::Input::KI_NEXT;
	KeyIdentifierMap[VK_END] = Rml::Input::KI_END;
	KeyIdentifierMap[VK_HOME] = Rml::Input::KI_HOME;
	KeyIdentifierMap[VK_LEFT] = Rml::Input::KI_LEFT;
	KeyIdentifierMap[VK_UP] = Rml::Input::KI_UP;
	KeyIdentifierMap[VK_RIGHT] = Rml::Input::KI_RIGHT;
	KeyIdentifierMap[VK_DOWN] = Rml::Input::KI_DOWN;
	KeyIdentifierMap[VK_SELECT] = Rml::Input::KI_SELECT;
	KeyIdentifierMap[VK_PRINT] = Rml::Input::KI_PRINT;
	KeyIdentifierMap[VK_EXECUTE] = Rml::Input::KI_EXECUTE;
	KeyIdentifierMap[VK_SNAPSHOT] = Rml::Input::KI_SNAPSHOT;
	KeyIdentifierMap[VK_INSERT] = Rml::Input::KI_INSERT;
	KeyIdentifierMap[VK_DELETE] = Rml::Input::KI_DELETE;
	KeyIdentifierMap[VK_HELP] = Rml::Input::KI_HELP;

	KeyIdentifierMap[VK_LWIN] = Rml::Input::KI_LWIN;
	KeyIdentifierMap[VK_RWIN] = Rml::Input::KI_RWIN;
	KeyIdentifierMap[VK_APPS] = Rml::Input::KI_APPS;

	KeyIdentifierMap[VK_SLEEP] = Rml::Input::KI_SLEEP;

	KeyIdentifierMap[VK_NUMPAD0] = Rml::Input::KI_NUMPAD0;
	KeyIdentifierMap[VK_NUMPAD1] = Rml::Input::KI_NUMPAD1;
	KeyIdentifierMap[VK_NUMPAD2] = Rml::Input::KI_NUMPAD2;
	KeyIdentifierMap[VK_NUMPAD3] = Rml::Input::KI_NUMPAD3;
	KeyIdentifierMap[VK_NUMPAD4] = Rml::Input::KI_NUMPAD4;
	KeyIdentifierMap[VK_NUMPAD5] = Rml::Input::KI_NUMPAD5;
	KeyIdentifierMap[VK_NUMPAD6] = Rml::Input::KI_NUMPAD6;
	KeyIdentifierMap[VK_NUMPAD7] = Rml::Input::KI_NUMPAD7;
	KeyIdentifierMap[VK_NUMPAD8] = Rml::Input::KI_NUMPAD8;
	KeyIdentifierMap[VK_NUMPAD9] = Rml::Input::KI_NUMPAD9;
	KeyIdentifierMap[VK_MULTIPLY] = Rml::Input::KI_MULTIPLY;
	KeyIdentifierMap[VK_ADD] = Rml::Input::KI_ADD;
	KeyIdentifierMap[VK_SEPARATOR] = Rml::Input::KI_SEPARATOR;
	KeyIdentifierMap[VK_SUBTRACT] = Rml::Input::KI_SUBTRACT;
	KeyIdentifierMap[VK_DECIMAL] = Rml::Input::KI_DECIMAL;
	KeyIdentifierMap[VK_DIVIDE] = Rml::Input::KI_DIVIDE;
	KeyIdentifierMap[VK_F1] = Rml::Input::KI_F1;
	KeyIdentifierMap[VK_F2] = Rml::Input::KI_F2;
	KeyIdentifierMap[VK_F3] = Rml::Input::KI_F3;
	KeyIdentifierMap[VK_F4] = Rml::Input::KI_F4;
	KeyIdentifierMap[VK_F5] = Rml::Input::KI_F5;
	KeyIdentifierMap[VK_F6] = Rml::Input::KI_F6;
	KeyIdentifierMap[VK_F7] = Rml::Input::KI_F7;
	KeyIdentifierMap[VK_F8] = Rml::Input::KI_F8;
	KeyIdentifierMap[VK_F9] = Rml::Input::KI_F9;
	KeyIdentifierMap[VK_F10] = Rml::Input::KI_F10;
	KeyIdentifierMap[VK_F11] = Rml::Input::KI_F11;
	KeyIdentifierMap[VK_F12] = Rml::Input::KI_F12;
	KeyIdentifierMap[VK_F13] = Rml::Input::KI_F13;
	KeyIdentifierMap[VK_F14] = Rml::Input::KI_F14;
	KeyIdentifierMap[VK_F15] = Rml::Input::KI_F15;
	KeyIdentifierMap[VK_F16] = Rml::Input::KI_F16;
	KeyIdentifierMap[VK_F17] = Rml::Input::KI_F17;
	KeyIdentifierMap[VK_F18] = Rml::Input::KI_F18;
	KeyIdentifierMap[VK_F19] = Rml::Input::KI_F19;
	KeyIdentifierMap[VK_F20] = Rml::Input::KI_F20;
	KeyIdentifierMap[VK_F21] = Rml::Input::KI_F21;
	KeyIdentifierMap[VK_F22] = Rml::Input::KI_F22;
	KeyIdentifierMap[VK_F23] = Rml::Input::KI_F23;
	KeyIdentifierMap[VK_F24] = Rml::Input::KI_F24;

	KeyIdentifierMap[VK_NUMLOCK] = Rml::Input::KI_NUMLOCK;
	KeyIdentifierMap[VK_SCROLL] = Rml::Input::KI_SCROLL;

	KeyIdentifierMap[VK_OEM_NEC_EQUAL] = Rml::Input::KI_OEM_NEC_EQUAL;

	KeyIdentifierMap[VK_OEM_FJ_JISHO] = Rml::Input::KI_OEM_FJ_JISHO;
	KeyIdentifierMap[VK_OEM_FJ_MASSHOU] = Rml::Input::KI_OEM_FJ_MASSHOU;
	KeyIdentifierMap[VK_OEM_FJ_TOUROKU] = Rml::Input::KI_OEM_FJ_TOUROKU;
	KeyIdentifierMap[VK_OEM_FJ_LOYA] = Rml::Input::KI_OEM_FJ_LOYA;
	KeyIdentifierMap[VK_OEM_FJ_ROYA] = Rml::Input::KI_OEM_FJ_ROYA;

	KeyIdentifierMap[VK_SHIFT] = Rml::Input::KI_LSHIFT;
	KeyIdentifierMap[VK_CONTROL] = Rml::Input::KI_LCONTROL;
	KeyIdentifierMap[VK_MENU] = Rml::Input::KI_LMENU;

	KeyIdentifierMap[VK_BROWSER_BACK] = Rml::Input::KI_BROWSER_BACK;
	KeyIdentifierMap[VK_BROWSER_FORWARD] = Rml::Input::KI_BROWSER_FORWARD;
	KeyIdentifierMap[VK_BROWSER_REFRESH] = Rml::Input::KI_BROWSER_REFRESH;
	KeyIdentifierMap[VK_BROWSER_STOP] = Rml::Input::KI_BROWSER_STOP;
	KeyIdentifierMap[VK_BROWSER_SEARCH] = Rml::Input::KI_BROWSER_SEARCH;
	KeyIdentifierMap[VK_BROWSER_FAVORITES] = Rml::Input::KI_BROWSER_FAVORITES;
	KeyIdentifierMap[VK_BROWSER_HOME] = Rml::Input::KI_BROWSER_HOME;

	KeyIdentifierMap[VK_VOLUME_MUTE] = Rml::Input::KI_VOLUME_MUTE;
	KeyIdentifierMap[VK_VOLUME_DOWN] = Rml::Input::KI_VOLUME_DOWN;
	KeyIdentifierMap[VK_VOLUME_UP] = Rml::Input::KI_VOLUME_UP;
	KeyIdentifierMap[VK_MEDIA_NEXT_TRACK] = Rml::Input::KI_MEDIA_NEXT_TRACK;
	KeyIdentifierMap[VK_MEDIA_PREV_TRACK] = Rml::Input::KI_MEDIA_PREV_TRACK;
	KeyIdentifierMap[VK_MEDIA_STOP] = Rml::Input::KI_MEDIA_STOP;
	KeyIdentifierMap[VK_MEDIA_PLAY_PAUSE] = Rml::Input::KI_MEDIA_PLAY_PAUSE;
	KeyIdentifierMap[VK_LAUNCH_MAIL] = Rml::Input::KI_LAUNCH_MAIL;
	KeyIdentifierMap[VK_LAUNCH_MEDIA_SELECT] = Rml::Input::KI_LAUNCH_MEDIA_SELECT;
	KeyIdentifierMap[VK_LAUNCH_APP1] = Rml::Input::KI_LAUNCH_APP1;
	KeyIdentifierMap[VK_LAUNCH_APP2] = Rml::Input::KI_LAUNCH_APP2;

	KeyIdentifierMap[VK_OEM_1] = Rml::Input::KI_OEM_1;
	KeyIdentifierMap[VK_OEM_PLUS] = Rml::Input::KI_OEM_PLUS;
	KeyIdentifierMap[VK_OEM_COMMA] = Rml::Input::KI_OEM_COMMA;
	KeyIdentifierMap[VK_OEM_MINUS] = Rml::Input::KI_OEM_MINUS;
	KeyIdentifierMap[VK_OEM_PERIOD] = Rml::Input::KI_OEM_PERIOD;
	KeyIdentifierMap[VK_OEM_2] = Rml::Input::KI_OEM_2;
	KeyIdentifierMap[VK_OEM_3] = Rml::Input::KI_OEM_3;

	KeyIdentifierMap[VK_OEM_4] = Rml::Input::KI_OEM_4;
	KeyIdentifierMap[VK_OEM_5] = Rml::Input::KI_OEM_5;
	KeyIdentifierMap[VK_OEM_6] = Rml::Input::KI_OEM_6;
	KeyIdentifierMap[VK_OEM_7] = Rml::Input::KI_OEM_7;
	KeyIdentifierMap[VK_OEM_8] = Rml::Input::KI_OEM_8;

	KeyIdentifierMap[VK_OEM_AX] = Rml::Input::KI_OEM_AX;
	KeyIdentifierMap[VK_OEM_102] = Rml::Input::KI_OEM_102;
	KeyIdentifierMap[VK_ICO_HELP] = Rml::Input::KI_ICO_HELP;
	KeyIdentifierMap[VK_ICO_00] = Rml::Input::KI_ICO_00;

	KeyIdentifierMap[VK_PROCESSKEY] = Rml::Input::KI_PROCESSKEY;

	KeyIdentifierMap[VK_ICO_CLEAR] = Rml::Input::KI_ICO_CLEAR;

	KeyIdentifierMap[VK_ATTN] = Rml::Input::KI_ATTN;
	KeyIdentifierMap[VK_CRSEL] = Rml::Input::KI_CRSEL;
	KeyIdentifierMap[VK_EXSEL] = Rml::Input::KI_EXSEL;
	KeyIdentifierMap[VK_EREOF] = Rml::Input::KI_EREOF;
	KeyIdentifierMap[VK_PLAY] = Rml::Input::KI_PLAY;
	KeyIdentifierMap[VK_ZOOM] = Rml::Input::KI_ZOOM;
	KeyIdentifierMap[VK_PA1] = Rml::Input::KI_PA1;
	KeyIdentifierMap[VK_OEM_CLEAR] = Rml::Input::KI_OEM_CLEAR;
}
