#include "logger.h"
#include "linked_list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

/* 
 * CASTING IN PRINTF, FREE OR OTHER STD FUNCTIONS ARE JUST FOR
 * REMOVE THE CONST QUALIFIER AND REMOVE COMPILE WARNING
 */

#define LOGGER_DEFAULT_TRACE   "\033[0;37m"  // Light Gray
#define LOGGER_DEFAULT_DEBUG   "\033[0;34m"  // Blue
#define LOGGER_DEFAULT_INFO    "\033[0;32m"  // Green
#define LOGGER_DEFAULT_WARNING "\033[0;33m"  // Yellow
#define LOGGER_DEFAULT_ERROR   "\033[0;31m"  // Red
#define LOGGER_DEFAULT_FATAL   "\033[1;31m"  // Bold Red
#define LOGGER_RESET           "\033[0m"      // Reset Color

static const char *LOGGER_LEVEL_COLORS[6] = {0};

const char *LOGGER_INFO_LABELS[] = {
    "REF", "LEVEL", "DATE", "TIME", "FILENAME", "LINE", "MSG"
};

enum LOGGER_INF_LABELS_ID {
    REF, LEVEL, DATE, TIME, FILENAME, LINE, MSG
};

static Node *logger_formatter(const char*);
static int logger_getID(const char*);
void logger_init(LOGGER *logger, const char *ref, const FILE *file, const char *format, const int level);
static char *logger_level_to_string(const int level);
void __logger_msg__(const char *fname, const int line, \
    const LOGGER logger, const int level, const char *msg, ...);
static Node *logger_formatter(const char *s);
void logger_remove(LOGGER *logger);
void logger_change_file(LOGGER *logger, const FILE *file);
void logger_change_format(LOGGER *logger, const char *format);
void logger_change_level(LOGGER *logger, const int level);
void logger_change_ffl(LOGGER *logger, const FILE *file, const char *format, const int level);
void logger_level_color_default(void);
void logger_level_color(const int level, const char *ansi_code);
void logger_level_color_reset(void);

void logger_level_color_reset(void) {
    memset(LOGGER_LEVEL_COLORS, 0, 6);
}

void logger_level_color(const int level, const char *ansi_code) {
    if (level < 0 || level >= OFF) {
        return;
    }
    LOGGER_LEVEL_COLORS[level] = ansi_code;
}

void logger_level_color_default(void) {
    LOGGER_LEVEL_COLORS[0] = LOGGER_DEFAULT_TRACE;
    LOGGER_LEVEL_COLORS[1] = LOGGER_DEFAULT_DEBUG;
    LOGGER_LEVEL_COLORS[2] = LOGGER_DEFAULT_INFO;
    LOGGER_LEVEL_COLORS[3] = LOGGER_DEFAULT_WARNING;
    LOGGER_LEVEL_COLORS[4] = LOGGER_DEFAULT_ERROR;
    LOGGER_LEVEL_COLORS[5] = LOGGER_DEFAULT_FATAL;
}

void logger_change_ffl(LOGGER *logger, const FILE *file, const char *format, const int level) {
    if (file) {
        logger_change_file(logger, file);
    }
    if (format) {
        logger_change_format(logger, format);
    }
    if (level != -1) {
        if (level < 0 || level > OFF) {
            logger_change_level(logger, level);
        }
    }
}

void logger_change_level(LOGGER *logger, const int level) {
    if (level == -1) {
        return;
    }
    if (level < TRACE && level > OFF) {
        fprintf((FILE*)logger -> file, "[ROOT] Invalid initiallizer 'level', should be one of those:\n"\
                "TRACE, DEBUG, INFO, ERROR, WARNING, FATAL, OFF");
        // casting just to avoid compilers warning(I hate this!)
        logger -> level = OFF;
        return;
    }
    logger -> level = level;
}

void logger_change_format(LOGGER *logger, const char *format) {
    if (!format) {
        return;
    }
    linked_list_free((Node*)logger -> format);
    Node *parsed_format = logger_formatter(format);
    logger -> format = parsed_format;
}

void logger_change_file(LOGGER *logger, const FILE *file) {
    if (!file) {
        return;
    }
    logger -> file = file;
}

void logger_remove(LOGGER *logger) {
    linked_list_free(logger -> format);
}

