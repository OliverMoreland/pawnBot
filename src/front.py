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
          [ 17, 3, 5, 9,15, 5, 3, 17], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
          [-1,-1,-1,-1,-1,-1,-1,-1], 
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
chessBotLib.get_possible_moves_from_piece.restype = ctypes.POINTER(CoordLL)
chessBotLib.get_possible_moves_from_piece.argtypes = [Board,ctypes.c_int,ctypes.c_int,ctypes.c_bool]
chessBotLib.print_board.argtypes = [Board]
chessBotLib.is_legal_move.restype = ctypes.c_bool
chessBotLib.is_legal_move.argtypes = [Board,Board,ctypes.c_bool]


board = Board()

for i, row in enumerate(init_board):
    for j, value in enumerate(row):
        board[i * BOARD_SIZE + j] = value


# coding: utf-8
import tkinter as tk
from PIL import ImageTk, Image, ImageDraw
root = tk.Tk()
boardSize = 8
padding = 50

squareSize = int((root.winfo_screenheight()-padding*3)/8)

moveRadius = 17
root.geometry(str(squareSize*boardSize+padding*2)+"x"+str(squareSize*boardSize+padding*2))
root.title("Chess Board")



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
pieces = {
    0: {
        "name": "White Pawn",
        "image_path": "wp.png"
    },
    1: {
        "name": "Black Pawn",
        "image_path": "bp.png"
    },
    2: {
        "name": "White Knight",
        "image_path": "wn.png"
    },
    3: {
        "image_path": "bn.png"
    },
    4: {
        "image_path": "wb.png"
    },
    5: {
        "image_path": "bb.png"
    },
    6: {
        "image_path": "wr.png"
    },
    7: {
        "image_path": "br.png"
    },
    8: {
        "image_path": "wq.png"
    },
    9: {
        "image_path": "bq.png"
    },
    10: {
        "image_path": "wk.png"
    },
    11: {
        "image_path": "bk.png"
    },
    12: {
        "image_path": "wp.png"
    },
    13: {
        "image_path": "bp.png"
    },
    14:{
        "image_path": "wk.png"
    },
    15: {
        "image_path": "bk.png"
    },
    16: {
        "image_path": "wr.png"
    },
    17: {
        "name": "Unmoved Black Rook",
        "image_path": "br.png"
    },
    
}
for i in pieces:
    #print(pieces[i])
    image = Image.open('assets/'+pieces[i]["image_path"])
    image = image.resize((squareSize, squareSize))
    image = image.convert("RGBA")
    square_image = Image.new("RGBA", (squareSize, squareSize))
    square_image.paste(image, (0, 0), mask=image)
    pieces[i]["image"] = ImageTk.PhotoImage(square_image)

pieceIds = {}
possiblesMoves = []
move_circle_ids = []
def get_possible_moves(x,y,playerIsBlack):
    global possiblesMoves
    possiblesMoves = []
    #print('start')
    moves = chessBotLib.get_possible_moves_from_piece(board,x,y,playerIsBlack)
    #print('end')

    movesHead = moves
    while(movesHead.contents.next):
        #print(movesHead.contents.x,movesHead.contents.y,movesHead.contents.next)
        possiblesMoves.append([movesHead.contents.x,movesHead.contents.y])
        movesHead = movesHead.contents.next
    chessBotLib.freeCLL(moves)

image = Image.new("RGBA", (moveRadius * 2+2, moveRadius * 2+1), (0, 0, 0, 0))
draw = ImageDraw.Draw(image)

# Draw a circle on the image
circle_color = (0, 0, 0, 128)  # RGBA color (0, 0, 0) with 50% transparency
draw.ellipse([(0, 0), (moveRadius * 2, moveRadius * 2)], fill=circle_color)

# Convert the image to Tkinter-compatible format
moveCircle = ImageTk.PhotoImage(image)

def computerMove():
    chessBotLib.computer_move(board, True)
    drawBoard(board)

def move_start(event):
    widget = event.widget
    item = widget.find_closest(event.x, event.y)[0]
    x_coords, y_coords = widget.coords(item)
    # Store the initial mouse position and selected item
    col = round((x_coords - squareSize/2) / squareSize)
    row = round((y_coords - squareSize/2) / squareSize)
    piecetype = pieceIds[item]
    get_possible_moves(col,row,piecetype % 2 == 1)
    for move in possiblesMoves:
        
        # Create an image item on the canvas
        image_item = widget.create_image((move[0]+0.5)*squareSize, (move[1]+0.5)*squareSize, image=moveCircle)
        widget.lift(image_item)
        move_circle_ids.append(image_item)

    widget.data = {'x': event.x, 'y': event.y, 'item': item,'oCol': col,'oRow':row}
