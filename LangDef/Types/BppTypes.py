"""
File containing classes and relevant code for distinguishing token types when compiling in B++.
"""


# Keywords

# Class for the keyword 'let'.
class Let:
    
    # Constructor
    def __init__(self):
        pass

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return 'Let'
    
    # Nothing to extract.
    def extract(self):
        pass

# Class for keyword 'now'.
class Now:
    
    # Constructor
    def __init__(self):
        pass

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return 'Now'
    
    # Nothing to extract.
    def extract(self):
        pass

# Global list of keywords.
KEYWRDS = [Let, Now]


# Primitive types

# Class for positive and negative integers.
class Int:

    # Constructor.
    def __init__(self, value):
        if (not isinstance(value, int)):
            raise TypeError(f"Int class was instantiated with {value}.")
        self.value = value

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'Int({self.value})'

    # Retrieve the contained data.
    def extract(self):
        return self.value

# Class for positive and negative integers.
class Bool:
    
    # Constructor.
    def __init__(self, value):
        if (not isinstance(value, bool)):
            raise TypeError(f"Bool class was instantiated with {value}.")
        self.value = value

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'Bool({self.value})'

    # Retrieve the contained data.
    def extract(self):
        return self.value
    


# Nothing type

# Class for the nothing token.
class Nothing:
    
    # Constructor.
    def __init__(self):
        pass

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return 'Nothing'
    
    # Nothing to extract.
    def extract(self):
        pass


# Variable type

# Class for the variable token.
class Var:
    
    # Constructor.
    def __init__(self, value):
        if (not isinstance(value, str)):
            raise TypeError(f"Var class was instantiated with {value}.")
        self.value = value

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'Var({self.value})'

    # Retrieve the contained data.
    def extract(self):
        return self.value
    


# Primtive Operator types
    
    # Note: These are not the classes used in lexing. Those classes can be found in bottom section.

# Class for primitive operator '+'.


class Add:

    # Constructor.
    def __init__(self, nums):
        if (isinstance(nums, tuple) and (len(nums) == 2)):
            self.nums = nums
        else:
            raise TypeError(f"Add class was instantiated with {nums}.")

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'Add{self.nums}'
    
    # Retrieve the contained data.
    def extract(self):
        return self.nums
    
    # Update the currently stored values and return the object.
    def update(self, values):
        if ((isinstance(values, tuple)) and (len(values) == 2)):
            self.nums = values
            return self
        else:
            raise TypeError(f"Add updated with {values}.")

# Class for primitive operator '-'.
class Sub:

    # Constructor.
    def __init__(self, nums):
        if (isinstance(nums, tuple) and (len(nums) == 2)):
            self.nums = nums
        else:
            raise TypeError(f'Sub class was instantiated with {nums}.')

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'Sub{self.nums}'
    
    # Retrieve the contained data.
    def extract(self):
        return self.nums
    
    # Update the currently stored values and return the object.
    def update(self, values):
        if ((isinstance(values, tuple)) and (len(values) == 2)):
            self.nums = values
            return self
        else:
            raise TypeError(f"Sub updated with {values}.")
    
# Class for primitive operator '*'.
class Mult:

    # Constructor.
    def __init__(self, nums):
        if (isinstance(nums, tuple) and (len(nums) == 2)):
            self.nums = nums
        else:
            raise TypeError(f'Mult class was instantiated with {nums}.')

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'Mult{self.nums}'
    
    # Retrieve the contained data.
    def extract(self):
        return self.nums
    
    # Update the currently stored values and return the object.
    def update(self, values):
        if ((isinstance(values, tuple)) and (len(values) == 2)):
            self.nums = values
            return self
        else:
            raise TypeError(f"Mult updated with {values}.")
    
