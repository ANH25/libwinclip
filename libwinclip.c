#include <Windows.h>

#include <stdlib.h>
#include <string.h>

#include "libwinclip.h"


int winclip_init_n(struct winclip *wclip, UINT format, size_t buf_size) {
	
	wclip->format = format;
	wclip->data = malloc(buf_size);
	if(!wclip->data) return 0;
	wclip->size = buf_size;
	return 1;
}

int winclip_init(struct winclip *wclip, UINT format) {
	
	return winclip_init_n(wclip, format, LIBWINCLIP_INIT_BUF_SIZE);
}

int winclip_free(struct winclip *wclip) {
		
	free(wclip->data);
	wclip->data = NULL;
	wclip->size = 0;
	wclip->format = 0;
	return 1;
}

int winclip_get(struct winclip *wclip) {

	void *data = GetClipboardData(wclip->format);
	if (!data) return 0;
	void *locked_data = GlobalLock(data);
	if (!locked_data) return 0;
	HGLOBAL hglb = GlobalHandle(locked_data);
	if(!hglb) return 0;
	size_t data_size = GlobalSize(hglb);
	if (data_size > wclip->size) {
		void *tmp = realloc(wclip->data, data_size);
		if (!tmp) return 0;
		wclip->data = tmp;
		wclip->size = data_size;
	}
	memcpy(wclip->data, locked_data, data_size);
	GlobalUnlock(data);
	
	return 1;
}

int winclip_put_n(void *data, size_t size, UINT format) {
	
	if(!EmptyClipboard()) return 0;
	
	HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE, size);
	if (!hglb) return 0;
	void *hglb_locked = GlobalLock(hglb);
	if (!hglb_locked) {
		GlobalFree(hglb);
		return 0;
	}
	memcpy(hglb_locked, data, size);
	GlobalUnlock(hglb);
	SetClipboardData(format, hglb);
	
	return 1;
}

int winclip_put(struct winclip *wclip) {
	
	return winclip_put_n(wclip->data, wclip->size, wclip->format);
}

int winclip_put_str(char *str) {
	
	return winclip_put_n(str, strlen(str) + 1, CF_TEXT);
}

int winclip_put_wstr(wchar_t *str) {
	
	return winclip_put_n(str, (wcslen(str) + 1) * sizeof(wchar_t), CF_UNICODETEXT);
}

