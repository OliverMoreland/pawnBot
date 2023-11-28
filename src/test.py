#! /bin/python
import ctypes
import os
from time import sleep
import tkinter as tk
playerIsBlack = False

init_board = [                              
          [ 17, 3, 5, 9,15, 5, 3, 17], 
          [ 1, 1, 1, 1, 1, 1, 1, 1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [ 0, 0, 0, 0, 0, 0, 0, 0], 
          [ 16, 2, 4, 8,14, 4, 2, 16] 
     ]
'''
init_board = [                              
          [-1,-1,-1,-1,15,-1,-1,-1], 
          [1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [ 0, 0, 0, 0, 0, 0, 0, 0],
          [ 16, 2, 4, 8,14, 4, 2, 16] 
     ]'''
BOARD_SIZE = 8

Board = ctypes.c_int * (BOARD_SIZE * BOARD_SIZE)
class CoordLL(ctypes.Structure):
    pass
CoordLL._fields_ = [
    ("x", ctypes.c_int),
    ("y", ctypes.c_int),
    ("next", ctypes.POINTER(CoordLL))
    ]

class BoardLinkedList(ctypes.Structure):
    pass
BoardLinkedList._fields_ = [
    ("current", Board),
    ("next", ctypes.POINTER(BoardLinkedList))
    ]
     
path = os.getcwd()
chessBotLib = ctypes.CDLL(os.path.join(path, 'lib/libchessbot.so'))
chessBotLib.get_possible_moves_from_board.restype = ctypes.POINTER(BoardLinkedList)
chessBotLib.get_possible_moves_from_board.argtypes = [Board,ctypes.c_bool]
chessBotLib.get_coords_from_piece.restype = ctypes.POINTER(CoordLL)
chessBotLib.get_coords_from_piece.argtypes = [Board,ctypes.c_int,ctypes.c_int,ctypes.c_bool]
chessBotLib.print_board.argtypes = [Board]
chessBotLib.is_legal_move.restype = ctypes.c_bool
chessBotLib.is_legal_move.argtypes = [Board,Board,ctypes.c_bool]
chessBotLib.start_bot_daemon.argtypes = [Board]

board = Board()

for i, row in enumerate(init_board):
    for j, value in enumerate(row):
        board[i * BOARD_SIZE + j] = value
print("before")
print(chessBotLib.start_bot_daemon(board))
print("after")