#include "window.h"
#include <stdlib.h>

int main(void){
	tui_window* my_child;
	if (tui_win_make(stdwin, &my_child) != TUI_OK){
		abort();
	}

	if (tui_win_set(my_child, TUI_SET_GRAVITY, TUI_GRAV_LEFT) != TUI_OK){
		abort();
	}
	tui_win_set(my_child, TUI_SET_WIDTH, 20);
	tui_win_set(my_child, TUI_SET_HEIGHT, 10);

	tui_show(stdwin);
	while (1);
}