static int logger_getID(const char *s) {
    int i;
    for (i = 0; i < sizeof(LOGGER_INFO_LABELS)/sizeof(LOGGER_INFO_LABELS[0]); i++) {
        if (strcmp(s, LOGGER_INFO_LABELS[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void logger_init(LOGGER *logger, const char *ref, const FILE *file, const char *format, const int level) {
    logger -> ref = ref;
    logger -> file = file;
    Node *parsed_format = logger_formatter(format);
    logger -> format = parsed_format;
    if (level > OFF || level < TRACE) {
        fprintf((FILE*)logger -> file, "[ROOT] Invalid initiallizer 'level', should be one of those:\nTRACE, DEBUG, INFO, ERROR, WARNING, FATAL, OFF");
        logger -> level = OFF;
        return;
    }
    logger -> level = level;
}

static char *logger_level_to_string(int level) {
    int len = 0;
    if (LOGGER_LEVEL_COLORS[level]) {
        len = strlen(LOGGER_LEVEL_COLORS[level]);
        len += 4;
        // Length of escape sequences (4 is the \033[0m to back to normal)
    }
    char *p = malloc(len + 8);
    if (LOGGER_LEVEL_COLORS[level]) {
        strcpy(p, LOGGER_LEVEL_COLORS[level]);
    } else *p = '\0';
    switch (level) {
        case TRACE:
            strcat(p, "TRACE");
            break;
        case DEBUG:
            strcat(p, "DEBUG");
            break;
        case INFO:
            strcat(p, "INFO");
            break;
        case WARNING:
            strcat(p, "WARNING");
            break;
        case ERROR:
            strcat(p, "ERROR");
            break;
        case FATAL:
            strcat(p, "FATAL");
            break;
        case OFF:
            strcat(p, "OFF");
            break;
        default:
            strcat(p, "UNKNOWN");
            break;
    }
    if (LOGGER_LEVEL_COLORS[level]) {
        strcat(p, LOGGER_RESET);
    }
    return p;
}

void __logger_msg__(const char *fname, const int line, \
    LOGGER logger, const int level, const char *msg, ...) {
    if (level > OFF) {
        fprintf((FILE*)(FILE*)logger.file, "[ROOT] Invalid initiallizer 'level', should be one of those:\nTRACE, DEBUG, INFO, ERROR, WARNING, FATAL, OFF");
        return;
    }
    if (level < logger.level || level == OFF) {
        return;
    }
    va_list args;
    va_start(args, msg);time_t now = time(NULL);

    struct tm *tm_info = localtime(&now);
    char time_str[26];
    char date_str[26];
    // Format: HH:MM:SS
    strftime(time_str, 26, "%H:%M:%S", tm_info);
    // Format: YYYY-MM-DD
    strftime(date_str, 26, "%Y-%m-%d", tm_info);

    char *level_str = logger_level_to_string(level);
    const Node *curr = logger.format;
    while (curr) {
        if (curr -> type == LITERAL) {
            fprintf((FILE*)logger.file, "%s", curr -> string);
        } else {
            int ID = logger_getID(curr -> string);
            switch (ID) {
                case REF:
                    fprintf((FILE*)logger.file, "%s", logger.ref);
                    break;
                case LEVEL:
                    fprintf((FILE*)logger.file, "%s", level_str);
                    break;
                case DATE:
                    fprintf((FILE*)logger.file, "%s", date_str);
                    break;
                case TIME:
                    fprintf((FILE*)logger.file, "%s", time_str);
                    break;
                case FILENAME:
                    fprintf((FILE*)logger.file, "%s", fname);
                    break;
                case LINE:
                    fprintf((FILE*)logger.file, "%d", line);
                    break;
                case MSG:
                    vfprintf((FILE*)logger.file, msg, args);
                    break;
            }
        }
        curr = curr -> next;
    }
    va_end(args);
    free(level_str);
}


/* Take a string and turn it into a linked list having the format */
static Node *logger_formatter(const char *s) {
    if (!s) return NULL;
    
    Node *pHead = malloc(sizeof(Node));
    if (!pHead) return NULL;
    pHead->string = "HEAD";
    pHead->type = HEAD;
    pHead->next = NULL;

    while (*s) {
        if (*s == '/' && *(s + 1) == '[') {
            char *buffer = malloc(2);
            if (!buffer) {
                linked_list_free(pHead);
                return NULL;
            }
            buffer[0] = '[';
            buffer[1] = '\0';
            
            Node *curr = malloc(sizeof(Node));
            if (!curr) {
                free(buffer);
                linked_list_free(pHead);
                return NULL;
            }
            curr->string = buffer;
            curr->type = LITERAL;
            curr->next = NULL;
            linked_list_add(pHead, curr);
            
            s += 2;
        } else if (*s == '/' && *(s + 1) == ']') {
            char *buffer = malloc(2);
            if (!buffer) {
                linked_list_free(pHead);
                return NULL;
            }
            buffer[0] = ']';
            buffer[1] = '\0';
            
            Node *curr = malloc(sizeof(Node));
            if (!curr) {
                free(buffer);
                linked_list_free(pHead);
                return NULL;
            }
            curr->string = buffer;
            curr->type = LITERAL;
            curr->next = NULL;
            linked_list_add(pHead, curr);
            
            s += 2;
        } else if (*s == '[') {
            s++;
            const char *sp = s;
            int len = 0;
            
            while (*s && *s != ']') {
                len++;
                s++;
            }
            
            if (!*s) {
                linked_list_free(pHead);
                return NULL;
            }

            char *buffer = malloc(len + 1);
            if (!buffer) {
                linked_list_free(pHead);
                return NULL;
            }
            
            strncpy(buffer, sp, len);
            buffer[len] = '\0';
            
            Node *curr = malloc(sizeof(Node));
            if (!curr) {
                free(buffer);
                linked_list_free(pHead);
                return NULL;
            }
            curr->string = buffer;
            curr->type = LABEL;
            curr->next = NULL;
            linked_list_add(pHead, curr);
            
            s++;
        } else {
            const char *sp = s;
            int len = 0;
            
            while (*s && 
                   !(*s == '[') && 
                   !(*s == '/' && (*(s + 1) == '[' || *(s + 1) == ']'))) {
                len++;
                s++;
            }
            
            if (len > 0) {
                char *buffer = malloc(len + 1);
                if (!buffer) {
                    linked_list_free(pHead);
                    return NULL;
                }
                
                strncpy(buffer, sp, len);
                buffer[len] = '\0';
                
                Node *curr = malloc(sizeof(Node));
                if (!curr) {
                    free(buffer);
                    linked_list_free(pHead);
                    return NULL;
                }
                curr->string = buffer;
                curr->type = LITERAL;
                curr->next = NULL;
                linked_list_add(pHead, curr);
            }
        }
    }
    
    return pHead;
}
