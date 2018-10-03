/** @file window/cpos_gravity.c
 * @brief Positions child windows using the gravity strategy.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cpos_gravity.h"

int tui_grav_calcchildren(tui_window* win){
	tui_container remaining_area;
	int ret;

	// get all windows to their right positions before collisions
	for (size_t i = 0; i < win->children.len; ++i){
		tui_window* child = win->children.arr[i];

		if (child->width != TUI_MATCH_PARENT){
			if (child->gravity & TUI_GRAV_LEFT){
				child->pos.total.col_left = 0;
				child->pos.total.col_right = child->width;
			}
			else if (child->gravity & TUI_GRAV_RIGHT){
				child->pos.total.col_left = win->pos.usable.col_right - child->width;
				child->pos.total.col_right = win->pos.usable.col_right;
			}
			else{
				child->pos.total.col_left = (win->pos.usable.col_left + win->pos.usable.col_right) / 2 - child->width / 2;
				child->pos.total.col_right = child->pos.total.col_left + child->width;
			}
		}

		if (child->height != TUI_MATCH_PARENT){
			if (child->gravity & TUI_GRAV_TOP){
				child->pos.total.row_top = 0;
				child->pos.total.row_bot = child->height;
			}
			else if (child->gravity & TUI_GRAV_BOT){
				child->pos.total.row_top = win->pos.usable.row_bot - child->height;
				child->pos.total.row_bot = win->pos.usable.row_bot;
			}
			else{
				child->pos.total.row_top = (win->pos.usable.row_top + win->pos.usable.row_bot) / 2 - child->height / 2;
				child->pos.total.row_bot = child->pos.total.row_top + child->height;
			}
		}
	}

	// resolve collisions
	if ((ret = __tui_solvechildcollisions(win, &remaining_area)) != TUI_OK){
		return ret;
	}

	// set the match_parent window to max size
	for (size_t i = 0; i < win->children.len; ++i){
		tui_window* child = win->children.arr[i];

		if (child->width == TUI_MATCH_PARENT){
			child->pos.total.col_left  = remaining_area.col_left + 1;
			child->pos.total.col_right = remaining_area.col_right - 1;
		}
		if (child->height == TUI_MATCH_PARENT){
			child->pos.total.row_top   = remaining_area.row_top + 1;
			child->pos.total.row_bot   = remaining_area.row_bot - 1;
		}
	}

	for (size_t i = 0; i < win->children.len; ++i){
		__tui_calcusablespace(win->children.arr[i]);
	}
	return TUI_OK;
}
