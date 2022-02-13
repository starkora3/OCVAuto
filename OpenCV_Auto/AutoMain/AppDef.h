#pragma once

namespace OCVAUTO {


	enum {
		RET_TIMEOUT = -3,  // ���Z�b�g�����^�C���A�E�g
		RET_UNKNOWN = -2,	// �G���[
		RET_ERR = -1,			// �G���[
		RET_OK = 0,				// ��������
		RET_NG,					// �������s
		RET_WAIT,				// �J�n�҂�
		RET_RESET,				// ���Z�b�g�v��


		TYPE_NONE = 0,
		TYPE_END,										// 1, �I������
		TYPE_SNAPSHOT,									// 2, �X�i�b�v�V���b�g
		TYPE_CLICK,										// 3, �N���b�N
		TYPE_DCLICK,									// 4, �_�u���N���b�N

		TYPE_SHELL = 10,								// 10, �V�F���R�}���h���s
		TYPE_ACC_GET ,									// 11, �A�J�E���g�擾
		TYPE_ACC_NEXT ,									// 12, �A�J�E���g����
		TYPE_ACC_INPUT,									// 13, �A�J�E���g����

		TYPE_NOX_CMD = 15,								// 15, Nox�R�}���h���s


		TYPE_SEARCH_BORDER_EXIST		=21,			// 21, �E�B���h�E����Ώۉ摜(����)��T���āA�{�[�_�[�ȏォ�𔻒�A�����OK
		TYPE_SEARCH_BORDER_NONE		,					// 22, �E�B���h�E����Ώۉ摜(����)��T���āA�{�[�_�[�ȏォ�𔻒�A�Ȃ����OK
		TYPE_SEARCH_BORDER_CLICK_OK	,					// 23, �E�B���h�E����Ώۉ摜(����)��T���āA�{�[�_�[�ȏ�Ȃ�cn�Ԗڂ��N���b�N ����OK ��Ԃ�
		TYPE_SEARCH_BORDER_CLICK_NG,					// 24, �E�B���h�E����Ώۉ摜(����)��T���āA�{�[�_�[�ȏ�Ȃ�cn�Ԗڂ��N���b�N ����NG ��Ԃ�
		TYPE_SEARCH_BORDER_CLICK2		,				// 25, �E�B���h�E����Ώۉ摜(����)��T���āA�{�[�_�[�ȏ�Ȃ� �N���b�N ���T�u���ڂ��N���b�N

		TYPE_DRUG_P2P = 31,								// 31, �ʒu�ԃh���b�O


		TYPE_CHILD_SEARCH_CLICK_V=81,					// 81, �q�E�B���h�E����Ώۉ摜��T���āA�cn�Ԗڂ��N���b�N
		TYPE_CHILD_SEARCH_NONE,							// 82, �q�E�B���h�E����Ώۉ摜��T���āA�Ȃ����OK
		TYPE_CHILD_SEARCH_EXIST,						// 83, �q�E�B���h�E����Ώۉ摜��T���āA�����OK

		TYPE_MOVE_L = 101,								// 101, ���ړ�
		TYPE_MOVE_R,									// 102, �E�ړ�
		TYPE_MOVE_U,									// 103, ��ړ�
		TYPE_MOVE_D,									// 104, ���ړ�

		TYPE_MOVE_LU,									// 105, ����ړ�
		TYPE_MOVE_LD,									// 106, �����ړ�
		TYPE_MOVE_RU,									// 107, �E��ړ�
		TYPE_MOVE_RD,									// 108, �E���ړ�

		TYPE_KEY_DOWNUP = 201,							// 201, �L�[����
		TYPE_KEY_DOWN,									// 202, �L�[��
		TYPE_KEY_UP,									// 203, �L�[��

		TYPE_MEMORY_CHECK = 501,						// 501, �������g�p�ʃ`�F�b�N
		TYPE_STATE_SEND,								// 502, ��ԑ��M 

		TYPE_ADB_PS_START = 800,						// 800 adb �v���Z�X �N��
		TYPE_ADB_PS_CHECK = 801,						// 801 adb �v���Z�X �m�F
		TYPE_ADB_PS_KILL = 802,							// 802 adb �v���Z�X ��~

		TYPE_TASK_KILL_W=900,							// 900 ���C�h��ʂ̃^�X�N�L��
		TYPE_TASK_KILL  = 901,							// 901 �^�X�N�L��
		TYPE_KEY_HOME,									// 902 Home �L�[
		TYPE_KEY_PSW,									// 903 �p�X���[�h���� �L�[
		TYPE_KEY_INPUT,									// 904 ���� �L�[

		TYPE_PROCESS_KILL_SERVICE = 998,				// 998 �v���Z�X �L�� �T�[�r�X
		TYPE_PROCESS_KILL = 999,						// 999 �v���Z�X �L��


