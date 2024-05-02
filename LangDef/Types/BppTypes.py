"""
File containing classes and relevant code for distinguishing token types when compiling in B++.
"""

# Variable storage.

class VarNode:

    def __init__(self, pair, next = None):
        if ((isinstance(pair, tuple)) and (len(pair) == 2)):
            if ((next == None) or (isinstance(next, VarNode))):
                self.pair = pair
                self.next = next
            else:
                raise TypeError(f"Variable Node instantiated with 'next' value {next}.")
        else:
            raise TypeError(f"Variable Node instantiated with {pair}")
        
    def __repr__(self):
        if (self.next == None):
            return f"N{self.pair}"
        return f"N{self.pair} -> "

    def attach(self, node):
        if (isinstance(node, VarNode)):
            self.next = node
            return self
        raise TypeError(f"Tried to attach {self} to {node}.")
    
    def extract(self):
        return self.pair
    
    def get_next(self):
        return self.next
    
    def repoint(self, node):
        if ((isinstance(node, VarNode)) or (node == None)):
            curr_node = self.get_next()
            while (curr_node != node):
                if (curr_node == None):
                    break
                temp = curr_node.get_next()
                curr_node.cleanup()
                curr_node = temp
            self.next = node
        else:
            raise TypeError(f"Tried to repoint {self} to {node}.")
        
    def jump(self):
        if (self.get_next() != None):
            self.repoint(self.get_next().get_next())
            return True
        return False
        
    def cleanup(self):
        del self.pair
        del self.next


class VarList:

    def __init__(self, head = None):
        if ((head == None) or (isinstance(head, VarNode))):
            self.head = head
        else:
            raise TypeError(f"Variable List instantiated with {head}")

    def __repr__(self):
        if (self.head == None):
            return "||"
        lst_str = f"{self.head}"
        curr_node = self.head.get_next()
        while (curr_node != None):
            lst_str = lst_str + f"{curr_node}"
            curr_node = curr_node.get_next()
        return f"|{lst_str}|"
    
    def add(self, node):
        if (isinstance(node, VarNode)):
            curr_node = self.head
            if (curr_node == None):
                self.head = node
                return self
            while (curr_node.get_next() != None):
                curr_node = curr_node.get_next()
            curr_node.attach(node)
            return self
        else:
             raise TypeError(f"Cannot add {node} to a Variable List, must be a Variable Node.")
        
    def remove(self, key):
        if (isinstance(key, str)):
            curr_node = self.head
            if (curr_node.extract()[0] == key):
                self.head = curr_node.get_next()
                curr_node.cleanup()
                return self
            while (curr_node.get_next().extract()[0] != key):
                if (curr_node.get_next().get_next() == None):
                    return self
                curr_node = curr_node.get_next()
            curr_node.jump()
            return self
        else:
            raise TypeError(f"Tried to remove with {key}.")


class VarMap:

    var_arr = []

    def __init__(self):
        pass

    def __repr__(self):
        return repr(self.var_arr)
    
    def insert(self, pair):
        if (isinstance(pair, tuple) and (isinstance(pair[0], str)) and (len(pair) == 2)):
            idx = ord(pair[0][0].lower()) - 97
            var_lst = self.var_arr[idx]
            var_lst.add(VarNode(pair))


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