# Class for primitive operator '/'.
class Div:

    # Constructor.
    def __init__(self, nums):
        if (isinstance(nums, tuple) and (len(nums) == 2)):
            self.nums = nums
        else:
            raise TypeError(f'Div class was instantiated with {nums}.')

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'Div{self.nums}'
    
    # Retrieve the contained data.
    def extract(self):
        return self.nums
    
    # Update the currently stored values and return the object.
    def update(self, values):
        if ((isinstance(values, tuple)) and (len(values) == 2)):
            self.nums = values
            return self
        else:
            raise TypeError(f"Div updated with {values}.")
    
# Class for primitive operator '**'.
class Exp:

    # Constructor.
    def __init__(self, nums):
        if (isinstance(nums, tuple) and (len(nums) == 2)):
            self.nums = nums
        else:
            raise TypeError(f'Exp class was instantiated with {nums}.')

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'Exp{self.nums}'
    
    # Retrieve the contained data.
    def extract(self):
        return self.nums
    
    # Update the currently stored values and return the object.
    def update(self, values):
        if ((isinstance(values, tuple)) and (len(values) == 2)):
            self.nums = values
            return self
        else:
            raise TypeError(f"Exp updated with {values}.")

# Standard Operators

# Class for standard operator '=='.
class IsEqual:
    
    # Constructor.
    def __init__(self, value):
        if (not isinstance(value, int)):
            raise TypeError(f"IsEqual class was instantiated with {value}.")
        self.value = value

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'IsEqual({self.value})'

    # Retrieve the contained data.
    def extract(self):
        return self.value
    
# Class for standard operator '!='.
class NotEqual:
    
    # Constructor.
    def __init__(self, value):
        if (not isinstance(value, int)):
            raise TypeError(f"NotEqual class was instantiated with {value}.")
        self.value = value

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'NotEqual({self.value})'

    # Retrieve the contained data.
    def extract(self):
        return self.value
    


# Binding Operators

# Class for binding operator '='.
class Bind:
    
    # Constructor.
    def __init__(self, value):
        if (not isinstance(value, int)):
            raise TypeError(f"Bind class was instantiated with {value}.")
        self.value = value

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'IsEqual({self.value})'

    # Retrieve the contained data.
    def extract(self):
        return self.value
    
# Class for binding operator '+='.
class Increm:
    
    # Constructor
    def __init__(self):
        pass
    
    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return 'Increm'
    
    # Nothing to extract.
    def extract(self):
        pass

# Groupers

# Class for left grouper '('.
class LeftParen:
    
    # Constructor.
    def __init__(self, value):
        if (not isinstance(value, int)):
            raise TypeError(f"LeftParen class was instantiated with {value}.")
        self.value = value

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'LeftParen({self.value})'

    # Retrieve the contained data.
    def extract(self):
        return self.value
    
# Class for right grouper ')'.
class RightParen:
    
    # Constructor.
    def __init__(self, value):
        if (not isinstance(value, int)):
            raise TypeError(f"RightParen class was instantiated with {value}.")
        self.value = value

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'RightParen({self.value})'

    # Retrieve the contained data.
    def extract(self):
        return self.value



# Classes not relevant to B++ typing

# Class for combining 'let' with a variable.
class LetVar:

    # Constructor.
    def __init__(self, binding):
        if ((isinstance(binding, tuple)) and (len(binding) == 2)):
            self.binding = binding
        else:
            raise TypeError(f"LetVar class was instantiated with {binding}.")
        

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'LetVar{self.binding}'

    # Retrieve the contained data.
    def extract(self):
        return self.binding

# Class for combining 'now' with a variable.
class NowVar:

    # Constructor.
    def __init__(self, binding):
        if ((isinstance(binding, tuple)) and (len(binding) == 2)):
            self.binding = binding
        else:
            raise TypeError(f"NowVar class was instantiated with {binding}.")
        

    # Display in an easy-to-read manner for debugging.
    def __repr__(self):
        return f'NowVar{self.binding}'

    # Retrieve the contained data.
    def extract(self):
        return self.binding

# Global lists of primitive operators.
PRIM_OPS = [Add, Sub, Mult, Div, Exp]
ADD_OPS = [Add, Sub]
MULT_OPS = [Mult, Div]

# Global list of number types.
NUMS = [Int]