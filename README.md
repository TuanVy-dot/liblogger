# Liblogger

`liblogger` is a lightweight and ease of use C logging library (hence not the most efficient). It support coloring using ANSI escape sequences, log levels and multiple loggers. With a little set up, it can be ready to use.

current version: 2.0.0

Key features:
1. Lightweight Logging: Designed to enhance the standard printf functionality with more structured logging capabilities.


2. Dynamic Formatting: Supports dynamic formatting using keywords (e.g., REF, LEVEL, FILENAME, LINE, DATE, TIME, MSG) to customize log messages with dynamic infomations.


3. Custom Log Levels: Offers multiple log levels (TRACE, DEBUG, INFO, WARNING, ERROR, FATAL) to filter log messages based on importance.


4. ANSI Color Support: Allows ANSI escape codes for colored output in log messages, improving readability. Users can define custom colors for different log levels.


5. Automatic Context Information: Automatically includes the file name and line number in log messages, enhancing debugging.


6. Memory Management: Provides functions to create and remove loggers, with explicit memory management to avoid leaks.


7. Flexible Logger Configuration: Users can change the logger's format, file, reference, and log level dynamically through dedicated functions.


8. Convenient Macros: Offers convenient logging macros (logger_trace, logger_debug, logger_info, etc.) for quick logging without needing to specify parameters repeatedly.


9. Global Color Settings: Color settings for log levels can be adjusted globally, affecting all loggers consistently.


10. No Multi-threading Safety: Simplicity comes at the cost of thread safety; the library is not designed for concurrent access.


11. Default Log Format: Includes a default log format, which can be overridden by user-defined formats to meet specific requirements.


12. Error Handling: Provides return values to indicate failure or success in creating or manipulating loggers, allowing for better error management.

# Installation
Get the source and install by yourself, it is the most portable way I can think about.

***Note that I did not check for any compatibility, there could be a problem with it.***

# Usage
The usage is fairly simple, all you have to do is declare a `LOGGER pointer`, initialize using `logger_create()` and some optional additional configurations and it is done. See documentation in the github wiki.
