/* Author: Marius Ingebrigtsen */

#include "common.h"

#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

/* Init-calloc size for tokenize_file(). */
#define INITSIZE 1024

/* Typedef's for general and use in source file. */
typedef struct dirent dirent_t;
typedef struct stat stat_t;


void fatal_error(char *errmsg, ...) {
	va_list args;

	fprintf(stderr, "%s: ", "Fatal Error");

	va_start(args, errmsg);

	vfprintf(stderr, errmsg, args);

	va_end(args);

	fputs("\n", stderr);

	exit(1);
}

char **tokenize_file(const char *file, int *n) {
	FILE *f;
	char **words, word[128+1];
	int i, size;

	f = fopen(file, "r");
	if(f == NULL) {
		fatal_error("Couldn't open file; \'%s\'. ", file);
	}

	/* Allocate initial size, with possible resizal. */
	size = INITSIZE;
	words = calloc(size, sizeof(char*));
	if(words == NULL) {
		fatal_error("Out of memory.");
	}

	/* Append words into array. */
	for(i = 0; !feof(f); i++) {
		/* Ignore other than [a-zA-Z0-9]. */
		fscanf(f, "%*[^a-zA-Z0-9]");

		/* 'fscanf' returns number of successful character put into 
		 * char*-args provided. 
		 * Read upto 128-characters, or until other than specified 
		 * characters are found, and put them into 'word'. */
		if( fscanf(f, "%128[a-zA-Z0-9]", word) != 1 ) {
			break;
		}

		/* Convert word to lower-case. */
		for(char *c = word; *c != '\0'; c++) {
			*c = (char)tolower(*c);
		}

		/* Dynamically reallocate array. */
		if(i >= size) {
			size *= 2;
			words = realloc(words, sizeof(char*) * size);
			if(words == NULL) {
				fatal_error("Out of memory.");
			}
		}

		/* Assign word into array. */
		words[i] = strdup(word);
		if(words[i] == NULL) {
			fatal_error("Out of memory.");
		}
	}
	if(fclose(f) < 0) {
		fatal_error("Unable to close file; \'%s\'. ", file);
	}

	/* Resize array to fit number of words, plus NULL terminator. */
	words = realloc(words, sizeof(char*) * (i+1));
	if(words == NULL) {
		fatal_error("Out of memory.");
	}
	/* NULL terminate array. */
	words[i] = NULL;

	/* Assign number of words if option taken. */
	if(n != NULL) {
		*n = i;
	}

	return words;

	/* Count number of words in 'file'. */
	/* Initialize word-count; for as long as end-of-file is not reached; increment word-count. */
	for(i = 0; !feof(f); i++) {
		fscanf(f, "%*[^a-zA-Z0-9]");/* Read and increment file-pointer until characters [a-zA-Z0-9] are encountered. */
		fscanf(f, "%*[a-zA-Z0-9]");	/* Read upto 128 characters, or until other than [a-zA-Z0-9] is encountered. Ignore all and only increment file-pointer. */
	}
	/* Allocate return-array. 
	 * i-number of char*'s, +1 for NULL terminator. */
	words = calloc(sizeof(char*), i + 1); 
	if(words == NULL) {
		fatal_error("Out of memory.");
	}

	/* Reset file-pointer to start of file. */
	rewind(f);
}

void tokenize_destroy(char **words) {
	for(int i = 0; words[i] != NULL; i++) {
		free(words[i]);
	}
	free(words);
}

/* Return 1 if argument type is regular file-type, 0 otherwise. */
static int isfile(const dirent_t *file) {
	const char *filename;
	stat_t buf;

	filename = file->d_name;

	/* Stat return 0 if ok, otherwise if error. */
	if(stat(filename, &buf) < 0) {
		fatal_error("Unable to examine attributes of dirent_t; \'%s\', in function: %s. ", filename, __func__);
	}

	if( (file->d_type == DT_REG) || (S_ISREG(buf.st_mode)) ) {
		return 1;
	}
	return 0;
}

/* Return 1 if argument is a directory-type, 0 otherwise. */
static int isdir(const dirent_t *dir) {
	const char *filename;
	stat_t buf;

	filename = dir->d_name;

	/* Stat return 0 if ok, otherwise if error. */
	if(stat(filename, &buf) < 0) {
		fatal_error("Unable to examine attributes of dirent_t; %s, in function: %s. ", dir->d_name, __func__);
	}

	/* If neither current or parent directory, and double confirmation on directory. */
	if( !(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) && 
		(dir->d_type == DT_DIR) && (S_ISDIR(buf.st_mode)) ) {
		return 1;
	}
	return 0;
}

