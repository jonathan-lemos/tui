/** @file window/window.h
 * @brief Defines the window structure as well as its creation methods.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __TUI_WINDOW_H
#define __TUI_WINDOW_H

#include "../attribute.h"
#include "cpos_gravity.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct tui_container{
	int row_top;
	int row_bot;
	int col_left;
	int col_right;
}tui_container;

typedef enum tui_cpos_type{
	TUI_CPOS_GRAVITY
}tui_cpos_type;

typedef struct tui_cpos{
	tui_cpos_type type;

}

/**
 * @brief Main window structure.
 * The width and height of the window are specified as well as the window's "gravity", determining its position within its parent.
 * Note that a relative window can only be specified if the window has a parent.
 */
typedef struct tui_window{
	/**
	 * @brief The gravity of the window.
	 * Depending on the height/width/constraints, this may not be used.
	 */
	tui_gravity gravity;

	/**
	 * @brief The width of the window.
	 * This can be TUI_MATCH_PARENT to take up the maximum space.
	 */
	int width;

	/**
	 * @brief The height of the window.
	 * This can be TUI_MATCH_PARENT to take up the maximum space.
	 */
	int height;

	/**
	 * @brief The x-padding of the window.
	 * This is how much padding off the left or right edge of other windows there is.
	 */
	int x_padding;

	/**
	 * @brief The y-padding of the window.
	 * This is how much padding off the top or bottom edge of other windows there is.
	 */
	int y_padding;

	/**
	 * @brief The parent of the current window.
	 */
	struct tui_window* parent;

	/**
	 * @brief The children of the current window.
	 */
	struct tui_children{
		struct tui_window** arr;
		size_t len;
	}children;

	/**
	 * @brief The true coordinates of this window.
	 */
	struct tui_area{
		tui_container total;
		tui_container usable;
	}pos;
}tui_window;

#define TUI_OK          (0)
#define TUI_ENOMEM      (1)
#define TUI_EINVAL      (2)
#define TUI_SET_BADATTR (3)
#define TUI_ENOSPC      (4)

#define TUI_SET_GRAVITY       (1)
#define TUI_SET_PARENT        (2)
#define TUI_SET_PLACE_LEFTOF  (3)
#define TUI_SET_PLACE_RIGHTOF (4)
#define TUI_SET_PLACE_ABOVE   (5)
#define TUI_SET_PLACE_BELOW   (6)
#define TUI_SET_WIDTH         (7)
#define TUI_SET_HEIGHT        (8)
#define TUI_SET_X_PADDING     (9)
#define TUI_SET_Y_PADDING     (10)
#define TUI_SET_PADDING       (11)

#define TUI_MATCH_PARENT (-1)

/**
 * @brief Do not call this function directly. Use the stdwin macro instead.
 */
TUI_CONST tui_window* __getstdwin();

/**
 * This macro refers to the standard window of this terminal.
 * This is the window that takes up all of the terminal's space.
 * All other windows should be children of this window.
 */
#define stdwin __getstdwin()

TUI_CONST const char* tui_strerr(int errc);

int tui_win_make(tui_window* parent, tui_window** out);
int tui_win_set(tui_window* win, int attr_type, ...);

int tui_show(tui_window* win);
int tui_hide(void);

int tui_win_free(tui_window* win);

#endif
