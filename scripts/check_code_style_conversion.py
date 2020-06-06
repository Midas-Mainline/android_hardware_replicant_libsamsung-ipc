#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# This is a script that helps checking the initial patches that converted
# libsamsung-ipc to the kernel code style.
#
# As there is a massive amount of files to convert, and that the number of
# patches is also big, this script can hopefully make testing and reviewing
# easier.
#
# Copyright (C) 2020 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import os
import re
import sh
import sys

def usage(progname):
    print("{} <revision_range>".format(progname))
    sys.exit(1)

def run(*args):
    # print(*args)
    return sh.sh("-c", " ".join(args))

def git(*args):
    return sh.git("--no-pager", *args)

def git_log_oneline(revision_range, *args):
    return git("show",
               "--oneline",
               "-s",
               "--no-decorate",
               '--color=never',
               revision_range,
               *args).split(os.linesep)[:-1]

def git_get_diffed_files(commit1, commit2):
    return git("diff", "--name-only", commit1, commit2).split(os.linesep)[:-1]

def git_get_commit_list(revision_range):
    return git_log_oneline(revision_range, '--reverse', '--format=%h')

def checkpatch(revision_range):
    for commit in git_get_commit_list(revision_range):
        print("Checking {}".format(git_log_oneline(commit)[0]))

        # Check the commit
        try:
            diff = run("scripts" + os.sep + "checkpatch.pl", "-g", commit)
            print("  [  OK  ] Commit")
        except:
            print("  [  !!  ] Commit")

        # Check the files of the commit
        modified_files = git_get_diffed_files(commit, commit + "~1")
        for modified_file in modified_files:
            try:
                file_report = run("scripts" + os.sep + "checkpatch.pl",
                                  "-f", modified_file)
                print("  [  OK  ] {}".format(modified_file))
            except:
                print("  [  !!  ] {}".format(modified_file))

if __name__ == '__main__':
    if len(sys.argv) != 2:
        usage(sys.argv[0])

    revision_range = sys.argv[1]
    checkpatch(revision_range)
