#include <stdlib.h>
#include <string.h>

#include "filelist.h"
#include "ui.h"

#include "dir_stack.h"

struct stack_entry {
	char* lpane_dir;
	char* lpane_file;
	char* rpane_dir;
	char* rpane_file;
};

static struct stack_entry * stack;
static unsigned int stack_size;
static unsigned int stack_top;

static void free_entry(const struct stack_entry * entry);

/*
 * Returns 0 on success and -1 when not enough memory
 */
int
pushd(void)
{
	if(stack_top == stack_size)
	{
		struct stack_entry* s = realloc(stack, (stack_size + 1)*sizeof(*stack));
		if(s == NULL)
			return -1;

		stack = s;
		stack_size++;
	}

	stack[stack_top].lpane_dir = strdup(lwin.curr_dir);
	stack[stack_top].lpane_file = strdup(lwin.dir_entry[lwin.list_pos].name);
	stack[stack_top].rpane_dir = strdup(rwin.curr_dir);
	stack[stack_top].rpane_file = strdup(rwin.dir_entry[rwin.list_pos].name);

	if(stack[stack_top].lpane_dir == NULL || stack[stack_top].lpane_file == NULL
			|| stack[stack_top].rpane_dir == NULL
			|| stack[stack_top].rpane_file == NULL)
	{
		free_entry(&stack[stack_top]);
		return -1;
	}

	stack_top++;

	return 0;
}

/*
 * Returns 0 on success and -1 on underflow
 */
int
popd(void)
{
	if(stack_top == 0)
		return -1;

	stack_top -= 1;

	change_directory(&lwin, stack[stack_top].lpane_dir);
	load_dir_list(&lwin, 0);

	change_directory(&rwin, stack[stack_top].rpane_dir);
	load_dir_list(&rwin, 0);

	moveto_list_pos(curr_view, curr_view->list_pos);

	return 0;
}

static void
free_entry(const struct stack_entry * entry)
{
	free(entry->lpane_dir);
	free(entry->lpane_file);
	free(entry->rpane_dir);
	free(entry->rpane_file);
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab : */