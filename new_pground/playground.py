# Standard library imports.
import tkinter as tk
from tkinter import scrolledtext, font
import subprocess

def _init_window(screen_ratio):
    window = tk.Tk()
    window.title("Regal Playground")

    screen_width = window.winfo_screenwidth()
    screen_height = window.winfo_screenheight()

    window_width = int(screen_width * screen_ratio)
    window_height = int(screen_height * screen_ratio)

    window_xpos = (screen_width - window_width) // 2
    window_ypos = (screen_height - window_height) // 2

    window.geometry(f"{window_width}x{window_height}+{window_xpos}+{window_ypos}")

    window.resizable(True, True)

    return window, window_width, window_height



def _init_linenumbers_old(linenum_frame, bg, text_color, text_font, linenum_str, num_lines):
    line_height = text_font.metrics("linespace")
    for i in range(1, num_lines + 1):
        #str(i) + linenum_str
        curr_linenum = tk.Label(linenum_frame, text = "100", width = len(linenum_str) + 1, bg = bg, fg = text_color, anchor = "e", font = text_font)
        curr_linenum.place(x = 10, y = (i - 1) * line_height)

def _draw_next_linenumber(linenum_frame, bg, text_color, linenum_str): 
    pass

def _init_linenumber_frame(window, window_dim, bg, fg, font, linenum_str): 
    num_width = -1
    for i in range(10):
        curr_width = font.measure(str(i))

        if num_width < curr_width or num_width == -1:
            num_width = curr_width

    linenum_width = num_width * 4 + font.measure(linenum_str)

    linenum_frame = tk.Frame(window, width = linenum_width, height = int(window_dim[1] * 0.7), bg = bg)
    linenum_frame.pack(side = "top", anchor = "nw")

    _init_linenumbers_old(linenum_frame, bg = bg, text_color = fg, text_font = font, linenum_str = linenum_str, num_lines = 10)

def _init_text_frame(window, window_dim, bg, fg, font, linenum_str):
    _init_linenumber_frame(window = window, window_dim = window_dim, bg = bg, fg = fg, font = font, linenum_str = linenum_str)



def init_playground(screen_ratio = 0.8, text_bg = "#5A5858", text_color = "white", text_font = "Courier New", text_size = 12, linenum_str =  ">"):
    window, width, height = _init_window(screen_ratio)

    font_obj = font.Font(family = text_font, size = text_size)
    _init_text_frame(window, window_dim = (width, height), bg = text_bg, fg = text_color, font = font_obj, linenum_str = linenum_str)

    window.mainloop()


init_playground()