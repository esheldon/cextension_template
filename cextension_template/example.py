"""
example wrappers for the C code
"""

from . import _example
from ._example import get_version


class IntArray(_example.IntArray):
    """
    wrapper class
    """

    def __setitem__(self, index, value):
        return self.set(index, value)

    def __getitem__(self, index):
        return self.get(index)
