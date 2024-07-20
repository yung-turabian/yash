#include "../include/io.h"

// Fix this
static const char* hist[40];
static int hist_len;
static int curr_hist_sel = 0;
static bool editing_line = false;

#define ETX 3 // CTRL+C
#define HOR_TAB 9
#define LINE_FEED 10 // \n
#define FORM_FEED 12 // FF, CTRL+L
#define DEL 127
#define BS 8
#define SYN 22 // CTRL+V

void
handle_input(char* buf, u8 *buf_len)
{
		bool breakout = false;

		//unsigned char* emoji;
		// pretty shit, rework

		char c;
		do {
				if(isatty(Shell.terminal)) { // write buffer if not testing
						fprintf(stdout, "%c8", ESC);
						fprintf(stdout, "%s%c[0K", buf, ESC);
						fflush(stdout);
				}

				c = getchar();

				switch(c) {
						case ETX:
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
						case EOF: // added for testing and feeding from another file
								breakout = true;
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
								//emoji = getX11Clipboard();
								//emoji = getClipboardContents();
								//strcat(buf, (const char*) emoji);
								//fprintf(stdout, "%s", emoji);
								//fflush(stdout);
								//*buf_len += strlen((const char*)emoji);
								break;
						/*case 'e':
								//debug, kinda janky
								char* argv[2];
								argv[0] = (char*)"efck-chat-keyboard/efck-chat-keyboard";
								call(argv);
								buf[0] = '\0';
								buf_len++;
								break;*/
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
