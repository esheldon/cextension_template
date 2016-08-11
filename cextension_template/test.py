from __future__ import with_statement, print_function
import sys, os

import unittest

from . import example

def test():
    suite = unittest.TestLoader().loadTestsFromTestCase(TestExample)
    res=unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()

    if not res:
        sys.exit(1)


class TestExample(unittest.TestCase):
    def testBasic(self):
        vers=example.get_version() 

        self.assertEqual(vers,"0.9.0");

        size=35
        arr=example.IntArray(35)

        self.assertEqual(arr.get_size(), size,"check size");

        index=3
        value=25

        # test the function versions
        arr.set(index, value)
        self.assertEqual(arr.get(index), value, "check value");

        # test the [] versions
        index = 8
        value = -1035
        arr[index] = value
        self.assertEqual(arr[index], value, "check value []");

    def testIndexError(self):

        size=100
        arr=example.IntArray(size)

        try:
            arr[size*2] = 25
            ok=True
        except IndexError:
            ok=False

        self.assertFalse(ok,"out of bounds high check")

        try:
            arr[-10] = 25
            ok=True
        except IndexError:
            ok=False

        self.assertFalse(ok,"out of bounds low check")
