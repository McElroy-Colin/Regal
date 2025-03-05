import tkinter as tk
import subprocess
import sys
import os


class Playground:
    """
    The Regal playground. This represents a tkinter application that generates a simple trext environment with live stack display and code error display.
    """

    def __init__(self, display_package):
        """
        Initialize a playground application instance.
            display_package: a disiplayPackage object to theme the playground visually (input)
            debug: boolean, True if the playground should run a debug version of the interpreter on given code (input)
        """

#       Initialize the main window and retrieve window data.
        self.main_frame = tk.Tk()
        self.screen_width = self.main_frame.winfo_screenwidth()
        self.screen_height = self.main_frame.winfo_screenheight()

#       Configure the main window.
        self.main_frame.geometry(f"{int(self.screen_width * 0.7)}x{int(self.screen_height * 0.7)}")
        self.main_frame.title("Regal Playground")
        self.main_frame.resizable(True, True)
        self.main_frame.minsize(int(self.screen_width * 0.2), int(self.screen_height * 0.2))

#       Set the colors and fonts from the given display package.
        self.color_scheme = display_package.colors
        self.font_scheme = display_package.fonts

#       Initialize the frame to display the text window and stack display.
        self._init_code_frame()
#       Initialize the frame to display line numbers.
        self._init_line_numbers()
#       Initialize the frame to display the text editing region.
        self._init_text_region()
#       Initialize the frame to display the stack.
        self._init_stack_display()
#       Initialize the frame to display error messages and interpretation time.
        self._init_error_display()

        return

    def _init_code_frame(self):
        """
        Initialize the frame to display the text window and stack display.
        """

        self.code_frame = tk.Frame(self.main_frame)
        self.code_frame.pack(expand=True, fill="both")

#       Establish the relative size of each portion of this frame.
        self.code_frame.columnconfigure(1, weight=3, minsize=500)
        self.code_frame.columnconfigure(2, weight=2, minsize=200)
        self.code_frame.rowconfigure(0, weight=1)

        return
    
    def _init_line_numbers(self):
        """
        Initialize the frame to display line numbers.
        """

#       Initialize the variable storing the current number of lines.
        self.line_nums = 0
        self.line_num_region = tk.Text(self.code_frame, width=3, bg=self.color_scheme[0], fg=self.color_scheme[3], font=self.font_scheme[0], bd=0)
#       Insert the  firt line number and disable editing in this frame.
        self.line_num_region.insert(tk.END, "1>")
        self.line_num_region.config(state="disabled")

        self.line_num_region.grid(row=0, column=0, sticky="nsew")

        return
        
    def _init_text_region(self):
        """
        Initialize the frame to display the text editing region.
        """

        self.text_region = tk.Text(self.code_frame, tabs=1, insertbackground=self.color_scheme[2], bg=self.color_scheme[0], fg=self.color_scheme[2], font=self.font_scheme[0], bd=0)
        self.text_region.grid(row=0, column=1, sticky="nsew")

#       after_id ensures that the stack is not printed twice on top of itself.
        self.after_id = None

#       Initialize binding so that a tab key is always the same number of spaces.
        self.text_region.bind("<Tab>", self.insert_tab)

#       Ensure line numbers are updates every key press/release if a line appears/disappears.
        self.text_region.bind("<KeyPress>", self.update_line_nums)
#       Ensure text is interpreted if no key is pressed for 500 ms.
        self.text_region.bind("<KeyRelease>", lambda event: (
            self.update_line_nums(event), 
            self.delay_event(500, self.text_region, self.interpret_text, event)
        ))

        return

    def _init_stack_display(self):
        """
        Initialize the frame to display the stack.
        """

        self.stack_disp_frame = tk.Text(self.code_frame, state="disabled", width=50, bg=self.color_scheme[1], fg=self.color_scheme[2], font=self.font_scheme[0], bd=0)
        self.stack_disp_frame.grid(row=0, column=2, sticky="nsew")

        return
    
    def _init_error_display(self):
        """
        Initialize the frame to display error messages and interpretation time.
        """

#       Errors display in a different color than regular text (from the 'fg' parameter).
        self.error_display = tk.Text(self.main_frame, state="disabled", height=5, bg=self.color_scheme[1], fg=self.color_scheme[4], font=self.font_scheme[1], bd=0)
        self.error_display.pack(fill="x", side="bottom")
        return

    def delay_event(self, time, frame, fun, event=None):
        """
        Delay the given function on the implied event by the given time. 
        If an event is interrupted while being delayed, cancel that delay and restart delaying.
            time: float of time in ms to delay the event (input)
            frame: frame that the event is taking place in (input)
            fun: function to run after delaying (input)
            event: implied event being delayed (default None) (input)
        """

        if self.after_id:
            frame.after_cancel(self.after_id)
        self.after_id = frame.after(time, fun)
        return
    
    def interpret_text(self):
        """
        Interpret the current text in the text region of the playground. 
        Display the error message if an error was caught during interpretation.
        Otherwise, update the stack display and interpretation time.
        """

#       Retrieve tect input from the text region.
        text_input = self.text_region.get("1.0", tk.END)

#       Retrieve interpeter path.
        args = [os.path.dirname(os.path.abspath(__file__)) + "/interpreter.exe"]
#       Capture all output from the interpeter.
        output = subprocess.run(args, input=text_input, capture_output=True, text=True)

#       If an error occurred, display the error message.
        if output.stderr:
            self.write_error(output.stderr)