static int _find_files(char ***files, int entries, const char *reldir) {
	int		numfiles, numdirs, i, end;
	char	*path;
	dirent_t**filelist, **dirlist;

	/* Scan directories. 
	 * Return paths added in list. 
	 * 1st argument is directory to look. 
	 * 2nd argument is list where desired paths are stored. 
	 * 3rd argument supplies selector-function. 
	 * 4th argument supplies sorting-function. */

	numfiles = scandir(".", &filelist, isfile, alphasort);
	if(numfiles < 0) {
		fatal_error("Unable to scan directory; \'%s\', in function \'%s\'. ", reldir, __func__);
	}
	
	numdirs = scandir(".", &dirlist, isdir, alphasort);
	if(numdirs < 0) {
		fatal_error("Unable to scan directory; \'%s\', in function \'%s\'. ", reldir, __func__);
	}

	/* Allocate space for new file-entries. */
	if(numfiles > 0) {
		*files = realloc(*files, sizeof(char*) * (entries + numfiles));
		if(*files == NULL) {
			fatal_error("Unable to re-allocate memory in function %s. ", __func__);
		}
	}

	/* Append files found in directory, and increment entry count. 
	 * 'i' is index for 'filelist'.
	 * 'entries' used for indexing 'files'. */
	for(i = 0, end = entries + numfiles;
		entries < end;
		i++, entries++) {
		
		/* dir + 1 to skip initial '.'-character. */
		path = concatenate_strings(3, reldir + 1, "/", filelist[i]->d_name);
	
		/* Beware: '[i]' occurs before '*'! */
		(*files)[entries] = path;

		/* Deallocate entry struct made by scandir. 
		 * NOTE: 'filelist[i]->d_name' is not allocated memory, but static string of file- or folder-name. */
		free(filelist[i]);
	}
	free(filelist);

	/* Recursively traverse all directories. */
	for(i = 0; i < numdirs; i++) {
		/* This is next recursion-levels' 'reldir'-argument. 
		 * Needed for correct representation in append-loop above. */
		path = concatenate_strings(3, reldir, "/", dirlist[i]->d_name);

		/* scandir only makes names and relative path to file or directory, 
		 * causing stat-function call to fail. */
		if(chdir(dirlist[i]->d_name) < 0) {
			fatal_error("Unable to change cwd to directory; \'%s\', in function; \'%s\'. \n", dirlist[i]->d_name, __func__);
		}
		entries = _find_files(files, entries, path);
		if(chdir("..") < 0) {
			fatal_error("Unable to revert cwd back to parent directory from cwd; \'%s\', in function; \'%s\'. \n", dirlist[i]->d_name, __func__);
		}

		free(path);
		free(dirlist[i]);
	}
	free(dirlist);

	return entries;
}

char **recursive_find_files(const char *rootdir, int *n) {
	char **files, cwd[512];
	int entries;

	/* Store cwd for switching back later. */
	if(getcwd(cwd, sizeof(cwd)) == NULL) {
		fatal_error("Unable to fetch current working directory in function: %s", __func__);
	}

	entries = 0;
	/* Initialize empty return-array for dynamic reallocation. */
	files = calloc(entries, sizeof(char*));
	if(files == NULL) {
		fatal_error("Out of memory in function %s. ", __func__);
	}

	/* Change working directory to argument path. */
	if(chdir(rootdir) < 0) {
		fatal_error("Unable to change directory to; \'%s\'. \n", rootdir);
	}

	/* Traverse directories, add all files to array, and return number of entries. */
	entries = _find_files(&files, entries, ".");

	/* Make space for NULL terminator. */
	files = realloc(files, sizeof(char*) * (entries + 1));
	if(files == NULL) {
		fatal_error("Unable to reallocate memory to size %d. ", sizeof(char*) * (entries + 1));
	}
	files[entries] = NULL;

	/* Assign number of entries if optional argument provided. */
	if(n != NULL) {
		*n = entries;
	}

	/* Reset working directory as was. */
	if(chdir(cwd) < 0) {
		fatal_error("Unable to revert cwd from \'%s\' back to \'%s\'. \n", rootdir, cwd);
	}
	
	return files;
}

char **find_files(const char *rootdir, int *n) {
	char	**files, *cmd, *line;
	FILE	*f;
	int		entries, i;
	size_t	len;

	/* Initialize return-array with one-slot for dynamic reallocation. */
	entries = INITSIZE;
	files = calloc(entries, sizeof(char*));
	if(files == NULL) {
		fatal_error("Out of memory in function; \'%s\'. \n", __func__);
	}

	/* Allocate command message. +25 is for static characters in 'sprintf' below. */
	cmd = calloc(strlen(rootdir) + 25, sizeof(char));
	if(cmd == NULL) {
		fatal_error("Out of memory in function; \'%s\'. \n", __func__);
	}
	/* Write command into buffer. */
	sprintf(cmd, "/usr/bin/find %s -not -type d", rootdir);

	/* Open terminal sub-process file-stream 
	 * that will feed all directory-entries found in 'rootdir', 
	 * as well as subsequent under-directories' entries, but exclude directories themselves. */
	f = popen(cmd, "r");
	if(f == NULL) {
		fatal_error("Unable to create process with popen. \n");
	}

	/* Set line and len to zero in order to make getline allocate memory for buffer. 
	 * Read from stream and append to return-array. */
	for(i = len = 0, line = NULL;
		!feof(f); 
		i++, len = 0, line = NULL) {

		/* Return characters read, -1 on error or end-of-file. */
		len = getline(&line, &len, f);
		if((int)len < 0) {
			/* If end-of-file, line is still allocated. */
			free(line);
			break;
		}
		/* Set newline to zero. */
		line[len-1] = '\0';

		/* Re-size array dynamically. */
		if(i >= entries) {
			entries *= 2;
			files = realloc(files, sizeof(char*) * entries);
			if(files == NULL) {
				fatal_error("Out of memory in function; \'%s\'. ", __func__);
			}
		}

		/* Assign path to return-array. */
		files[i] = line;
		if(files[i] == NULL) {
			fatal_error("Out of memory in function; \'%s\'. ", __func__);
		}
	}
	/* Fix array sizeof exactly to number of entries plus one for NULL terminator. */
	files = realloc(files, sizeof(char*) * (i+1));
	if(files == NULL) {
		fatal_error("Out of memory in function; \'%s\'. ", __func__);
	}
	files[i] = NULL;

	/* Close file-stream process. */
	if(pclose(f) != 0) {
		fatal_error("Unable to close process with 'pclose'. \n");
	}
	free(cmd);

	/* Assign number of entries if option taken. */
	if(n != NULL) {
		*n = i;
	}

	return files;
}

