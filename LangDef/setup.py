# Setup file to handle package imports for parent and sibling directories.
# Running 'pip install .' in the LangDef folder will correctly import into all files.

from setuptools import setup, find_packages

setup(name = 'BppLanguageDefinition', version = '1.0', packages = find_packages())