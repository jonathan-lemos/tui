/** @file ctermtools.c
 *
 * Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "../backend.h"
#include <stdio.h>
#include <stdint.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

static uint_fast32_t attr_cur;

static void tui_apply(void){
	if (attr_cur & TUI_BOLD){
		printf("\033[1m");
	}
	if (attr_cur & TUI_UNDERLINE){
		printf("\033[4m");
	}
	if (attr_cur & TUI_BLINK){
		printf("\033[5m");
	}
	if (attr_cur & TUI_INVERT){
		printf("\033[7m");
	}

	if (attr_cur & TUI_FG_BRIGHT){
		if (attr_cur & TUI_FG_DEFAULT){
			printf("\033[39m");
		}
		if (attr_cur & TUI_FG_BLACK){
			printf("\033[90m");
		}
		if (attr_cur & TUI_FG_RED){
			printf("\033[91m");
		}
		if (attr_cur & TUI_FG_GREEN){
			printf("\033[92m");
		}
		if (attr_cur & TUI_FG_YELLOW){
			printf("\033[93m");
		}
		if (attr_cur & TUI_FG_BLUE){
			printf("\033[94m");
		}
		if (attr_cur & TUI_FG_MAGENTA){
			printf("\033[95m");
		}
		if (attr_cur & TUI_FG_CYAN){
			printf("\033[96m");
		}
		if (attr_cur & TUI_FG_WHITE){
			printf("\033[97m");
		}
	}
	else{
		if (attr_cur & TUI_FG_DEFAULT){
			printf("\033[39m");
		}
		if (attr_cur & TUI_FG_BLACK){
			printf("\033[30m");
		}
		if (attr_cur & TUI_FG_RED){
			printf("\033[31m");
		}
		if (attr_cur & TUI_FG_GREEN){
			printf("\033[32m");
		}
		if (attr_cur & TUI_FG_YELLOW){
			printf("\033[33m");
		}
		if (attr_cur & TUI_FG_BLUE){
			printf("\033[34m");
		}
		if (attr_cur & TUI_FG_MAGENTA){
			printf("\033[35m");
		}
		if (attr_cur & TUI_FG_CYAN){
			printf("\033[36m");
		}
		if (attr_cur & TUI_FG_WHITE){
			printf("\033[37m");
		}
	}

	if (attr_cur & TUI_BG_BRIGHT){
		if (attr_cur & TUI_BG_DEFAULT){
			printf("\033[49m");
		}
		if (attr_cur & TUI_BG_BLACK){
			printf("\033[100m");
		}
		if (attr_cur & TUI_BG_RED){
			printf("\033[101m");
		}
		if (attr_cur & TUI_BG_GREEN){
			printf("\033[102m");
		}
		if (attr_cur & TUI_BG_YELLOW){
			printf("\033[103m");
		}
		if (attr_cur & TUI_BG_BLUE){
			printf("\033[104m");
		}
		if (attr_cur & TUI_BG_MAGENTA){
			printf("\033[105m");
		}
		if (attr_cur & TUI_BG_CYAN){
			printf("\033[106m");
		}
		if (attr_cur & TUI_BG_WHITE){
			printf("\033[107m");
		}

	}
	else{
		if (attr_cur & TUI_BG_DEFAULT){
			printf("\033[49m");
		}
		if (attr_cur & TUI_BG_BLACK){
			printf("\033[40m");
		}
		if (attr_cur & TUI_BG_RED){
			printf("\033[41m");
		}
		if (attr_cur & TUI_BG_GREEN){
			printf("\033[42m");
		}
		if (attr_cur & TUI_BG_YELLOW){
			printf("\033[43m");
		}
		if (attr_cur & TUI_BG_BLUE){
			printf("\033[44m");
		}
		if (attr_cur & TUI_BG_MAGENTA){
			printf("\033[45m");
		}
		if (attr_cur & TUI_BG_CYAN){
			printf("\033[46m");
		}
		if (attr_cur & TUI_BG_WHITE){
			printf("\033[47m");
		}
	}
}

void tui_attron(uint_fast32_t attr){
	attr_cur |= attr;
	printf("\033[0m");
	tui_apply();
}

void tui_attroff(uint_fast32_t attr){
	attr_cur &= ~(attr);
	printf("\033[0m");
	tui_apply();
}

void tui_attrclear(void){
	printf("\033[0m");
	attr_cur = 0;
}

int tui_getcols(void){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
}

int tui_getrows(void){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_row;
}

void tui_showcursor(int enable){
	if (enable){
		printf("\033[?25h");
	}
	else{
		printf("\033[?25l");
	}
	fflush(stdout);
}

int tui_setecho(int enable){
	struct termios term_info;

	if (tcgetattr(STDIN_FILENO, &term_info) != 0){
		return -1;
	}
	term_info.c_lflag = (term_info.c_lflag & ~(ECHO)) | (enable * ECHO);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term_info) != 0){
		return -1;
	}
	return 0;
}

void tui_setcursorpos(int row, int col){
	row++;
	col++;
	printf("\033[%d;%dH", row, col);
	fflush(stdout);
}

void tui_movecursorpos(int row_delta, int col_delta){
	if (row_delta > 0){
		printf("\033[%dB", row_delta);
	}
	else if (row_delta < 0){
		printf("\033[%dA", -row_delta);
	}
	if (col_delta > 0){
		printf("\033[%dC", col_delta);
	}
	else if (col_delta < 0){
		printf("\033[%dD", -col_delta);
	}
	fflush(stdout);
}

void tui_clear(void){
	printf("\033[2J");
	printf("\033[1;1H");
	fflush(stdout);
}

int tui_getch(void){
	struct termios old;
	struct termios new;
	int c;

	tcgetattr(STDIN_FILENO, &old);
	new = old;
	new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new);

	do{
		c = getchar();
		/* arrow keys are escape sequences:
		 * example: up arrow == "\033[A" */
		if (c == '\033'){
			/* discard '[' */
			getchar();
			c = getchar();
			tcsetattr(STDIN_FILENO, TCSANOW, &old);
			switch (c){
			case 'A':
				return KEY_UP;
			case 'B':
				return KEY_DOWN;
			case 'C':
				return KEY_RIGHT;
			case 'D':
				return KEY_LEFT;
			}
		}
	}while (c == EOF);

	tcsetattr(STDIN_FILENO, TCSANOW, &old);
	return c;
}
