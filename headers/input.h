#ifndef INPUT_H
#define INPUT_H

#include "structs.h"
int human_move(board current);
piece read_piece();
int human_move_gui(board current, int ox, int oy, int x, int y);
#endif