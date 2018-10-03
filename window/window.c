/** @file window/window.c
 * @brief Defines the window structure as well as its creation methods.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "window.h"

#include "backend.h"
#include <locale.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int __tui_addchild(tui_window* parent, tui_window* child){
	tui_window** tmp;
	for (size_t i = 0; i < parent->children.len; ++i){
		if (parent->children.arr[i] == child){
			return TUI_OK;
		}
	}

	(parent->children.len)++;
	tmp = realloc(parent->children.arr, parent->children.len * sizeof(*(parent->children.arr)));
	if (!tmp){
		(parent->children.len)--;
		return TUI_ENOMEM;
	}
	parent->children.arr = tmp;

	parent->children.arr[parent->children.len - 1] = child;
	return TUI_OK;
}

int __tui_delchild(tui_window* parent, tui_window* child){
	tui_window** tmp;
	size_t i;
	for (i = 0; i < parent->children.len; ++i){
		if (parent->children.arr[i] == child){
			break;
		}
	}
	if (i == parent->children.len){
		return TUI_EINVAL;
	}

	for (; i < parent->children.len; ++i){
		parent->children.arr[i] = parent->children.arr[i + 1];
	}

	(parent->children.len)--;
	tmp = realloc(parent->children.arr, parent->children.len * sizeof(*(parent->children.arr)));
	if (!tmp){
		(parent->children.len)++;
		return TUI_ENOMEM;
	}

	return TUI_OK;
}

int __tui_solvechildcollisions(tui_window* win, struct tui_container* out){
	int left_edge  = 0;
	int right_edge = win->pos.usable.col_right - 1;
	int top_edge   = 0;
	int bot_edge   = win->pos.usable.row_bot - 1;
	size_t i;

	// left
	for (i = 0; i < win->children.len; ++i){
		tui_window* child = win->children.arr[i];
		if (child->gravity == TUI_GRAV_LEFT){
			left_edge = child->pos.total.col_right + 1;
			i++;
			break;
		}
	}
	for (; i < win->children.len; ++i){
		tui_window* child = win->children.arr[i];
		if (child->gravity != TUI_GRAV_LEFT){
			continue;
		}
		child->pos.total.col_left  += left_edge;
		child->pos.total.col_right += left_edge;
		left_edge = child->pos.total.col_right + 1;
	}

	// right
	for (i = 0; i < win->children.len; ++i){
		tui_window* child = win->children.arr[i];
		if (child->gravity == TUI_GRAV_RIGHT){
			right_edge = child->pos.total.col_left - 1;
			i++;
			break;
		}
	}
	for (; i < win->children.len; ++i){
		tui_window* child = win->children.arr[i];
		size_t diff;
		if (child->gravity != TUI_GRAV_RIGHT){
			continue;
		}
		diff = child->pos.total.col_right - child->pos.total.col_left;
		child->pos.total.col_left  = right_edge - diff;
		child->pos.total.col_right = right_edge;
		right_edge = child->pos.total.col_left - 1;
	}

	// top
	for (i = 0; i < win->children.len; ++i){
		tui_window* child = win->children.arr[i];
		if (child->gravity == TUI_GRAV_TOP){
			top_edge = child->pos.total.row_bot + 1;
			i++;
			break;
		}
	}
	for (; i < win->children.len; ++i){
		tui_window* child = win->children.arr[i];
		if (child->gravity != TUI_GRAV_TOP){
			continue;
		}
		child->pos.total.row_top += top_edge;
		child->pos.total.row_bot += top_edge;
		top_edge = child->pos.total.row_bot + 1;
	}

	// bottom
	for (i = 0; i < win->children.len; ++i){
		tui_window* child = win->children.arr[i];
		if (child->gravity == TUI_GRAV_BOT){
			bot_edge = child->pos.total.row_top - 1;
			i++;
			break;
		}
	}
	for (; i < win->children.len; ++i){
		tui_window* child = win->children.arr[i];
		size_t diff;
		if (child->gravity != TUI_GRAV_RIGHT){
			continue;
		}
		diff = child->pos.total.row_top - child->pos.total.row_bot;
		child->pos.total.row_top = bot_edge - diff;
		child->pos.total.row_bot = bot_edge;
		bot_edge = child->pos.total.row_top - 1;
	}

	if (left_edge > right_edge || top_edge > bot_edge){
		return TUI_ENOSPC;
	}

	out->col_left  = left_edge;
	out->col_right = right_edge;
	out->row_top   = top_edge;
	out->row_bot   = bot_edge;

	return TUI_OK;
}

static int __tui_calcusablespace(tui_window* win){
	win->pos.usable.col_left  = win->pos.total.col_left + win->x_padding;
	win->pos.usable.col_right = win->pos.total.col_right - win->x_padding;
	win->pos.usable.row_top   = win->pos.total.row_top + win->y_padding;
	win->pos.usable.row_bot   = win->pos.total.row_bot - win->y_padding;
	return TUI_OK;
}

TUI_CONST tui_window* __getstdwin(void){
	static tui_window tui;
	static bool initialized = false;

	if (!initialized){
		tui.gravity      = TUI_GRAV_CENTER;
		tui.width        = TUI_MATCH_PARENT;
		tui.height       = TUI_MATCH_PARENT;
		tui.x_padding    = 0;
		tui.y_padding    = 0;
		tui.parent       = NULL;
		tui.children.arr = NULL;
		tui.children.len = 0;
		tui.pos.total.col_left   = 0;
		tui.pos.total.col_right  = tui_getcols() - 1;
		tui.pos.total.row_top    = 0;
		tui.pos.total.row_bot    = tui_getrows() - 1;
		tui.pos.usable.col_left  = 0;
		tui.pos.usable.col_right = tui_getcols() - 1;
		tui.pos.usable.row_top   = 0;
		tui.pos.usable.row_bot   = tui_getrows() - 1;
	}
	initialized = true;

	return &tui;
}

static void print_box(tui_window* win){
	const char* locale = setlocale(LC_ALL, NULL);
	size_t row1 = win->pos.total.row_top;
	size_t row2 = win->pos.total.row_bot;
	size_t col1 = win->pos.total.col_left;
	size_t col2 = win->pos.total.col_right;

	setlocale(LC_ALL, "");
	tui_setcursorpos(row1, col1);
	printf("%s", "┌");
	for (size_t i = col1 + 1; i <= col2 - 1; ++i){
		printf("%s", "─");
	}
	printf("%s", "┐");
	for (size_t i = row1 + 1; i <= row2 - 1; ++i){
		tui_setcursorpos(i, col1);
		printf("%s", "│");
		tui_setcursorpos(i, col2);
		printf("%s", "│");
	}
	tui_setcursorpos(row2, col1);
	printf("%s", "└");
	for (size_t i = col1 + 1; i <= col2 - 1; ++i){
		printf("%s", "─");
	}
	printf("%s", "┘");
	fflush(stdout);

	setlocale(LC_ALL, locale);
}

TUI_CONST const char* tui_strerr(int tuie){
	switch (tuie){
	case TUI_OK:
		return "No error";
	case TUI_ENOMEM:
		return "Out of memory";
	case TUI_EINVAL:
		return "Invalid value";
	case TUI_SET_BADATTR:
		return "Bad attribute type given to tui_win_set()";
	case TUI_ENOSPC:
		return "Not enough space on the screen";
	default:
		return "Unknown error. This is a bug.";
	}
}

int tui_win_make(tui_window* parent, tui_window** out){
	tui_window* ret = calloc(1, sizeof(*ret));
	if (!ret){
		return TUI_ENOMEM;
	}

	if (!parent){
		parent = stdwin;
	}
	__tui_addchild(parent, ret);

	*out = ret;
	return TUI_OK;
}

int tui_win_set(tui_window* win, int attr_type, ...){
	tui_gravity g;
	tui_window* wptr;
	int n;
	int q;
	va_list ap;

	va_start(ap, attr_type);

	switch (attr_type){
	case TUI_SET_GRAVITY:
		g = va_arg(ap, tui_gravity);
		if (((g & TUI_GRAV_LEFT) && (g & TUI_GRAV_RIGHT)) ||
			((g & TUI_GRAV_TOP)  && (g & TUI_GRAV_BOT))){
			va_end(ap);
			return TUI_EINVAL;
		}
		win->gravity = g;
		break;
	case TUI_SET_PARENT:
		wptr = va_arg(ap, tui_window*);
		win->parent = wptr;
		break;
	case TUI_SET_WIDTH:
		n = va_arg(ap, int);
		if (n < TUI_MATCH_PARENT){
			va_end(ap);
			return TUI_EINVAL;
		}
		win->width = n;
		break;
	case TUI_SET_HEIGHT:
		n = va_arg(ap, int);
		if (n < TUI_MATCH_PARENT){
			va_end(ap);
			return TUI_EINVAL;
		}
		win->height = n;
		break;
	case TUI_SET_X_PADDING:
		n = va_arg(ap, int);
		if (n < 0){
			va_end(ap);
			return TUI_EINVAL;
		}
		win->x_padding = n;
		break;
	case TUI_SET_Y_PADDING:
		n = va_arg(ap, int);
		if (n < 0){
			va_end(ap);
			return TUI_EINVAL;
		}
		win->y_padding = n;
		break;
	case TUI_SET_PADDING:
		n = va_arg(ap, int);
		q = tui_win_set(win, TUI_SET_X_PADDING, n);
		if (q != TUI_OK){
			va_end(ap);
			return q;
		}
		q = tui_win_set(win, TUI_SET_Y_PADDING, n);
		if (q != TUI_OK){
			va_end(ap);
			return q;
		}
		break;
	default:
		va_end(ap);
		return TUI_SET_BADATTR;
	}
	va_end(ap);
	return TUI_OK;
}

int tui_show(tui_window* win){
	tui_clear();
	__tui_calcchildareas(win);
	print_box(win);
	for (size_t i = 0; i < win->children.len; ++i){
		print_box(win->children.arr[i]);
	}
	return TUI_OK;
}

int tui_hide(void){
	return TUI_OK;
}

int tui_win_free(tui_window* win){
	free(win);
	return TUI_OK;
}
