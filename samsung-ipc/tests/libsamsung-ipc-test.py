#!/usr/bin/env python
#
#  This file is part of libsamsung-ipc.
#
#  Copyright (C) 2020 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
#
#  libsamsung-ipc is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  libsamsung-ipc is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.

import os
import re
import sys
import sh

class libsamsung_ipc_test(object):
    def __init__(self):
        srcdir = os.environ.get('srcdir', None)

        # Enable also to test without automake
        if not srcdir:
            srcdir = os.path.dirname(sys.argv[0])

        self.run = sh.Command(srcdir + os.sep + "libsamsung-ipc-test")

    def run_all_tests(self):
        output = str(self.run("list-tests")).split(os.linesep)
        # Remove the last line break from the output
        output.remove('')

        # Also Remove the first line from the output: We have an output like
        # that:
        # Available tests:
        #  [list of tests]
        output.pop(0)

        for test_name in output:
            self.run("test", test_name.replace(' ', ''))

def main():
    tests = libsamsung_ipc_test()
    tests.run_all_tests()

if __name__ == '__main__':
    main()
