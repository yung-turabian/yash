#include "../include/io.h"

// Fix this
static const char* hist[40];
static int hist_len;
static int curr_hist_sel = 0;
static bool editing_line = false;

#define SOH 1 // CTRL+A
#define ETX 3 // CTRL+C
#define EOT 4
#define HOR_TAB 9
#define LINE_FEED 10 // \n
#define FORM_FEED 12 // FF, CTRL+L
#define DEL 127
#define BS 8
#define SYN 22 // CTRL+V
#define SUB 26 // CTRL+Z


typedef struct Emoji {


} Emoji;

int
is_emoji(wchar_t wc)
{
   // Check for common emoji ranges in Unicode
    return (
        (wc >= 0x1F600 && wc <= 0x1F64F) || // Emoticons
        (wc >= 0x1F300 && wc <= 0x1F5FF) || // Miscellaneous Symbols and Pictographs
        (wc >= 0x1F680 && wc <= 0x1F6FF) || // Transport and Map Symbols
        (wc >= 0x1F700 && wc <= 0x1F77F) || // Alchemical Symbols
        (wc >= 0x1F780 && wc <= 0x1F7FF) || // Geometric Shapes Extended
        (wc >= 0x1F800 && wc <= 0x1F8FF) || // Supplemental Arrows-C
        (wc >= 0x1F900 && wc <= 0x1F9FF) || // Supplemental Symbols and Pictographs
        (wc >= 0x1FA00 && wc <= 0x1FA6F) || // Chess Symbols
        (wc >= 0x1FA70 && wc <= 0x1FAFF)    // Symbols and Pictographs Extended-A
    );
}

#include <wchar.h>
#include <locale.h>

void
handle_input(char* buf, u8 *buf_len)
{
		bool breakout = false;
		
		setlocale(LC_ALL, "en_US.utf8"); //include in main
		unsigned char* pasted;
		wchar_t wc;
		int ret;
		char emoji_buf[16]; // change
		// pretty shit, rework
		//
		// CTRL+A beginning of line
		// +E end
		// +K yank after cursor
		// +U before
		// +

		char c;
		do {
				if(isatty(Shell.terminal)) { // write buffer if not testing
						fprintf(stdout, "%c8", ESC);
						fprintf(stdout, "%s%c[0K", buf, ESC);
						fflush(stdout);
				}

				c = getchar();

				switch(c) {
						case SOH:
								fprintf(stdout, "heyy\n");
								fflush(stdout);
								break;	
						case ETX:
								cfbprintf("^C", ANSI_YELLOW, ANSI_RED);
								buf[0] = '\0';
								breakout = true;
								break;
						case HOR_TAB:
								break;
						case LINE_FEED: 
								breakout = true;
								editing_line = false;
								if(*buf_len > 0) {
										hist[hist_len++] = strdup(buf);
										hist[hist_len] = "";
										curr_hist_sel++;
								}
								break;
						case EOT:
								//shouldExit = true;
						case EOF: // added for testing and feeding from another file
								breakout = true;
								exit(0);
								break;
						case FORM_FEED:
								fprintf(stdout, "%c[2J%c[1;1H🐚 %c7", 
												ESC,ESC,ESC); 
								break;
						case DEL:
						case BS:
								editing_line = true;
								if(*buf_len > 0) buf[--(*buf_len)] = '\0';
								break;
						case SYN: // pretty sloppy
								pasted = getX11Clipboard();
								ret = mbtowc(&wc, (char*)pasted, MB_CUR_MAX);
								//wc = (wchar_t*)pasted;
								/* DEBUG if (ret > 0) {
										printf("The wide character: %lc\n", wc);
										if (is_emoji(wc)) {
												printf("The character is an emoji.\n");
										} else {
												printf("The character is not an emoji.\n");
										}
								} else {
										printf("Conversion failed.\n");
								}*/

								sprintf(emoji_buf, "%lc", wc);
								
								strcat(buf, emoji_buf); 
								*buf_len += strlen(emoji_buf); // use a linked instead and BS pops current,
																							 // account four cursor movement
								break;
						/*case 'e':
								//debug, kinda janky
								char* argv[2];
								argv[0] = (char*)"efck-chat-keyboard/efck-chat-keyboard";
								call(argv);
								buf[0] = '\0';
								buf_len++;
								break;*/
						case SUB:
								buf[0] = '\0';
								break;
						case ESC:
								getchar(); //consume '['
								switch(getchar()) { 
										case 'A': //Up arrow
										if(!editing_line && curr_hist_sel > 0) {
												strcpy(buf, hist[--curr_hist_sel]);
												*buf_len = strlen(hist[curr_hist_sel]);
												editing_line = false;
										}

										break;

										case 'B':
										if(!editing_line && curr_hist_sel >= 0 && curr_hist_sel < hist_len) {
												strcpy(buf, hist[++curr_hist_sel]);
												*buf_len = strlen(hist[curr_hist_sel]);
												editing_line = false;
										}
										break;

										case 'C': //right
										

										break;

										case 'D':


										break;
								}
								break;
						default:
								editing_line = true;
								strncat(buf, &c, 1);
								(*buf_len)++;
								break;
				}
		} while(!(breakout));
}
