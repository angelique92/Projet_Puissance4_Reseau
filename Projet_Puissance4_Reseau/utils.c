#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "utils.h"
#include "tlv.h"

#define POS_BUF_SIZE 256

char pos_buf[POS_BUF_SIZE];

const char *
strpos(char *file, const char *func, int line)
{
    snprintf(pos_buf, POS_BUF_SIZE,
             "pid %d, file %s, func %s, line %d",
             getpid(), file, func, line);
    return pos_buf;
}


void
panic(char *file, const char *func, int line, char *msg)
{
    fprintf(stderr, "%s: %s\n", strpos(file, func, line), msg);
    exit(1);
}

void
log_error(char *file, const char *func, int line, char *msg)
{
    fprintf(stderr, "%s: %s: %s\n",
            strpos(file, func, line), msg, strerror(errno));
}

void
handle_error(int rc, char *file, const char *func, int line, char *msg)
{
    if (rc < 0) {
        log_error(file, func, line, msg);
        exit(1);
    }
}
/******************************************************************
    handle_discon
 Fonction qui envoie un TLV DISCON si il ya un probleme en lecture 
******************************************************************/
void handle_discon( int rc, int fdc){
    if(rc <= 0 ){
        send_discon(fdc);
        exit(1);
    }
}

ssize_t
write_all(int fd, const void *buf, size_t count)
{

    const char *charbuf = buf;
    size_t buf_div = 0;
    int rc;
    while (buf_div < count) {
        rc = write(fd, charbuf + buf_div,
                       count - buf_div);
        if (rc < 0) {
            return -1;
        }
        if( rc == 0){
            break;
        }
        buf_div += rc;
    }
    return buf_div;
}

ssize_t
read_all(int fd, void *buf, size_t count)
{
    char *charbuf = buf;
    size_t buf_div = 0;
    int rc;
    while (buf_div < count) {
        
         rc = read(fd, charbuf + buf_div, count - buf_div);
        if (rc < 0) {
            return -1;
        }
        if (rc == 0) {
            break;
        }

        buf_div += rc;
    }
    return buf_div;
}