		TYPE_TASK_SERVICE_ENABLE = 1000,				//!< �T�[�r�X�L����
		TYPE_TASK_SERVICE_DISABLE,						//!< �T�[�r�X�L�����Z��

		TYPE_ERROR = 9999,
	};

	enum
	{
		VKD_LBUTTON = 0x01,
		VKD_RBUTTON = 0x02,
		VKD_CANCEL = 0x03,
		VKD_MBUTTON = 0x04,
		VKD_XBUTTON1 = 0x05,
		VKD_XBUTTON2 = 0x06,
		VKD_BACK = 0x08,
		VKD_TAB = 0x09,
		VKD_CLEAR = 0x0C,
		VKD_RETURN = 0x0D,
		VKD_SHIFT = 0x10,
		VKD_CONTROL = 0x11,
		VKD_ALT = 0x12,
		VKD_PAUSE = 0x13,
		VKD_CAPITAL = 0x14,
		VKD_KANA = 0x15,
		VKD_JUNJA = 0x17,
		VKD_FINAL = 0x18,
		VKD_KANJI = 0x19,
		VKD_ESCAPE = 0x1B,
		VKD_CONVERT = 0x1C,
		VKD_NONCONVERT = 0x1D,
		VKD_ACCEPT = 0x1E,
		VKD_MODECHANGE = 0x1F,
		VKD_SPACE = 0x20,
		VKD_PRIOR = 0x21,// PgUp	 
		VKD_NEXT = 0x22,// PgDn	 
		VKD_END = 0x23,// End	 
		VKD_HOME = 0x24,// Home 	 
		VKD_LEFT = 0x25,// ��	 
		VKD_UP = 0x26,// �� 	 
		VKD_RIGHT = 0x27,// �� 	 
		VKD_DOWN = 0x28,// �� 	 
		VKD_SELECT = 0x29,
		VKD_PRINT = 0x2A,
		VKD_EXECUTE = 0x2B,
		VKD_SNAPSHOT = 0x2C,
		VKD_INSERT = 0x2D,// Insert 	 
		VKD_DELETE = 0x2E,// Delete 	 
		VKD_HELP = 0x2F,
		VKD_0 = 0x30,
		VKD_1 = 0x31,
		VKD_2 = 0x32,
		VKD_3 = 0x33,
		VKD_4 = 0x34,
		VKD_5 = 0x35,
		VKD_6 = 0x36,
		VKD_7 = 0x37,
		VKD_8 = 0x38,
		VKD_9 = 0x39,
		VKD_A = 0x41,
		VKD_B = 0x42,
		VKD_C = 0x43,
		VKD_D = 0x44,
		VKD_E = 0x45,
		VKD_F = 0x46,
		VKD_G = 0x47,
		VKD_H = 0x48,
		VKD_I = 0x49,
		VKD_J = 0x4A,
		VKD_K = 0x4B,
		VKD_L = 0x4C,
		VKD_M = 0x4D,
		VKD_N = 0x4E,
		VKD_O = 0x4F,
		VKD_P = 0x50,
		VKD_Q = 0x51,
		VKD_R = 0x52,
		VKD_S = 0x53,
		VKD_T = 0x54,
		VKD_U = 0x55,
		VKD_V = 0x56,
		VKD_W = 0x57,
		VKD_X = 0x58,
		VKD_Y = 0x59,
		VKD_Z = 0x5A,
		VKD_LWIN = 0x5B,
		VKD_RWIN = 0x5C,
		VKD_APPS = 0x5D,	// �A�v���P�[�V�����L�[	�|�b�v�A�b�v���j���[���J���L�[
		VKD_SLEEP = 0x5F,
		VKD_NUMPAD0 = 0x60, 	//�e���L�[               
		VKD_NUMPAD1 = 0x61,
		VKD_NUMPAD2 = 0x62,
		VKD_NUMPAD3 = 0x63,
		VKD_NUMPAD4 = 0x64,
		VKD_NUMPAD5 = 0x65,
		VKD_NUMPAD6 = 0x66,
		VKD_NUMPAD7 = 0x67,
		VKD_NUMPAD8 = 0x68,
		VKD_NUMPAD9 = 0x69,
		VKD_MULTIPLY = 0x6A,	// *
		VKD_ADD = 0x6B,	// +
		VKD_SEPARATOR = 0x6C,// ,
		VKD_SUBTRACT = 0x6D,// -
		VKD_DECIMAL = 0x6E,// .
		VKD_DIVIDE = 0x6F,// /
		VKD_F1 = 0x70,
		VKD_F2 = 0x71,
		VKD_F3 = 0x72,
		VKD_F4 = 0x73,
		VKD_F5 = 0x74,
		VKD_F6 = 0x75,
		VKD_F7 = 0x76,
		VKD_F8 = 0x77,
		VKD_F9 = 0x78,
		VKD_F10 = 0x79,
		VKD_F11 = 0x7A,
		VKD_F12 = 0x7B,
		VKD_F13 = 0x7C,
		VKD_F14 = 0x7D,
		VKD_F15 = 0x7E,
		VKD_F16 = 0x7F,
		VKD_F17 = 0x80,
		VKD_F18 = 0x81,
		VKD_F19 = 0x82,
		VKD_F20 = 0x83,
		VKD_F21 = 0x84,
		VKD_F22 = 0x85,
		VKD_F23 = 0x86,
		VKD_F24 = 0x87,
		VKD_NUMLOCK = 0x90,// Num Lock	 
		VKD_SCROLL = 0x91,// Scroll Lock	 
		VKD_LSHIFT = 0xA0,// 
		VKD_RSHIFT = 0xA1,// 
		VKD_LCONTROL = 0xA2,//  
		VKD_RCONTROL = 0xA3,// 
		VKD_LMENU = 0xA4,// 
		VKD_RMENU = 0xA5,// 
		VKD_BROWSER_BACK = 0xA6,// �u���E�U�[	�߂�
		VKD_BROWSER_FORWARD = 0xA7,// �i��
		VKD_BROWSER_REFRESH = 0xA8,// �X�V
		VKD_BROWSER_STOP = 0xA9,// ���~
		VKD_BROWSER_SEARCH = 0xAA,// �����E�B���h�E�̕\���E��\��
		VKD_BROWSER_FAVORITES = 0xAB,//	���C�ɓ���E�B���h�E�̕\���E��\��
		VKD_BROWSER_HOME = 0xAC,// �z�[��
		VKD_VOLUME_MUTE = 0xAD,
		VKD_VOLUME_DOWN = 0xAE,
		VKD_VOLUME_UP = 0xAF,
		VKD_MEDIA_NEXT_TRACK = 0xB0,
		VKD_MEDIA_PREV_TRACK = 0xB1,
		VKD_MEDIA_STOP = 0xB2,
		VKD_MEDIA_PLAY_PAUSE = 0xB3,
		VKD_LAUNCH_MAIL = 0xB4,// ���[���[�N��
		VKD_LAUNCH_MEDIA_SELECT = 0xB5,
		VKD_LAUNCH_APP1 = 0xB6,
		VKD_LAUNCH_APP2 = 0xB7,
		VKD_OEM_1 = 0xBA,// :	 
		VKD_OEM_PLUS = 0xBB,// ;	 
		VKD_OEM_COMMA = 0xBC,// ,	 
		VKD_OEM_MINUS = 0xBD,// -^	 
		VKD_OEM_PERIOD = 0xBE,// .	 
		VKD_OEM_2 = 0xBF,// /	 
		VKD_OEM_3 = 0xC0,// @	 
		VKD_OEM_4 = 0xDB,// [	 
		VKD_OEM_5 = 0xDC,// \	 
		VKD_OEM_6 = 0xDD,// ]	 
		VKD_OEM_7 = 0xDE,// ^	 
		VKD_OEM_8 = 0xDF,
		VKD_OEM_AX = 0xE1,
		VKD_OEM_102 = 0xE2,// \(�o�b�N�X���b�V���j	 
		VKD_ICO_HELP = 0xE3,
		VKD_ICO_00 = 0xE4,
		VKD_PROCESSKEY = 0xE5,// IME��ON�̎��A�ϊ��Ɋւ�邷�ׂẴL�[�Ŕ���
		VKD_ICO_CLEAR = 0xE6,
		VKD_PACKET = 0xE7,// 
		VKD_OEM_RESET = 0xE9,//
		VKD_OEM_JUMP = 0xEA,
		VKD_OEM_PA1 = 0xEB,
		VKD_OEM_PA2 = 0xEC,
		VKD_OEM_PA3 = 0xED,
		VKD_OEM_WSCTRL = 0xEE,
		VKD_OEM_CUSEL = 0xEF,
		VKD_OEM_ATTN = 0xF0,// Caps Lock	 
		VKD_OEM_FINISH = 0xF1,
		VKD_OEM_COPY = 0xF2,// �J�^�J�i
		VKD_OEM_AUTO = 0xF3,// ���p�^�S�p
		VKD_OEM_ENLW = 0xF4,// ���p�^�S�p
		VKD_OEM_BACKTAB = 0xF5,
		VKD_ATTN = 0xF6,
		VKD_CRSEL = 0xF7,
		VKD_EXSEL = 0xF8,
		VKD_EREOF = 0xF9,
		VKD_PLAY = 0xFA,
		VKD_ZOOM = 0xFB,
		VKD_NONAME = 0xFC,
		VKD_PA1 = 0xFD,
		VKD_OEM_CLEAR = 0xFE,
	};

};