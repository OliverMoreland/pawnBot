import ctypes
import os

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

class BoardLinkedList(ctypes.Structure):
    _fields_ = [("current", ctypes.c_int * 8 * 8),
                ("next", ctypes.POINTER(ctypes.c_int))]
class BoardLinkedList(ctypes.Structure):
    _fields_ = [("current", ctypes.c_int * 8 * 8),
                ("next", ctypes.POINTER(BoardLinkedList))]
     
path = os.getcwd()
movesL = ctypes.CDLL(os.path.join(path, 'moves.so'))
movesL.getPossibleMovesFromPiece.restype = ctypes.POINTER(BoardLinkedList)

moves = ctypes.POINTER(BoardLinkedList)
cboard = (ctypes.c_int * 8 * 8)(*init_board)

moves = movesL.getPossibleMovesFromBoard(cboard,1)
print(moves.contents.current[0][0])
