/*********************************************************************
 *
 * Copyright (C) 2020 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 
#include <string.h>
#include <dirent.h>
#include <stdlib.h> 
#include <stdbool.h>
#include "fileman.h"


/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_read(char *fname, size_t foffset, char *buf, size_t boffset, size_t size) {
	int fd = open(fname, O_RDONLY);
	if(fd < 0) return -1;
	lseek(fd, foffset, SEEK_CUR);
	size_t bytes_read = read(fd, buf + boffset, size);
	close(fd);
	return bytes_read;
}

/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_write(char *fname, size_t foffset, char *buf, size_t boffset, size_t size) {
	int check_if_exist = open(fname, O_WRONLY);
	if(check_if_exist >= 0) {
		close(check_if_exist);
		return -1;
	}
	creat(fname, S_IRWXU);
	int fd = open(fname, O_WRONLY);
	lseek(fd, foffset, SEEK_CUR);
	size_t bytes_written = write(fd, buf + boffset, size);
	close(fd);
	return bytes_written;
}

/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_append(char *fname, char *buf, size_t size) {
	int fd = open(fname, O_WRONLY | O_APPEND);
	if(fd < 0) return fd;
	size_t bytes_appended = write(fd, buf, size);
	close(fd);
	return bytes_appended;
}

/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_copy(char *fsrc, char *fdest) {
	int fd_src = open(fsrc, O_RDONLY);
	if(fd_src < 0) return fd_src;
	int check_if_exist = open(fdest, O_WRONLY);
	if(check_if_exist >= 0) {
		close(check_if_exist);
		return -1;
	}
	creat(fdest, S_IRWXU);
	int fd_dest = open(fdest, O_WRONLY);
	// char buffer[4096];
	// size_t bytes_copied = 0, bytes_read = 0, bytes_written = 0;
	// do {
	// 	bytes_read = read(fd_src, &buffer, 4096);
	// 	lseek(fd_src, bytes_read, SEEK_CUR);
	// 	bytes_written = write(fd_dest, &buffer, bytes_read);
	// 	lseek(fd_dest, bytes_written, SEEK_CUR);
	// 	bytes_copied += bytes_written;
	// 	printf("%s%d\n", "bytes copied: ", bytes_copied);
	// 	memset(&buffer, '\0', 4096);
	// } while(bytes_written > 0);
	char buffer[1<<20];
	int bytes_copied = write(fd_dest, &buffer, read(fd_src, &buffer, 1<<20));
	close(fd_src);
	close(fd_dest);
	return bytes_copied;
}

void add_spaces(int fd, int depth) {
	for(int i = 0; i < depth; i++)
		dprintf(fd, "%s", "    ");
}

static int compare(const void* x, const void* y) {
	return strcmp(*(const char**)x, *(const char**)y);
}

void sort(const char *arr[], int n) {
	qsort(arr, n, sizeof(const char*), compare);
}

void fileman_dir_helper(int fd, char *curr_path, const char *dname, int depth) {
	DIR *dir = opendir(curr_path);
	add_spaces(fd, depth);
	dprintf(fd, "%s", dname);
	dprintf(fd, "%s", "\n");
	if(dir == NULL) return;
	struct dirent *d;
	const char *dir_entries[16];
	int count = 0;
	while((d = readdir(dir)) != NULL) {
		if(strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
			dir_entries[count] = d->d_name;
			count++;
		}
	}
	sort(dir_entries, count);
	char new_path[256];
	for(int i = 0; i < count; i++) {
		strcpy(new_path, curr_path);
		strcat(new_path, "/");
		strcat(new_path, dir_entries[i]);
		fileman_dir_helper(fd, new_path, dir_entries[i], depth + 1);
	}
}

/*
 * You need to implement this function, see fileman.h for details 
 */
// https://codeforwin.org/2018/03/c-program-to-list-all-files-in-a-directory-recursively.html
// https://www.geeksforgeeks.org/c-program-sort-array-names-strings/
// David Harrison
void fileman_dir(int fd, char *dname) {
	fileman_dir_helper(fd, dname, dname, 0);
	close(fd);
}

char *a = "\u251C\u2500\u2500 "; // ├── 
char *b = "\u2514\u2500\u2500 "; // └── 
char *c = "\u2502   "; // │   
char *d = "    "; //    

void add_struct(int fd, int depth, bool *layout) {
	for(int i = 0; i < depth; i++)
		dprintf(fd, "%s", i + 1 == depth ? (layout[i] ? b : a) : (layout[i] ? d : c));
}

void fileman_tree_helper(int fd, char *curr_path, const char *dname, int depth, bool *parent_layout) {
	DIR *dir = opendir(curr_path);
	/* Display tree structure. */
	add_struct(fd, depth, parent_layout);
	dprintf(fd, "%s", dname);
	dprintf(fd, "%s", "\n");
	if(dir == NULL) return;
	/* Get all entries in current directory and sort them. */
	struct dirent *d;
	const char *dir_entries[16];
	int count = 0;
	while((d = readdir(dir)) != NULL) {
		if(strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
			dir_entries[count] = d->d_name;
			count++;
		}
	}
	sort(dir_entries, count);
	char new_path[256];
	/* Need array of booleans that will determine which block to print out. */
	bool layout[depth + 1];
	if(parent_layout != NULL) { // Edge case to handle first sub directory.
		/* Keep track of parent's layout. */
		for(int i = 0; i < depth; i++)
			layout[i] = parent_layout[i];
	}
	for(int i = 0; i < count; i++) {
		/* Build path the sub file/directory. */
		strcpy(new_path, curr_path);
		strcat(new_path, "/");
		strcat(new_path, dir_entries[i]);
		/* Update the layout the current file/directory, depending on if it
		is the last sibling or not. */
		layout[depth] = i + 1 == count;
		/* Recurse on the sub problem. */
		fileman_tree_helper(fd, new_path, dir_entries[i], depth + 1, layout);
	}
}

/*
 * You need to implement this function, see fileman.h for details 
 */
void fileman_tree(int fd, char *dname) {
	fileman_tree_helper(fd, dname, dname, 0, NULL);
	close(fd);
}



















