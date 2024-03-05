#include "alclipboard.h"

//To debug, or not to debug...
bool bDebug = true;
bool quit = false;
HHOOK g_hKeyboardHook;


AlCLogger	*logger = NULL;
bool process = false;

AlClipboard *alClipboard = NULL;

AL_COPY_OR_PASTE copyOrPaste = AL_COPY_OR_PASTE::ALCOPY_NONE;

/**
This function handles the key events.
**/
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	wchar_t tBuf[2048];
	char    cBuf[2048];
	char    cBuf2[2048];

	ZeroMemory(tBuf, sizeof(wchar_t) * 2048);
	ZeroMemory(cBuf, sizeof(char) * 2048);
	ZeroMemory(cBuf2, sizeof(char) * 2048);

	LPKBDLLHOOKSTRUCT str = NULL;
	str = (LPKBDLLHOOKSTRUCT)lParam;

	if (str->vkCode == VK_ESCAPE)
	{
		//esc = (wParam == WM_KEYDOWN);
		if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);
		g_hKeyboardHook = NULL;

		logger->debug(L"quitting...\n");
		quit = true;
		copyOrPaste = AL_COPY_OR_PASTE::ALCOPY_NONE;
		exit(0);
		return true;
	}

	if (process)
	{
		return false;
	}

	if (wParam == WM_KEYDOWN)
	{
		char cPressed[3];
		ZeroMemory(cPressed, sizeof(char) * 3);

		cPressed[0] = (char)MapVirtualKey(str->vkCode, MAPVK_VK_TO_CHAR);

		switch (str->vkCode)
		{
		case VK_F1:
			strcpy(cPressed, "F1\0");
			break;
		case VK_F2:
			strcpy(cPressed, "F2\0");
			break;
		case VK_F3:
			strcpy(cPressed, "F3\0");
			break;
		case VK_F4:
			strcpy(cPressed, "F4\0");
			break;
		case VK_F5:
			strcpy(cPressed, "F5\0");
			break;
		case VK_F6:
			strcpy(cPressed, "F6\0");
			break;
		case VK_F7:
			strcpy(cPressed, "F7\0");
			break;
		case VK_F8:
			strcpy(cPressed, "F8\0");
			break;
		case VK_F9:
			strcpy(cPressed, "F9\0");
			break;
		case VK_F10:
			strcpy(cPressed, "F10\0");
			break;
		case VK_F11:
			strcpy(cPressed, "F11\0");
			break;
		case VK_F12:
			strcpy(cPressed, "F12\0");
			break;
		default:
			break;
		}

		sprintf_s(cBuf, "Pressed a Vcode: '%i' : '%s'\n", str->vkCode, cPressed);
		logger->debug(cBuf);

		sprintf_s(cBuf, "%s\0", cPressed);


		//GetPrivateProfileStringA("MyRobot", cBuf, "", cBuf2, sizeof(char) * 2048, ".\\MyRobot.ini");
		//if (strlen(cBuf2) > 0)
		{
		//	sprintf_s(cBuf, "\tProcessing...key..Value: '%s'\n", cBuf2);
		//	logger->debug(cBuf);
		//	process = true;
		}

		//PROCESS HERE
		if (copyOrPaste == AL_COPY_OR_PASTE::ALCOPY_NONE)
		{
			if (strcmp(cPressed, "C") == 0)
			{
				logger->debug("\t copying . . . . \n");
				copyOrPaste = AL_COPY_OR_PASTE::ALCOPY_COPY;
				return true;
			} //if
			if (strcmp(cPressed, "P") == 0)
			{
				logger->debug("\t pasting . . . . \n");
				copyOrPaste = AL_COPY_OR_PASTE::ALCOPY_PASTE;
				return true;
			} //if
		} //if


		//NOW DO SENDKEYS...
		if (copyOrPaste != AL_COPY_OR_PASTE::ALCOPY_NONE)
		{
			if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);
			g_hKeyboardHook = NULL;

			process = true;

			logger->debug("\t processing....");
			logger->debug(cPressed);
			logger->debug("\n");

			//RUN THREAD HERE
			_beginthread(doProcess, 0, cPressed);

			quit = true;
			return true;
		} //if
	}
	return false;
}



