#include <Windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>


#include "libwinclip.h"


int main(void) {
	
	/* opening and closing the clipboard is done using the Windows API functions  */
	if(!OpenClipboard(NULL)) return EXIT_FAILURE;
	
	struct winclip clip;
	/* initialize the winclip struct to store unicode text */
	if(!winclip_init(&clip, CF_UNICODETEXT)) return EXIT_FAILURE;
	/* Get unicode text from the clipboard */
	if(!winclip_get(&clip)) return EXIT_FAILURE;
	/* Data is simply accessed by the struct 'data' field */
	wprintf(L"%ls\n", clip.data);
	/* change format */
	clip.format = CF_TEXT;
	if(!winclip_get(&clip)) return EXIT_FAILURE;
	printf("%s\n", clip.data);
	
	/* copy a unicode string to the clipboard */
	winclip_put_wstr(L"!مرحباً");
	/* free the struct */
	winclip_free(&clip);
	/* close the clipboard */
	CloseClipboard();
	
	return 0;
}
