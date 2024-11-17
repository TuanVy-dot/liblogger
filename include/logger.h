#ifndef LOGGER

#include <stdio.h>

enum LOGLEVEL {
    TRACE, DEBUG, INFO, WARNING, ERROR, FATAL, OFF
};
typedef enum LOGLEVEL log_level_t;

typedef struct LOGGER LOGGER;

/* Core */

/* Logger creation and initiallization 
 * Return the logger or
 * NULL if fail */
LOGGER *logger_create(const char *ref, FILE *file, 
        const char *format, const log_level_t level);

/* Logger removal
 * free the format(as it is allocated linked list) and the logger */
void logger_remove(LOGGER *logger);

/* logging */

/* print the log message
 * Print the msg with format defined in logger if level > logger's level 
 * Support formatted string, this is not suppose to be call, use macros instead */
void __logger_msg__(const char *fname, int line, 
                    const LOGGER *logger, const log_level_t level, 
                    const char *msg, ...);

/* print the msg then call print_element for each pointer to each element of the array */
void __logger_log_array__(const char *fname, const int line, 
                          const LOGGER *logger, const log_level_t level, 
                          void *array, size_t element_size, size_t len,
                          void (*print_element)(FILE *, void *), const char *msg, ...);


/* Change logger config */

void logger_change_ref(LOGGER *logger, const char *ref);
void logger_change_file(LOGGER *logger, FILE *file);
void logger_change_format(LOGGER *logger, const char *format);
void logger_change_level(LOGGER *logger, const int level);
void logger_change_rffl(LOGGER *logger, const char *ref, FILE *file, const char *format, const int level);
/* Those are self-explanatory, paste NULL or -1 to keep it unchanged */

/* Coloring levels label */

/* change color of a level */
void logger_level_color(const log_level_t level, const char *ansi_code);

/* use the default color set */
void logger_level_color_default(void);

/* no color */
void logger_level_color_reset(void);

/* Though I called it color, any string will work
 * it basically prefix the label with const char *ansi_code
 * So you can make it "level: " and it will be level: INFO\033[0m */

#endif