#       Otherwise, display the stack and interpretation time.
        else:
            output_text = output.stdout
#           Separate the stack output from the interpretation output using the delimeter "$$$".
            index = output_text.find("$$$")

#           If both outputs were found, display them.
            if index != -1:
                stack_str = output_text[:index]
                time_str = output_text[index + 3:]
                
                self.write_stack(stack_str)
                self.write_execution_time(time_str)

        return

    def write_stack(self, display_str):
        """
        Display the given string in the stack display frame.
            display_str: string to display (input)
        """

#       Allow insertion to the frame, overwrite the current text, then disable insertion.
        self.stack_disp_frame.config(state="normal")
        self.stack_disp_frame.delete("1.0", tk.END)
        self.stack_disp_frame.insert(tk.END, display_str)
        self.stack_disp_frame.config(state="disabled")

        return

    def write_error(self, error_str):
        """
        Display the given string in the error display frame.
            error_str: string to display (input)
        """

#       Allow insertion to the frame, overwrite the current text, then disable insertion.
#       Change the text to the error color before displaying.
        self.error_display.config(state="normal", fg=self.color_scheme[4])
        self.error_display.delete("1.0", tk.END)
        self.error_display.insert(tk.END, error_str)
        self.error_display.config(state="disabled")

    def write_execution_time(self, time_str):
        """
        Display the given string in the error display frame.
            time_str: string to display (input)
        """

#       Allow insertion to the frame, overwrite the current text, then disable insertion.
#       Change the text to the default color before displaying.
        self.error_display.config(state="normal", fg=self.color_scheme[2])
        self.error_display.delete("1.0", tk.END)
        self.error_display.insert(tk.END, time_str)
        self.error_display.config(state="disabled")

    def insert_tab(self, event=None):
        """
        Convert a tab key press to a set number of spaces for indent consistency.
            event: implied event to bind the tab key to this function (default None) (input)
        """

        self.text_region.insert(tk.INSERT, "   ")
        return

    def update_line_nums(self, event=None):
        """
        Update the line number display.
            event: implied event to bind this function (default None) (input)
        """

#       Parse the current number of lines, including trailing lines with just '\n'.
        curr_lines = int(self.text_region.index("end-1c").split('.')[0])
#       Do nothing if the current line count matches the parsed line count, i.e. no lines were added or removed.
        if self.line_nums == curr_lines:
            return 
#       Otherwise, update the current line count.
        self.line_nums = curr_lines

#       Allow insertion, overwrite the line number display, and display the new amount of line numbers.
        self.line_num_region.config(state="normal")
        self.line_num_region.delete("1.0", tk.END)
        for i in range(1, self.line_nums + 1):
            self.line_num_region.insert(tk.END, f"{i}>\n")
#       Disable insertion.
        self.line_num_region.config(state="disabled")

        return


class displayPackage:
    """
    Package that stores colors and fonts for playground display customization.
    """

    def __init__(self, colors, fonts):
        """
        Initialize a display package.
            colors: list of at least 3 colors to theme the playground (input)
            fonts: list of at least 2 fonts to theme the playground (input)
        """

#       Initialize colors and fonts, some colors are derived from others.
        self.colors = [colors[0], self.darken_hex_color(colors[0], 0.1), colors[1], self.darken_hex_color(colors[1], 0.4), colors[2]]
        self.fonts = fonts

    def lighten_hex_color(self, hex_color, factor):
        """
        Lighten a given hex color by the given factor.
            hex_color: string representing a hex_color (e.g. "#ffffff") (input)
            factor: float between 0 and 1, higher factor means a more dramatic lightening (input)
        """

        # Ensure hex_color starts with "#" and is 7 characters long.
        if hex_color[0] == '#':
            hex_color = hex_color[1:]
        
        # Convert the hex color to RGB.
        r, g, b = [int(hex_color[i:i+2], 16) for i in (0, 2, 4)]
        
        # Lighten each component by the factor, blend it with 255 (white).
        r = int(r + (255 - r) * factor)
        g = int(g + (255 - g) * factor)
        b = int(b + (255 - b) * factor)
        
        # Ensure the values are clamped between 0 and 255.
        r = max(0, min(255, r))
        g = max(0, min(255, g))
        b = max(0, min(255, b))
        
        # Convert the new RGB values back to a hex color.
        return f"#{r:02x}{g:02x}{b:02x}"

    def darken_hex_color(self, hex_color, factor):
        """
        Darken a given hex color by the given factor.
            hex_color: string representing a hex_color (e.g. "#ffffff") (input)
            factor: float between 0 and 1, higher factor means a more dramatic darkening (input)
        """

        # Remove the "#" if present.
        hex_color = hex_color.lstrip('#')

        # Convert the hex color to RGB values.
        r, g, b = [int(hex_color[i:i+2], 16) for i in (0, 2, 4)]

        # Apply the darkening factor to each RGB value.
        r = max(0, int(r * (1 - factor)))
        g = max(0, int(g * (1 - factor)))
        b = max(0, int(b * (1 - factor)))

        # Return the darkened color in hex format.
        return f"#{r:02x}{g:02x}{b:02x}"



if __name__ == "__main__":
#   Create a display package with the given colors and fonts.
    colors = ["#4c4c4c", "#ffffff", "#D22B2B"]
    fonts = [("Courier New", 16), ("Courier New", 16, "bold")]
    disp_pkg = displayPackage(colors, fonts)

#   Create and run the playground.
    editor = Playground(disp_pkg)
    editor.main_frame.mainloop()
