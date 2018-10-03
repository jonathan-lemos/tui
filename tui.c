#include "attribute.h"
#include "ctermtools/ctermtools.h"
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define FLAG_NO_HL_ROW (1 << 0)

static size_t AT_CONST middle_col(size_t str_len){
	return (tt_getcols() - str_len) / 2;
}

static int print_middle_screen(const char* msg){
	char* tmp;
	char* tmp_tok = NULL;
	size_t n_lines = 0;
	size_t cur_line;
	int rows = tt_getrows();
	size_t i;

	tmp = malloc(strlen(msg) + 1);
	if (!tmp){
		errno = ENOMEM;
		return -1;
	}
	strcpy(tmp, msg);

	for (i = 0; i < strlen(tmp); ++i){
		if (tmp[i] == '\n'){
			n_lines++;
		}
	}
	cur_line = (rows - n_lines) / 2;

	tmp_tok = strtok(tmp, "\n");
	while (tmp_tok != NULL){
		tt_setcursorpos(cur_line, middle_col(strlen(tmp_tok)));
		printf("%s", tmp_tok);
		cur_line++;
		tmp_tok = strtok(NULL, "\n");
	}

	free(tmp);
	return 0;
}

static void print_entry(const char* entry, size_t row, uint_fast16_t flags){
	size_t cols = tt_getcols();
	tt_setcursorpos(row, 0);

	if (!(flags & FLAG_NO_HL_ROW)){
		tt_attron(TT_INVERT);
	}
	for (size_t i = 0; i < middle_col(strlen(entry)); ++i){
		putchar(' ');
	}

	tt_attron(TT_INVERT);
	printf("%s", entry);

	if (flags & FLAG_NO_HL_ROW){
		tt_attroff(TT_INVERT);
	}
	for (size_t i = middle_col(strlen(entry)) + strlen(entry); i < cols; ++i){
		putchar(' ');
	}
	tt_attroff(TT_INVERT);
}

static void print_box(int row1, int col1, int row2, int col2){
	int i;
	const char* locale = setlocale(LC_ALL, NULL);

	if (row1 > row2){
		int tmp = row1;
		row1 = row2;
		row2 = tmp;
	}
	if (col1 > col2){
		int tmp = col1;
		col1 = col2;
		col2 = tmp;
	}

	setlocale(LC_ALL, "");
	tt_setcursorpos(row1, col1);
	printf("%s", "┌");
	for (i = col1 + 1; i <= col2 - 1; ++i){
		printf("%s", "─");
	}
	printf("%s", "┐");
	for (i = row1 + 1; i <= row2 - 1; ++i){
		tt_setcursorpos(i, col1);
		printf("%s", "│");
		tt_setcursorpos(i, col2);
		printf("%s", "│");
	}
	tt_setcursorpos(row2, col1);
	printf("%s", "└");
	for (i = col1 + 1; i <= col2 - 1; ++i){
		printf("%s", "─");
	}
	printf("%s", "┘");

	setlocale(LC_ALL, locale);
}

size_t display_menu(const char* const* choices, size_t choices_len, uint_fast16_t flags){

}
