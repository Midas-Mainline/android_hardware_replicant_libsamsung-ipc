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

def usage(progname):
    print("{} [test]".format(progname))
    sys.exit(1)

def get_output(data):
    return str(data).replace(os.linesep, "")

class NvDataMD5(object):
    def __init__(self):
        srcdir = os.environ.get('srcdir', None)
        # Enable also to test without automake
        if not srcdir:
            srcdir = os.path.dirname(sys.argv[0])

        self.nv_data_md5 = sh.Command(srcdir + os.sep + "nv_data-md5")
    def test_help(self):
        try:
            self.nv_data_md5()
        except sh.ErrorReturnCode_1:
            pass
        else:
            raise Exception()

    def test_commands(self):
        expected_md5 = "5293814414abb3831e3fc1a1b35e69bc"
        NV_DATA_SIZE = 0x200000
        nv_data_bin = get_output(sh.mktemp())

        # Create nv_data.bin
        sh.ddrescue("/dev/zero", nv_data_bin, "-s", str(NV_DATA_SIZE))

        output = get_output(self.nv_data_md5(nv_data_bin))

        print(output)

        if output != expected_md5:
            raise Exception()

def main():
    nv_data_md5 = NvDataMD5()
    nv_data_md5.test_help()
    nv_data_md5.test_commands()

if __name__ == '__main__':
    rc = main()
    sys.exit(rc)
