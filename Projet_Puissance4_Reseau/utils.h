#ifndef _UTILS_H
#define _UTILS_H

#define __POS__ strpos(__FILE__, __func__, __LINE__)

#define PANIC(msg) panic(__FILE__, __func__, __LINE__, msg)

#define LOG_ERROR(msg) log_error(__FILE__, __func__, __LINE__, msg)

#define HANDLE_ERROR(rc, msg) handle_error(rc, __FILE__, __func__, __LINE__, msg)

#define HANDLE_DISCON(rc,fd) handle_discon(rc,fd)

void handle_discon(int rc, int fd);

const char *strpos(char *file, const char *func, int line);

void panic(char *file, const char *func, int line, char *msg);

void log_error(char *file, const char *func, int line, char *msg);

void handle_error(int rc, char *file, const char *func, int line, char *msg);

ssize_t write_all(int fd, const void *buf, size_t count);

ssize_t read_all(int fd, void *buf, size_t count);

#endif /* "utils.h" included. */
