# Standard library imports.
import tkinter as tk
from tkinter import scrolledtext
import subprocess
from tkinter import font

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



def _init_linenumbers(linenum_frame, bg, text_color, num_lines):
    linenum_str = "> "
    for i in range(1, num_lines + 1):
        curr_linenum = tk.Label(linenum_frame, text = str(i) + linenum_str, width = len(linenum_str) + 1, bg = bg, fg = text_color, anchor = "w")
        curr_linenum.pack()

def _init_text_frame(window, window_dim, bg, fg, font):
    num_width = -1
    for i in range(10):
        curr_width = font.measure(str(i))

        if num_width < curr_width or num_width == -1:
            num_width = curr_width

    linenum_width = num_width * 3 + font.measure(">  ")

    linenum_frame = tk.Frame(window, width = linenum_width, height = int(window_dim[1] * 0.7))
    linenum_frame.pack(side = "top", anchor = "nw")

    _init_linenumbers(linenum_frame, bg = bg, text_color = fg, num_lines = 10)



def init_playground(screen_ratio = 0.8, text_bg = "#5A5858", text_color = "white", text_font = "Arial", text_size = 12):
    window, width, height = _init_window(screen_ratio)

    font_obj = font.Font(family = text_font, size = text_size)
    _init_text_frame(window, window_dim = (width, height), bg = text_bg, fg = text_color, font = font_obj)

    window.mainloop()


init_playground()