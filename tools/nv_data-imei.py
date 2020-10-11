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

# sysexits.h
class SysExit(object):
    #define EX_USAGE        64      /* command line usage error */
    EX_USAGE = sh.ErrorReturnCode_64
    #define EX_NOINPUT      66      /* cannot open input */
    EX_NOINPUT = sh.ErrorReturnCode_66

def usage(progname):
    print("{} [test]".format(progname))
    sys.exit(1)

commands = [
    "list-supported",
    "read-imei",
    "write-imei",
    "bruteforce-imei",
]

def get_output(data):
    return str(data).replace(os.linesep, "")

class NvDataImei(object):
    def __init__(self):
        srcdir = os.environ.get('srcdir', None)
        # Enable also to test without automake
        if not srcdir:
            srcdir = os.path.dirname(sys.argv[0])

        self.nv_data_imei = sh.Command(srcdir + os.sep + "nv_data-imei")
    def test_help(self):
        try:
            self.nv_data_imei("")
        except SysExit.EX_USAGE:
            pass
        else:
            raise Exception()

        for help_arg in ["-h", "--help"]:
            self.nv_data_imei(help_arg)
            for command in commands:
                self.nv_data_imei(command, help_arg)
                try:
                    self.nv_data_imei("file", command, help_arg)
                except SysExit.EX_USAGE:
                    pass
                else:
                    raise Exception()

        self.nv_data_imei("list-supported")

    def test_commands(self):
        # Create nv_data.bin
        valid_imei = "123456789012345"
        offset = 0x100
        XMM616_NV_DATA_SIZE = 0x200000
        nv_data_bin = get_output(sh.mktemp())
        sh.ddrescue("/dev/zero", nv_data_bin, "-s", str(XMM616_NV_DATA_SIZE))

        self.nv_data_imei(nv_data_bin, "write-imei", "-o", str(hex(offset)),
                          "-i", valid_imei)
        output = get_output(self.nv_data_imei(nv_data_bin, "read-imei", "-o",
                                              str(hex(offset))))
        print(output)
        expect = "IMEI: " + valid_imei
        if output != expect:
            raise Exception()

        output = get_output(self.nv_data_imei(nv_data_bin, "bruteforce-imei",
                                              "-i", valid_imei))
        print(output)
        expect = re.escape("Found IMEI at {} ({})".format(str(hex(offset)),
                                                          offset))
        if not re.search(expect, output):
            raise Exception()

        inaccessible_nv_data_bin = str(sh.mktemp("-u")).replace(os.linesep,"")
        sh.ddrescue("/dev/zero", inaccessible_nv_data_bin, "-s",
                    str(XMM616_NV_DATA_SIZE))
        sh.chmod("000", inaccessible_nv_data_bin);
        try:
            self.nv_data_imei(inaccessible_nv_data_bin, "write-imei",
                              "-o", "0x0", "-i", valid_imei)
            self.nv_data_imei(inaccessible_nv_data_bin, "read-imei",
                              "-o", "0x0")
            self.nv_data_imei(inaccessible_nv_data_bin, "bruteforce-imei",
                              "-i", valid_imei)
        except SysExit.EX_NOINPUT:
            pass
        else:
            raise Exception()

def main():
    nv_data_imei = NvDataImei()
    nv_data_imei.test_help()
    nv_data_imei.test_commands()

if __name__ == '__main__':
    rc = main()
    sys.exit(rc)
