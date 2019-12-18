#define STRICT
#define WIN32_LEAN_AND_MEAN

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include <wchar.h>


#include "libwinclip.h"


/* For functions that set error codes on failure.
   display the error code message and exit
*/
void check_err_code_msg_exit(int exp)
{
	if (!exp) {
		PWCHAR buf;
		DWORD error_code = GetLastError();
		FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			error_code,
			0,
			(PWCHAR)& buf,
			0,
			NULL);
		MessageBoxW(NULL, buf, NULL, MB_OK);
		LocalFree(buf);
		ExitProcess(error_code);
	}
}
#define CHECK(EXP) check_err_code_msg_exit(EXP)


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	switch (uMsg) {
		
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_SIZE: {
			
			HWND edit_ctrl = FindWindowEx(hwnd, NULL, L"Edit", NULL);
			HWND status_bar = FindWindowEx(hwnd, NULL, STATUSCLASSNAME, NULL);
			RECT client_rect;
			GetClientRect(hwnd, &client_rect);
			
			SetWindowPos(edit_ctrl, HWND_TOP, 
			client_rect.left, client_rect.top,
			client_rect.right, client_rect.bottom - 25,
			SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
			
			SetWindowPos(status_bar, HWND_TOP, 
			0,0,0,0,
			SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
			
			
			return 0;
		}
		case WM_CLIPBOARDUPDATE: {
			
			HWND status_bar = FindWindowEx(hwnd, NULL, STATUSCLASSNAME, NULL);
			
			HWND edit_ctrl = FindWindowEx(hwnd, NULL, L"Edit", NULL);
			struct winclip *clip = (struct winclip*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
			if(!OpenClipboard(hwnd)) return 0;
			if(winclip_get(clip)) {
				SetWindowTextW(edit_ctrl, clip->data);
				
				int length = GetWindowTextLength(edit_ctrl);
				wchar_t status_bar_msg[128];
				swprintf(status_bar_msg, 128, L"clipboard content length: %d - buffer size: %zu", 
				length, clip->size);
				SetWindowTextW(status_bar, status_bar_msg);
			
			}
			CloseClipboard();
			
			return 0;
		}
		
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class
	const TCHAR CLASS_NAME[] = L"win-class";
	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	//wc.hIconSm = NULL;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;

	CHECK(RegisterClassEx(&wc));

	// Create the window
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Demo",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);
	CHECK(hwnd != NULL);
	
	struct winclip clip;
	if(!winclip_init_n(&clip, CF_UNICODETEXT, 64)) return EXIT_FAILURE;
	SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)&clip);
	
	/* Retrieve window coordinates */
	RECT client_rect;
	CHECK(GetClientRect(hwnd, &client_rect));
	
	HWND edit_ctrl = CreateWindowEx(0, L"Edit", L"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | 
		ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL,
		0, 0,
		client_rect.right, client_rect.bottom - 25,
		hwnd, NULL, hInstance,
		NULL);
	CHECK(edit_ctrl != NULL);
	
	
	// Create the status bar.
    HWND status_bar = CreateWindowEx(
        0,                       // no extended styles
        STATUSCLASSNAME,         // name of status bar class
        NULL,           // no text when first created
        SBARS_SIZEGRIP |         // includes a sizing grip
        WS_CHILD | WS_VISIBLE,   // creates a visible child window
        0, 0, 0, 0,              // ignores size and position
        hwnd,              // handle to parent window
        0,       // child window identifier
        hInstance,                   // handle to application instance
        NULL);                   // no window creation data
	
	CHECK(status_bar != NULL);
	
	
	CHECK(AddClipboardFormatListener(hwnd) != FALSE);

	ShowWindow(hwnd, nCmdShow);
	
	MSG msg = {0};
	BOOL ret;
	while ((ret != GetMessage(&msg, NULL, 0, 0)))
	{
		CHECK(ret != -1);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

