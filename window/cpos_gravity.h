/** @file window/cpos_gravity.h
 * @brief Positions child windows using the gravity strategy.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __TUI_WINDOW_CPOS_GRAVITY
#define __TUI_WINDOW_CPOS_GRAVITY

#include "window.h"

typedef enum tui_gravity{
	TUI_GRAV_CENTER = 0,
	TUI_GRAV_LEFT   = 1 << 0,
	TUI_GRAV_RIGHT  = 1 << 1,
	TUI_GRAV_TOP    = 1 << 2,
	TUI_GRAV_BOT    = 1 << 3
}tui_gravity;

int tui_grav_calcchildren(tui_window* win);

#endif
