# Standard library imports.
import tkinter as tk
import subprocess

window = tk.Tk()
window.title("Regal Playground")

screen_width = window.winfo_screenwidth()
screen_height = window.winfo_screenheight()

wwindow_width = int(screen_width * 0.8)
window_height = int(screen_height * 0.8)

x = (screen_width - wwindow_width) // 2
y = (screen_height - window_height) // 2

window.geometry(f"{wwindow_width}x{window_height}+{x}+{y}")

text_area = tk.Text(window, wrap = "word", font = ("Arial", 12), bg = "#5A5858", fg = "#ffffff")
text_area.pack(expand = 1, fill = "both")

window.mainloop()