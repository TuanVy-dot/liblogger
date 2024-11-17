#ifndef LOGGER

#include <stdio.h>

/*
 *
 * A lightweight logging module that is just simply a better printf!
 *
 * NO MULTI THREADS SAFETY IMPLEMENTED
 * YOU MUST USE logger_delete TO REMOVE A LOGGER
 * it used memory allocation and will lead to memory leak otherwise
 * It does not free the file pointers and references
 *
 * When you want to change some values of a logger:
 * use logger_change_format, logger_change_file, 
 * logger_change_level, logger_change_ref
 * or logger_change_rffl (put NULL to not change something or -1 in case of int)
 * Change the reference using logger_change_ref, just be careful about memory
 * it does not free
 *
 * See prototypes for more info about arguements
 *
 * How to use:
 * Declare a LOGGER* variable (can be global)
 * LOGGER *mylogger;
 * Then inside a function that gonna get run, use
 * mylogger = logger_create(char *ref, FILE *file, char *format, log_level_t level)
 * char *ref - reference of the logger, use in format
 * FILE *file - a file pointer that gonna get write to
 * char *format - format of logging messages
 * log_level_t level - The lowest level that going to get print by that logger
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

/* FORMATTED LOG MESSAGES 
 * You must provide at least 1 variadic arguement
 * If you don't, use the unformatted one
 * Just the way macros work in standard C 
 * (I trying to remove all extensions usage) */
#define logger_logf(logger, level, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, level, msg, __VA_ARGS__)
// Use this instead of __logger_msg__

#define logger_tracef(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, TRACE, msg, __VA_ARGS__)
#define logger_debugf(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, DEBUG, msg, __VA_ARGS__)
#define logger_infof(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, INFO, msg, __VA_ARGS__)
#define logger_warningf(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, WARNING, msg, __VA_ARGS__)
#define logger_errorf(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, ERROR, msg, __VA_ARGS__)
#define logger_fatalf(logger, msg, ...) \
__logger_msg__(__FILE__, __LINE__, logger, FATAL, msg, __VA_ARGS__)

#define logger_log(logger, level, msg) \
__logger_msg__(__FILE__, __LINE__, logger, level, msg)
// Use this instead of __logger_msg__

#define logger_trace(logger, msg) \
__logger_msg__(__FILE__, __LINE__, logger, TRACE, msg)
#define logger_debug(logger, msg) \
__logger_msg__(__FILE__, __LINE__, logger, DEBUG, msg)
#define logger_info(logger, msg) \
__logger_msg__(__FILE__, __LINE__, logger, INFO, msg)
#define logger_warning(logger, msg) \
__logger_msg__(__FILE__, __LINE__, logger, WARNING, msg)
#define logger_error(logger, msg) \
__logger_msg__(__FILE__, __LINE__, logger, ERROR, msg)
#define logger_fatal(logger, msg) \
__logger_msg__(__FILE__, __LINE__, logger, FATAL, msg)

enum LOGLEVEL {
    TRACE, DEBUG, INFO, WARNING, ERROR, FATAL, OFF
};
typedef enum LOGLEVEL log_level_t;

typedef struct LOGGER LOGGER;

/* Order here is not the same in the source (I am sorry) 
 * I will try to be more organize on later versions */

/* FUNCTIONALITIES */
LOGGER *logger_create(const char *ref, const FILE *file, const char *format, const log_level_t level);
/* Logger creation and initiallization 
 * Return the logger or
 * NULL if fail */

void logger_remove(LOGGER *logger);
/* Logger removal
 * free the format(as it is linked list) and the logger */


void __logger_msg__(const char *fname, int line, \
                const LOGGER *logger, const log_level_t level, const char *msg, ...);
/* print the log message
 * Print the msg with format defined in logger if level > logger's level 
 * Support formatted string, this is not suppose to be call, use macros instead */

/* Change logger param */
void logger_change_file(LOGGER *logger, const FILE *file);
void logger_change_format(LOGGER *logger, const char *format);
void logger_change_level(LOGGER *logger, const int level);
void logger_change_ref(LOGGER *logger, const char *ref);
void logger_change_rffl(LOGGER *logger, const char *ref, const FILE *file, const char *format, const int level);
/* Those are self-explanatory, paste NULL or -1 to keep it unchanged */

/* Coloring levels label */
void logger_level_color(const log_level_t level, const char *ansi_code);
/* change color of a level */
void logger_level_color_default(void);
/* use default color */
void logger_level_color_reset(void);
/* no color */
/* Though I called it color, any string will work
 * it basically prefix the label with const char *ansj_code
 * So you can make it "level: " and it will be level: INFO\033[0m */

#endif

