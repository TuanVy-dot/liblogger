#ifndef LOGGER

#include <stdio.h>
#include "linked_list.h"
#include <stdarg.h>

/*
 * YOU MUST USE logger_delete TO REMOVE A LOGGER
 * it used memory allocation and will lead to memory leak otherwise
 * It does not free the file pointer and reference, by the way, and you have to
 * free the struct yourself
 *
 * When you want to change some values of a logger:
 * DO NOT use logger_init it cause memory leaks because it will not free the pointer
 * DO NOT change values directly, it can be undefined
 * Instead use logger_change_format, logger_change_file, logger_change_level
 * or logger_change_ffl (put NULL to not change something or -1 in case of int)
 * DO NOT use those if the logger is not initiallized using logger_init
 * It will free the pointers which should not be static or unallocated
 * It does not free the file though so it is fine.
 * reference of a logger SHOULD NOT be change because why would you ever want that?
 * But you are free to do so by change the value of the struct LOGGER
 * mylg.ref = str;
 * Just don't forget to free if it is allocated
 *
 * See prototypes for more infos
 *
 * How to use:
 * Declare a LOGGER variable (can be global)
 * LOGGER mylogger;
 * Then inside a function that gonna get run, use
 * logger_init(LOGGER*, char *ref, FILE *file, char *format, int level)
 * LOGGER* - a pointer to the logger
 * char *ref - reference of the logger, use in format
 * FILE *file - a file pointer that gonna get write to
 * char *format - format of logging messages
 * int level - The lowest level that going to get print by that logger
 *
 * Format:
 * Use those keywords:
 * REF, LEVEL, FILENAME, LINE, DATE, TIME, MSG
 * enclosed in brackets to get dynamic values, use / for escaping brackets, such as
 * "[DATE] - [TIME] /[[REF]/] - [LEVEL] | ([FILENAME:LINE]) [MSG]"
 * Will give something like
 * 00/00/0000 - 00:00 [mylogger] - INFO | (main.c:69) x = 5
 * You can use ANSI code for colors but must prefix every [ with / to escape
 * Such as: "\033/[1;34m/[[REF]/]\033/[0m \033/[1;32m/[[LEVEL]/]\033/[0m - \033/[1;37m/[[MSG]/]\033/[0m";
 * You can also use logger_level_color(int level, char *ansi_code);
 * This will prefix every print of the level string
 * So it can effectively change the colors or appearence of the level label
 * You don't need to prefix [ with / here, it is normal ansi code
 * It will automatically suffix it to back to the normal state
 * (not what it is in the string, the default one: 033[0m)
 * If it is a part of another string that used ansi code,
 * you must manually re-do it
 * You can also call logger_level_color_default(void) to use our defined set
 * It is called color but technically any escape sequences work
 * Not even need to be esc sequence, it will just prefix it with what setted
 * And suffix with \033[0m (only if the color is setted)
 * You can set ansi_code to NULL or 0, it will remove the color
 * and will use the normal appearence
 * To quickly remove all colors, use logger_level_color_reset(void)
 * Colors will overwrite code in format
 *
 * The colors are set globally to all loggers
 *
 * Look below for parameters
 */

#define DEFAULT_LOG_FORMAT "/[[REF]/]/[[LEVEL]/] - ([FILENAME]:[LINE])\n- [MSG]\n"

#define logger_log(logger, level, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, level, msg, ##__VA_ARGS__)
// automatic parameters for log call, you should use this instead
// of calling directly the __logger_msg__
// or use the following

/* Convinient macros for logging (automatic logging level) */
#define logger_trace(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, TRACE, msg, ##__VA_ARGS__)
#define logger_debug(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, DEBUG, msg, ##__VA_ARGS__)
#define logger_info(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, INFO, msg, ##__VA_ARGS__)
#define logger_warning(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, WARNING, msg, ##__VA_ARGS__)
#define logger_error(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, ERROR, msg, ##__VA_ARGS__)
#define logger_fatal(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, FATAL, msg, ##__VA_ARGS__)

enum LOGLEVEL {
    TRACE, DEBUG, INFO, WARNING, ERROR, FATAL, OFF
};

typedef struct {
    const char *ref;
    const FILE *file; // destination file
    const Node *format; // printing format
    unsigned char level; // lowest level to be print
} LOGGER;

void __logger_msg__(const char *fname, int line, \
                const LOGGER logger, int level, const char *msg, ...);
void logger_init(LOGGER *logger, const char *ref, const FILE *file, const char *format, const int level);
void logger_remove(LOGGER *logger);
void logger_change_file(LOGGER *logger, const FILE *file);
void logger_change_format(LOGGER *logger, const char *format);
void logger_change_level(LOGGER *logger, const int level);
void logger_change_ffl(LOGGER *logger, const FILE *file, const char *format, const int level);
void logger_level_color(const int level, const char *ansi_code);
void logger_level_color_default(void);
void logger_level_color_reset(void);

#endif

