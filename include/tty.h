#ifndef TTY_H
#define TTY_H

#include "mtask.h"
#define NUMROWS 25
#define NUMCOLS 80
typedef struct{
    /* cola para lo del teclado */
    unsigned short buf[NUMROWS][NUMCOLS];
    void * data;
    unsigned cur_x, cur_y, cur_attr;
    unsigned scrolls;
   // bool cursor_on;
    int cursor_on;
} Tty;

#endif