void find_files_destroy(char **files) {
	for(int i = 0; files[i] != NULL; i++) {
		free(files[i]);
	}
	free(files);
}

void *new_array(int n, size_t size) {
	void *array;

	array = calloc(n, size);
	if(array == NULL) {
		fatal_error("Out of memory in function %s. ", __func__);
	}

	return array;
}

int *new_integer(int value) {
	int *new;

	new = malloc(sizeof(int));
	if(new == NULL) {
		fatal_error("Out of memory.\n");
	}
	*new = value;

	return new;
}

char *generate_randomized_word(int maxlength) {
	char *word;
	int letters, length, i;

	letters = 26;
	length = (rand() % maxlength) + 1;

	word = malloc(sizeof(length + 1));
	if(word == NULL) {
		fatal_error("Out of memory.\n");
	}

	for(i = 0; i < length; i++) {
		if(rand() % 2 == 0) {
			word[i] = 'a' + (rand() % letters);
		} else {
			word[i] = 'A' + (rand() % letters);
		}
	}
	word[length] = '\0';

	return word;
}

char *str_reverse(char *str) {
	char tmp;
	int i, len;

	len = strlen(str) - 1;

	for(i = 0; i <= len; i++, len--) {
		
		tmp = str[i];

		str[i] = str[len];

		str[len] = tmp;
	}
	return str;
}

char *int_to_ascii(int num, int base) {
	char *str;
	int negative, rem, i;

	if(num != 0) {	/* Corner Case; If num is zero malloc return NULL or a pointer to an address that was freed. */
		str = malloc( (size_t)( ceil( log10(num) ) + 1) * sizeof(char) );
	} else {
		str = malloc(sizeof(char));
	}
	
	if(str == NULL) {
		fatal_error("Out of memory.\n");
	}

	negative = 0;
	i = 0;

	if(num == 0) {	/* If 'number' is zero. */
		*str = '0';

		str[1] = '\0';

		return str;
	}
	else if( (num < 0) && (base == 10) ) {
		negative = 1;

		num = -num;
	}

	while(num != 0) {
		rem = num % base;

		str[i] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		
		i++;

		num /= base;
	}

	if(negative) {
		str[i] = '-';
		i++;
	}

	str[i] = '\0';

	return str_reverse(str);
}

char *concatenate_strings(int num, const char *first, ...) {
	char	*str;
	int		length;
	va_list	args;

	length = strlen(first);

	va_start(args, first);
	for(int i = 1; i < num; i++) {
		length += strlen( va_arg(args, const char*) );
	}
	va_end(args);

	str = calloc(1, length + 1);
	if(str == NULL) {
		fatal_error("Out of memory.");
	}
	str = strcpy(str, first);

	va_start(args, first);
	for(int i = 1; i < num; i++) {
		str = strcat(str, va_arg(args, const char*) );
	}
	va_end(args);

	return str;
}

char *strbin(unsigned long long *num, int len) {
	char *bin;
	int spacecount, i;
	unsigned long long mask;

	if(len == 0) {
		return NULL;
	}

	/* Insert spaces every fourth bit. */
	spacecount = (len % 4) ? len % 4: 4;

	/* +2 for leading '0b...', +1 for NULL terminator, and len/4 for number of spaces. */
	len += 2 + 1 + (int)(len / 4);

	bin = calloc(len, sizeof(char));
	if(bin == NULL) {
		fatal_error("Out of memory in function; \'%s\'. ", __func__);
	}

	bin[0]	= '0';
	bin[1]	= 'b';
	mask	= (unsigned long long)1 << (len - 2 - (int)(len/4));

	for(i = 2; mask != 0; i++) {

		if(spacecount == 0) {
			bin[i] = ' ';
			spacecount = 5; /* Set to 5 to decrement sets it to 4. */
		}
		else if(*num & mask) {
			bin[i] = '1';
			mask = mask >> 1;
		}
		else {
			bin[i] = '0';
			mask = mask >> 1;
		}
		spacecount--;
	}
	bin[len-1] = 0;

	return bin;
}