void doProcess(void *pKey)
{
	logger->debug("doProcess:-- START\n");

	if (!alClipboard)
	{
		return;
	}

	if (copyOrPaste == AL_COPY_OR_PASTE::ALCOPY_COPY)
	{
		//keybd_event(VK_CONTROL, 0, 0, 0);
		//keybd_event(0x43, 0, 0, 0);
		//Sleep(1);
		//keybd_event(0x43, 0, KEYEVENTF_KEYUP, 0);
		//keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
		//Sleep(10);

		alClipboard->readFromClipboard((char *) pKey);
	}

	if (copyOrPaste == AL_COPY_OR_PASTE::ALCOPY_PASTE)
	{
		alClipboard->writeToClipboard((char *)pKey);

		//keybd_event(VK_CONTROL, 0, 0, 0);
		//keybd_event(0x56, 0, 0, 0);
		//Sleep(1);
		//keybd_event(0x56, 0, KEYEVENTF_KEYUP, 0);
		//keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);

	}

	copyOrPaste = AL_COPY_OR_PASTE::ALCOPY_NONE;
	_endthread();
}

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	logger = new AlCLogger(L".\\alclipboard.log", bDebug);
	alClipboard = new AlClipboard();

	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);


	//keybd_event((byte) 'P', 0, 0, 0);
	//Sleep(10);
	//keybd_event((byte) 'P', 0, KEYEVENTF_KEYUP, 0);

	g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

	MSG msg;
	int bRet = 0;
	while (!quit && (bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		Sleep(1);

		if (bRet == -1)
		{
			// handle the error and possibly exit
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	ShowCursor(TRUE);

	if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);

	SAFE_DEL(alClipboard);
	SAFE_DEL(logger);
	return 0;

}

bool AlClipboard::readFromClipboard(char *pKey)
{
	logger->debug("AlClipboard::readFromClipboard:-- START\n");

	char tbuf[128];
	ZeroMemory(tbuf, sizeof(char) * 128);


	if (!OpenClipboard(NULL))
	{
		logger->debug("\t OpenClipboard() FAILED..\n");
		return false;
	}

	char *pBuffer = NULL;
	

	if (	IsClipboardFormatAvailable(CF_TEXT)
		||  IsClipboardFormatAvailable(CF_OEMTEXT))
	{
		logger->debug("\t Getting text clipboard data..\n");
		HANDLE hClipboardData = GetClipboardData(CF_TEXT);
		char *pchData = (char*)GlobalLock(hClipboardData);
		pBuffer = new char[strlen(pchData)];

		strcpy(pBuffer, pchData);
		GlobalUnlock(hClipboardData);
	}

	logger->debug("\t CloseClipboard()..\n");
	CloseClipboard();

	writeToDisk(pKey, pBuffer);
	SAFE_DEL(pBuffer);

	return true;
}

bool AlClipboard::readFromDisk(char *pKey, char *pBuffer, int pMaxSize)
{
	char tbuf[128];
	ZeroMemory(tbuf, sizeof(char) * 128);

	char tbuf2[128];
	ZeroMemory(tbuf2, sizeof(char) * 128);

	sprintf(tbuf, "AlClipboard::readFromDisk:-- START, pKey: '%s', pBuffer size; %i\n\0", pKey, strlen(pBuffer));
	logger->debug(tbuf);

	sprintf(tbuf, "%s/%s\0", DEFAULT_DIRECTORY_NAME, pKey);
	sprintf(tbuf2, "\t reading... '%s'\n\0", tbuf);
	logger->debug(tbuf2);

	FILE *fle = fopen(tbuf, "rt");
	if (!fle)
	{
		sprintf(tbuf2, "\t Opening file '%s' failed.\n\0", tbuf);
		logger->debug(tbuf2);
		return false;
	}

	sprintf(tbuf2, "\t reading file '%s'...\n\0", tbuf);
	logger->debug(tbuf2);
	ZeroMemory(pBuffer, sizeof(char) * pMaxSize);
	fread(pBuffer, sizeof(char), pMaxSize, fle);

	sprintf(tbuf2, "\t closing file '%s'...\n\0", tbuf);
	fclose(fle);
	fle = NULL;

	return true;
}


bool AlClipboard::writeToClipboard(char *pKey)
{
	logger->debug("AlClipboard::writeToClipboard:-- START\n");

	char tbuf[128];
	ZeroMemory(tbuf, sizeof(char) * 128);

	char tbuf2[128];
	ZeroMemory(tbuf2, sizeof(char) * 128);
	sprintf(tbuf, "%s/%s\0", DEFAULT_DIRECTORY_NAME, pKey);
	sprintf(tbuf2, "\t reading... '%s'\n\0", tbuf);
	logger->debug(tbuf2);
	FILE *fle = fopen(tbuf, "rt");
	if (!fle)
	{
		sprintf(tbuf2, "\t Opening file '%s' failed.\n\0", tbuf);
		logger->debug(tbuf2);
		return false;
	}
	fseek(fle, 0, SEEK_END);
	int maxSize = ftell(fle);
	fclose(fle);

	sprintf(tbuf2, "\t File size: '%i'.\n\0", maxSize);
	logger->debug(tbuf2);
	if (maxSize < 1)
	{
		sprintf(tbuf2, "\t file too small.\n\0", tbuf);
		logger->debug(tbuf2);
		return false;
	}

	char *pBuffer = NULL;
	pBuffer = new char[maxSize+1];
	ZeroMemory(pBuffer, sizeof(char) * (maxSize+1));
	if (!readFromDisk(pKey, pBuffer, maxSize))
	{
		return false;
	}

	if (!OpenClipboard(NULL))
	{
		logger->debug("\t OpenClipboard.. FAILED..\n");
		return false;
	}

	EmptyClipboard();
	HGLOBAL hClipboardData;
	hClipboardData = GlobalAlloc(GMEM_DDESHARE, maxSize+1);
	char * pchData;
	pchData = (char*)GlobalLock(hClipboardData);
	strcpy(pchData, pBuffer);
	GlobalUnlock(hClipboardData);
	SetClipboardData(CF_TEXT, hClipboardData);


	logger->debug("\t CloseClipboard()");
	CloseClipboard();
	SAFE_DEL(pBuffer);
	return true;
}


bool AlClipboard::writeToDisk(char *pKey, char *pBuffer)
{
	char tbuf[128];
	ZeroMemory(tbuf, sizeof(char) * 128);

	char tbuf2[128];
	ZeroMemory(tbuf2, sizeof(char) * 128);

	sprintf(tbuf, "AlClipboard::writeToDisk:-- START, pKey: '%s', pBuffer size; %i\n\0", pKey, strlen(pBuffer));
	logger->debug(tbuf);

	CreateDirectoryA(DEFAULT_DIRECTORY_NAME, NULL);
	sprintf(tbuf, "%s/%s\0", DEFAULT_DIRECTORY_NAME, pKey);
	sprintf(tbuf2, "\t Writing... '%s'\n\0", tbuf);
	logger->debug(tbuf2);

	FILE *fle = fopen(tbuf, "wt");
	if (!fle)
	{
		sprintf(tbuf2, "\t Opening file '%s' failed.\n\0", tbuf);
		logger->debug(tbuf2);
		return false;
	}

	sprintf(tbuf2, "\t writing file '%s'...\n\0", tbuf);
	logger->debug(tbuf2);
	fwrite(pBuffer, sizeof(char), strlen(pBuffer), fle);
	fflush(fle);

	sprintf(tbuf2, "\t closing file '%s'...\n\0", tbuf);
	fclose(fle);
	fle = NULL;

	return true;
}

AlClipboard::AlClipboard()
{
	logger->debug("AlClipboard::AlClipboard():-- START\n");
	zeroMem();
}

AlClipboard::~AlClipboard()
{
	//destroy
	logger->debug("AlClipboard::~AlClipboard():-- START\n");

	//init
	zeroMem();
}


void AlClipboard::zeroMem()
{
	//init this.
	logger->debug("AlClipboard::zeroMem():-- START\n");

}