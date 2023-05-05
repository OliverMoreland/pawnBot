#ifndef MACROS_H
#define MACROS_H

#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )
#define FORWARD(piece) ((piece & 1)*2-1)
#define IS_BLACK(piece) (piece & 1)
#define GET_TYPE(piece) (piece >> 1)
#define IS_EMPTY(piece) (piece == -1)

#endif

