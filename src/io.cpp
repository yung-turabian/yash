#include "../include/io.h"

// Fix this
static const char* hist[40];
static int hist_len;
static int curr_hist_sel = 0;

static bool editing_line = false;

void
handle_input(char* buf, u8 *buf_len)
{
		bool breakout = false;
		unsigned char* emoji;
		// pretty shit, rework
		char c;
		do {
				//fprintf(stdout, "%c[2K\r", ESC);
				fprintf(stdout, "%c8", ESC);
				fprintf(stdout, "%s%c[0K", buf, ESC);
				fflush(stdout);

				c = getchar();

				switch(c) {
						case 3: //^C
								if(strlen(buf) == 0) {
								}
								break;
						case 9: //TAB
								break;
						case 10: //LF
								breakout = true;
								editing_line = false;
								if(strlen(buf) != 0) {
										hist[hist_len++] = strdup(buf);
										hist[hist_len] = "";
										curr_hist_sel++;
								}
								break;
						case 12: //FF, CTRL-L
								fprintf(stdout, "%c[2J%c[1;1H🐚 %c7", 
												ESC,ESC,ESC); 
								break;
						case 127: //DEL
						case 8: //BS
								editing_line = true;
								if(*buf_len > 0) buf[--(*buf_len)] = '\0';
								break;
						case 22: //CTRL-V, pretty sloppy
								//emoji = getX11Clipboard();
								//emoji = getClipboardContents();
								strcat(buf, (const char*) emoji);
								fprintf(stdout, "%s", emoji);
								//fflush(stdout);
								*buf_len += strlen((const char*)emoji);
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
										if(!editing_line && curr_hist_sel >= 0 && curr_hist_sel < hist_len)
												strcpy(buf, hist[++curr_hist_sel]);
												*buf_len = strlen(hist[curr_hist_sel]);
												editing_line = false;
										break;
								}
								break;
						default:
								editing_line = true;
								strncat(buf, &c, 1);
								(*buf_len)++;
								break;
				}
		} while(c != EOF && !(breakout));
}
