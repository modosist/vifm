#include "../config.h"

#if defined(HAVE_LIBGTK) || defined(HAVE_LIBMAGIC)

#ifdef HAVE_LIBGTK
#include <gtk/gtk.h>
#include <glib-2.0/gio/gio.h>
#endif

#ifdef HAVE_LIBMAGIC
#include <magic.h>
#endif

#include <sys/dir.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "status.h"

#include "file_magic.h"

static char* handlers;
static size_t handlers_len;

static int get_gtk_mimetype(const char* filename, char* buf);
static int get_magic_mimetype(const char* filename, char* buf);
static char * get_handlers(const char* mime_type);
static void enum_files(const char* path, const char* mime_type);
static void process_file(const char* path, const char* mime_type);
static int ends_with(const char* str, const char* suffix);
static void expand_desktop(const char* str, char* buf);

char*
get_magic_handlers(const char* file)
{
	char mimetype[128];

	if(get_gtk_mimetype(file, mimetype) == -1)
	{
		if(get_magic_mimetype(file, mimetype) == -1)
			return NULL;
	}

	return get_handlers(mimetype);
}

static int
get_gtk_mimetype(const char* filename, char* buf)
{
#ifdef HAVE_LIBGTK
	GFile* file;
	GFileInfo* info;

	if(!curr_stats.gtk_available)
		return -1;

	file = g_file_new_for_path(filename);
	info = g_file_query_info(file, "standard::", G_FILE_QUERY_INFO_NONE, NULL,
			NULL);
	if(info == NULL)
	{
		g_object_unref(file);
		return -1;
	}

	strcpy(buf, g_file_info_get_content_type(info));
	g_object_unref(info);
	g_object_unref(file);
	return 0;
#else /* #ifdef HAVE_LIBGTK */
	return -1;
#endif /* #ifdef HAVE_LIBGTK */
}

static int
get_magic_mimetype(const char* filename, char* buf)
{
#ifdef HAVE_LIBMAGIC
	magic_t magic;

	magic = magic_open(MAGIC_MIME_TYPE);
	if(magic == NULL)
		return -1;

	magic_load(magic, NULL);

	strcpy(buf, magic_file(magic, filename));

	magic_close(magic);
	return 0;
#else /* #ifdef HAVE_LIBMAGIC */
	return -1;
#endif /* #ifdef HAVE_LIBMAGIC */
}

char *
get_handlers(const char* mime_type)
{
	free(handlers);
	handlers = NULL;
	handlers_len = 0;

	enum_files("/usr/share/applications", mime_type);
	enum_files("/usr/local/share/applications", mime_type);

	return handlers;
}

static void
enum_files(const char* path, const char* mime_type)
{
	DIR* dir;
	struct dirent* dentry;
	const char* slash = "";

	dir = opendir(path);
	if(dir == NULL)
	{
		perror("opendir");
		return;
	}

	if(path[strlen(path) - 1] != '/')
		slash = "/";

	while((dentry = readdir(dir)) != NULL) {
		char buf[PATH_MAX];

		if(strcmp(dentry->d_name, ".") == 0)
			continue;
		else if (strcmp(dentry->d_name, "..") == 0)
			continue;

		snprintf(buf, sizeof (buf), "%s%s%s", path, slash, dentry->d_name);
		if(dentry->d_type == DT_DIR)
			enum_files(buf, mime_type);
		else
			process_file(buf, mime_type);
	}

	if(closedir(dir) != 0)
		perror("closedir");
}

static void
process_file(const char* path, const char* mime_type)
{
	FILE* f;
	char *p;
	char exec_buf[1024] = "";
	char mime_type_buf[2048] = "";
	char buf[2048];

	if(!ends_with(path, ".desktop"))
		return;

	f = fopen(path, "r");
	if(f == NULL)
	{
		perror("fopen");
		return;
	}

	while(fgets(buf, sizeof (buf), f) != NULL)
	{
		size_t len = strlen(buf);

		if(buf[len - 1] == '\n')
			buf[len - 1] = '\0';

		if(strncmp(buf, "Exec=", 5) == 0)
			strcpy(exec_buf, buf);
		else if (strncmp(buf, "MimeType=", 9) == 0)
			strcpy(mime_type_buf, buf);
	}

	fclose(f);

	if(strstr(mime_type_buf, mime_type) == NULL)
		return;
	if(exec_buf[0] == '\0')
		return;

	expand_desktop(exec_buf + 5, buf);
	p = realloc(handlers, handlers_len + 1 + strlen(buf) + 1);
	if(p == NULL)
		return;

	handlers = p;
	if(handlers_len == 0)
		*handlers = '\0';
	else
		strcat(handlers, ",");
	handlers_len += 1 + strlen(buf);
	strcat(handlers, buf);
}

static int
ends_with(const char* str, const char* suffix)
{
	size_t str_len = strlen(str);
	size_t suf_len = strlen(suffix);

	if(str_len < suf_len)
		return 0;
	else
		return (strcmp(suffix, str + str_len - suf_len) == 0);
}

static void
expand_desktop(const char* str, char* buf)
{
	int substituted = 0;
	while(*str != '\0')
	{
		if(*str != '%')
		{
			*buf++ = *str++;
			continue;
		}
		str++;
		if(*str == 'c')
		{
			substituted = 1;
			strcpy(buf, "caption");
			buf += strlen(buf);
		}
		else if(strchr("Uuf", *str) != NULL)
		{
			substituted = 1;
			strcpy(buf, "%f");
			buf += strlen(buf);
		}
		str++;
	}
	if(substituted)
		*buf = '\0';
	else
	{
		*buf = ' ';
		strcpy(buf + 1, "%f");
	}
}

#endif /* #if defined(HAVE_LIBGTK) || defined(HAVE_LIBMAGIC) */

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab : */