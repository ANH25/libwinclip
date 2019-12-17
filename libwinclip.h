#if !defined(LIBWINCLIP_H)
#define LIBWINCLIP_H

#include <Windows.h>

/* NOTES: 
- All functions return 1 on success, 0 on failure
- Clipboard must be opened before calling functions that deal directly with the clipboard 
  like winclip_get and winclip_put, and then closed to allow other applications to use the clipboard. 
  (Use the standard Windows functions OpenClipboard and CloseClipboard)
*/

struct winclip {
	UINT format;
	void *data;
	size_t size;
};

#if !defined(LIBWINCLIP_INIT_BUF_SIZE)
#define LIBWINCLIP_INIT_BUF_SIZE 64
#endif

/* Allocate initial memory of size 'buf_size' for wclip->data and 
   set wclip->format to 'format'
*/
int winclip_init_n(struct winclip *wclip, UINT format, size_t buf_size);

/* Same as winclip_init_n() but uses the LIBWINCLIP_INIT_BUF_SIZE macro as a buffer size */
int winclip_init(struct winclip *wclip, UINT format);

/* Free wclip->data */
int winclip_free(struct winclip *wclip);

/* Get clipboard content according to the format in wclip->format */
int winclip_get(struct winclip *wclip);

/* copy the data in wclip->data to the clipboard */
int winclip_put(struct winclip *wclip);

/* copy 'size' bytes from 'data' to the clipboard according to the format 'format' */
int winclip_put_n(void *data, size_t size, UINT format);

/* Copy the null-terminated ANSI string 'str' to the clipboard.
   The CF_TEXT format is used, so the string is interpreted as ANSI text.
 */
int winclip_put_str(char *str);

/* Copy the null-terminated wchar string 'str' to the clipboard.
   The CF_UNICODETEXT format is used, so the string is interpreted as UTF-16 text.
*/
int winclip_put_wstr(wchar_t *str);


#endif /* LIBWINCLIP_H */
