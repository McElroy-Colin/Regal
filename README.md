# Regal V0.6

**Interpreter source code in progress.**

Regal is a work-in-progress non-functional programming language developed by **Colin McElroy**. Currently, the language is unfinished, with progress on the interpreter source code located in the `~/src` and `~/include` directories. Regal is intended to support features such as:

- Variable declaration
- Function declaration
- Static type-checking
- Implicit typing
- Optimized evaluation
- And more...

Updates will be posted in the **main** branch as progress continues. The **dev** branch will contain unfinished updates as well.

The Regal playground is available in the **releases** tab and in the `release_build` folder. Running the `playground.py` script will open the application, and the `interpreter.exe` will automatically interpret any code written in the text window. Currently, the playground is only compatible with **Windows**.

**Note:** Python 3.6+ is required to run the playground.

---

## V0.6 Features:

- **Multi-line if-else statements** with indent-scope semantics.

    ```python
    if x == 5
        x = x + 5
    else if x > 7
        x = 9
    else
        x = 2
    ```

- **Playground application** with live error display and variable optimization.

---
