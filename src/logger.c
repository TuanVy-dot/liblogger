#include "linked_list.h"
#include "logger.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

/* BAD CODE ALLERT :skull: */

#define LOGGER_DEFAULT_TRACE   "\033[0;37m"  // Light Gray
#define LOGGER_DEFAULT_DEBUG   "\033[0;34m"  // Blue
#define LOGGER_DEFAULT_INFO    "\033[0;32m"  // Green
#define LOGGER_DEFAULT_WARNING "\033[0;33m"  // Yellow
#define LOGGER_DEFAULT_ERROR   "\033[0;31m"  // Red
#define LOGGER_DEFAULT_FATAL   "\033[1;31m"  // Bold Red
#define LOGGER_RESET           "\033[0m"      // Reset Color

static const char *LOGGER_LEVEL_COLORS[6] = {0};

struct LOGGER_IMP {
    const char *ref;
    FILE *file; // destination file
    const Node *format; // printing format
    unsigned char level; // lowest level to be print
};
typedef struct LOGGER_IMP lgimp_t;


const char *LOGGER_INFO_LABELS[] = {
    "REF", "LEVEL", "DATE", "TIME", "FILENAME", "LINE", "MSG"
};

enum LOGGER_INF_LABELS_ID {
    REF, LEVEL, DATE, TIME, FILENAME, LINE, MSG
};

static int logger_getID(const char*);
static char *logger_level_to_string(const int level);
static Node *logger_formatter(const char *s);
static void logger_print_msg(const char *fname, const int line, 
                             const LOGGER *logger, const log_level_t level, 
                             const char *msg, va_list args);


/* Create a logger using the given parameters */
LOGGER *logger_create(const char *ref, FILE *file, 
                      const char *format, const log_level_t level) {
    lgimp_t *logger_imp = (lgimp_t*)malloc(sizeof(lgimp_t));
    if (!logger_imp) {
        return NULL;
    }

    logger_imp -> ref = ref;
    logger_imp -> file = file;
    Node *parsed_format = logger_formatter(format);
    logger_imp -> format = parsed_format;
    logger_imp -> level = level;
    return (LOGGER*)logger_imp;
}


/* Free allocated memory in the creation process and the logger itself */
void logger_remove(LOGGER *logger) {
    lgimp_t *logger_imp = (lgimp_t*)logger;
    linked_list_free(logger_imp -> format);
    free(logger_imp);
}


/* Print a log message */
void __logger_msg__(const char *fname, const int line, 
    const LOGGER *logger, const log_level_t level, const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    logger_print_msg(fname, line, logger, level, msg, args);
}


/* Log an array */
void __logger_log_array__(const char *fname, const int line, 
                          const LOGGER *logger, const log_level_t level, 
                          void *array, size_t element_size, size_t len,
                          void (*print_element)(FILE *, void *), const char *msg, ...) {
    FILE *fp = ((lgimp_t *)logger) -> file;
    va_list args;
    va_start(args, msg);
    logger_print_msg(fname, line, logger, level, msg, args);

    for (unsigned int i = 0; i < len; i++) {
        fprintf(fp, "i%d: ", i);
        print_element(fp, (char *)array);
        array = (char *)array + element_size;
    }
}


void logger_change_ref(LOGGER *logger, const char *ref) {
    if (!ref) {
        return;
    }
    lgimp_t *logger_imp = (lgimp_t*)logger;
    logger_imp -> ref = ref;
}


void logger_change_file(LOGGER *logger, FILE *file) {
    if (!file) {
        return;
    }
    lgimp_t *logger_imp = (lgimp_t*)logger;
    logger_imp -> file = file;
}


void logger_change_format(LOGGER *logger, const char *format) {
    if (!format) {
        return;
    }
    lgimp_t *logger_imp = (lgimp_t*)logger;
    linked_list_free((Node*)logger_imp -> format);
    Node *parsed_format = logger_formatter(format);
    logger_imp -> format = parsed_format;
}


void logger_change_level(LOGGER *logger, const int level) {
    lgimp_t *logger_imp = (lgimp_t*)logger;
    if (level == -1) {
        return;
    }
    if (level < TRACE || level > OFF) {
        logger_imp -> level = OFF;
        return;
    }
    logger_imp -> level = level;
}


void logger_change_rffl(LOGGER *logger, const char *ref, 
                        FILE *file, const char *format, 
                        const int level) {
    if (ref) {
        logger_change_ref(logger, ref);
    }
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


void logger_level_color(const log_level_t level, const char *ansi_code) {
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


void logger_level_color_reset(void) {
    memset(LOGGER_LEVEL_COLORS, 0, 6);
}


static int logger_getID(const char *s) {
    size_t i;
    for (i = 0; i < sizeof(LOGGER_INFO_LABELS)/sizeof(LOGGER_INFO_LABELS[0]); i++) {
        if (strcmp(s, LOGGER_INFO_LABELS[i]) == 0) {
            return i;
        }
    }
    return -1;
}


static char *logger_level_to_string(const int level) {
    int len = 0;
    if (LOGGER_LEVEL_COLORS[level]) {
        len = strlen(LOGGER_LEVEL_COLORS[level]);
        len += 4;
        // Length of escape sequences (4 is the \033[0m to back to normal)
    }
    char *p = malloc(len + 8);
if (!p) {
        return NULL;
    }
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


/* Take a format string and parse it into an easier to use linked list */
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


/* print a log message */
static void logger_print_msg(const char *fname, const int line, 
                             const LOGGER *logger, const log_level_t level, 
                             const char *msg, va_list args) {
    lgimp_t *logger_imp = (lgimp_t*)logger;
    if (level < logger_imp -> level || level == OFF) {
        return;
    }

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[26];
    char date_str[26];
    // Format: HH:MM:SS
    strftime(time_str, 26, "%H:%M:%S", tm_info);
    // Format: YYYY-MM-DD
    strftime(date_str, 26, "%Y-%m-%d", tm_info);

    char *level_str = logger_level_to_string(level);
    if (!level_str) {
        level_str = "GET LEVEL FAILED";
        // if failed to get level label
    }
    const Node *curr = logger_imp -> format;
    while (curr) {
        if (curr -> type == LITERAL) {
            fprintf(logger_imp -> file, "%s", curr -> string);
        } else {
            int ID = logger_getID(curr -> string);
            switch (ID) {
                case REF:
                    fprintf(logger_imp -> file, "%s", logger_imp -> ref);
                    break;
                case LEVEL:
                    fprintf(logger_imp -> file, "%s", level_str);
                    break;
                case DATE:
                    fprintf(logger_imp -> file, "%s", date_str);
                    break;
                case TIME:
                    fprintf(logger_imp -> file, "%s", time_str);
                    break;
                case FILENAME:
                    fprintf(logger_imp -> file, "%s", fname);
                    break;
                case LINE:
                    fprintf(logger_imp -> file, "%d", line);
                    break;
                case MSG:
                    vfprintf(logger_imp -> file, msg, args);
                    break;
            }
        }
        curr = curr -> next;
    }
    va_end(args);
    free(level_str);
}

