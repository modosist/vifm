/* vifm
 * Copyright (C) 2001 Ken Steen.
 * Copyright (C) 2011 xaizek.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <curses.h>

#include <stddef.h> /* size_t */

#include "../utils/fs_limits.h"
#include "../color_scheme.h"
#include "../types.h"
#include "../ui.h"

#define VIFM_HELP "vifm-help.txt"
#define SCRIPTS_DIR "scripts"

typedef enum
{
	DD_ROOT_PARENT    = 1 << 0,
	DD_NONROOT_PARENT = 1 << 1,
	NUM_DOT_DIRS      =      2
}DotDirs;

/* Indexes for cfg.decorations. */
enum
{
	DECORATION_PREFIX, /* The symbol, which is prepended to file name. */
	DECORATION_SUFFIX, /* The symbol, which is appended to file name. */
};

typedef struct
{
	char home_dir[PATH_MAX]; /* ends with a slash */
	char config_dir[PATH_MAX];
	char trash_dir[PATH_MAX];
	char log_file[PATH_MAX];
	char *vi_command;
	int vi_cmd_bg;
	char *vi_x_command;
	int vi_x_cmd_bg;
	int num_bookmarks; /* Number of active bookmarks (set at the moment) */
	int use_trash;
	int vim_filter;
	int use_screen;
	int use_vim_help;
	int history_len;

	int auto_execute;
	int show_one_window;
	long max_args;
	int use_iec_prefixes;
	int wrap_quick_view;
	char *time_format;
	char *fuse_home; /* This one should be set using set_fuse_home() function. */

	char **search_history;
	int search_history_num;

	char **cmd_history;
	int cmd_history_num;

	char **prompt_history;
	int prompt_history_num;

	col_scheme_t cs;

	int undo_levels; /* Maximum number of changes that can be undone. */
	int sort_numbers; /* Natural sort of (version) numbers within text. */
	int follow_links; /* Follow links on l or Enter. */
	int confirm; /* Ask user about permanent deletion of files. */
	int fast_run;
	int wild_menu;
	int ignore_case;
	int smart_case;
	int hl_search;
	int vifm_info;
	int auto_ch_pos;
	char *shell;
	int timeout_len;
	int scroll_off;
	int gdefault;
#ifndef _WIN32
	char *slow_fs_list;
#endif
	int scroll_bind;
	int wrap_scan;
	int inc_search;
	int selection_is_primary; /* For yy, dd and DD: act on selection not file. */
	int tab_switches_pane; /* Whether <tab> is switch pane or history forward. */
	int last_status;
	int tab_stop;
	char *ruler_format;
	char *status_line;
	int lines; /* Terminal height in lines. */
	int columns; /* Terminal width in characters. */
	int dot_dirs; /* Controls displaying of dot directories. */
	char decorations[FILE_TYPE_COUNT][2]; /* Prefixes and suffixes of files. */
	int trunc_normal_sb_msgs; /* Truncate normal status bar messages if needed. */
}config_t;

extern config_t cfg;

void init_config(void);
void set_config_paths(void);
/* Sources vifmrc file (pointed to by the $MYVIFMRC). */
void source_config(void);
/* Returns non-zero on error. */
int source_file(const char filename[]);
/* Checks whether vifmrc file (pointed to by the $MYVIFMRC) has old format.
 * Returns non-zero if so, otherwise zero is returned. */
int is_old_config(void);
int are_old_color_schemes(void);
const char * get_vicmd(int *bg);
/* Generates name of file inside tmp folder. */
void generate_tmp_file_name(const char prefix[], char buf[], size_t buf_len);
void create_trash_dir(void);
/* Changes size of all histories. */
void resize_history(size_t new_len);
/* Sets value of cfg.fuse_home.  Returns non-zero in case of memory allocation
 * error. */
int set_fuse_home(const char new_value[]);
/* Sets whether screen support is enabled. */
void set_use_screen(int use_screen);
/* Frees memory previously allocated for specified history items. */
void free_history_items(const history_t history[], size_t len);

#endif

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 : */
