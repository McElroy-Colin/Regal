# Standard library imports.
import tkinter as tk
from tkinter import scrolledtext
import subprocess
from tkinter import font

def update_line_numbers(event=None):
    content = text_input.get("1.0", "end-1c")
    lines = content.splitlines()
    
    new_content = "\n".join([f"{i+1}> {line}" for i, line in enumerate(lines)])
    
    text_input.delete("1.0", "end")
    text_input.insert("1.0", new_content)



window_bg = "#5A5858"


window = tk.Tk()
window.title("Regal Playground")

screen_width = window.winfo_screenwidth()
screen_height = window.winfo_screenheight()

window_width = int(screen_width * 0.8)
window_height = int(screen_height * 0.8)

window_xpos = (screen_width - window_width) // 2
window_ypos = (screen_height - window_height) // 2

window.geometry(f"{window_width}x{window_height}+{window_xpos}+{window_ypos}")

window.resizable(True, True)

text_frame = tk.Frame(window)
text_frame.place(relwidth = 0.7, relheight = 0.8)

text_input = tk.Text(text_frame, font = ("Helvetica", 12), wrap = tk.WORD, bg = window_bg, fg = "white")
text_input.pack(fill = tk.BOTH, expand = True)

text_input.bind("<KeyRelease>", update_line_numbers())

text_input.focus_set()

window.mainloop()