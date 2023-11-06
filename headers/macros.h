#ifndef MACROS_H
#define MACROS_H

#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )
#define FORWARD(piece) ((piece & 1)*2-1)
#define GET_COLOR(piece) (piece & 1)
#define GET_TYPE(piece) (piece >> 1)
#define IS_EMPTY(piece) (piece == -1)
#define MAKE_PIECE(piece_type,color) (piece_type * 2 + color)

#endif