def drag_motion(event):
    widget = event.widget
    item = widget.data['item']

    # Calculate the distance moved
    dx = event.x - widget.data['x']
    dy = event.y - widget.data['y']

    # Move the selected image on the canvas
    widget.move(item, dx, dy)

    # Update the stored mouse position
    widget.data['x'] = event.x
    widget.data['y'] = event.y
def move_end(event):
    widget = event.widget
    item = widget.data['item']

    global move_circle_ids
    for id in move_circle_ids:
        widget.delete(id)
    move_circle_ids = []
    col = round((widget.data['x'] - squareSize/2) / squareSize)
    row = round((widget.data['y'] - squareSize/2) / squareSize)
    oCol = widget.data['oCol']
    oRow = widget.data['oRow']
    #if(move_piece(col,row))
    if([col,row] in possiblesMoves):
        ret = chessBotLib.human_move_gui(board,oCol, oRow, col, row)
        if(ret == 1):
            print('end')
        #chessBotLib.print_board(board)
        widget.coords(item,col * squareSize + squareSize/2,row * squareSize + squareSize/2)
        drawBoard(board)
        canvas.after(30,computerMove)
    else:
        widget.coords(item,oCol * squareSize + squareSize/2,oRow * squareSize + squareSize/2)



canvas = tk.Canvas(root, width=squareSize*boardSize, height=squareSize*boardSize)
piece_ids = []
def drawBoard(toDraw):
    global piece_ids
    for id in piece_ids:
        canvas.delete(id)
    piece_ids = []
    for i in range(boardSize):
        for j in range(boardSize):
            pieceNum = toDraw[j*BOARD_SIZE+i]
            if(pieceNum != -1):
                piecetype = pieces[pieceNum]
                piece = canvas.create_image((i+0.5) * squareSize, (j+0.5) * squareSize, image=piecetype["image"])
                pieceIds[piece] = pieceNum
                canvas.tag_bind(piece, "<Button-1>", move_start)
                canvas.tag_bind(piece,"<B1-Motion>", drag_motion)
                canvas.tag_bind(piece,"<ButtonRelease-1>", move_end)
                piece_ids.append(piece)
for i in range(boardSize):
    for j in range(boardSize):
        color = "#EBECD0" if (i + j) % 2 == 0 else "#779556"
        square = canvas.create_rectangle(i*squareSize, j*squareSize, (i+1)*squareSize, (j+1)*squareSize, fill=color,outline='')
        canvas.lower(square)
drawBoard(board)

canvas.pack(padx=padding,pady=padding)

root.mainloop()




'''
# Function to handle piece dragging
def drag_start(event):
    widget = event.widget
    widget.startX = event.x
    widget.startY = event.y

def drag_motion(event):
    widget = event.widget
    x = widget.winfo_x() - widget.startX + event.x
    y = widget.winfo_y() - widget.startY + event.y
    widget.place(x=x, y=y)

def drag_end(event):
    widget = event.widget
    col = (widget.winfo_x() - chessboard.winfo_x()) // widget.winfo_width()
    row = (widget.winfo_y() - chessboard.winfo_y()) // widget.winfo_height()
    move_piece(widget, row, col)

def move_piece(piece, row, col):
    # Get the previous position of the piece
    prev_row, prev_col = piece_positions.get(piece, (None, None))
    
    # Check if the move is valid
    if is_valid_move(piece, prev_row, prev_col, row, col):
        # Update the piece position
        piece_positions[piece] = (row, col)
        # Move the piece to the new position on the GUI
        piece.place(x=col * piece.winfo_width(), y=row * piece.winfo_height())
    else:
        # Move the piece back to its previous position
        piece.place(x=prev_col * piece.winfo_width(), y=prev_row * piece.winfo_height())

# Create the chess pieces and attach drag-and-drop functionality
for piece_name, piece_symbol in pieces.items():
    piece = tk.Label(window, text=piece_symbol, font=("Arial", 24), bg="white")
    piece.bind("<Button-1>", drag_start)
    piece.bind("<B1-Motion>", drag_motion)
    piece.bind("<ButtonRelease-1>", drag_end)
    piece.place(x=0, y=0)  # Initial position outside the chessboard
    piece_positions[piece] = (None, None)
# Start the main event loop
window.mainloop()
'''