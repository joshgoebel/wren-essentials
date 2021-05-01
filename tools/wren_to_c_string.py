#!/usr/bin/env python
# coding: utf-8

import argparse
import glob
import os.path
import re

# The source for the Wren modules that are built into the VM or CLI are turned
# include C string literals. This way they can be compiled directly into the
# code so that file IO is not needed to find and read them.
#
# These string literals are stored in files with a ".wren.inc" extension and
# #included directly by other source files. This generates a ".wren.inc" file
# given a ".wren" module.

PREAMBLE = """// Generated automatically from {0}. Do not edit.
static const char* {1}ModuleSource =
{2};
"""

def wren_to_c_string(input_path, wren_source_lines, module):
  wren_source = ""
  # cut off blank lines at the bottom
  while (wren_source_lines[-1].strip()==""):
    wren_source_lines.pop()
  for line in wren_source_lines:
    line = line.replace('"', "\\\"")
    line = line.replace("\n", "\\n")
    wren_source += '"' + line + '"\n'

  wren_source = wren_source.strip()

  return PREAMBLE.format(input_path, module, wren_source)

def process_file(path):
  infile = os.path.basename(path)
  outfile = path + ".inc"
  # print("{} => {}").format(path.replace("src/",""), outfile)

  with open(path, "r") as f:
    wren_source_lines = f.readlines()

  module = os.path.splitext(infile)[0]
  module = module.replace("opt_", "")
  module = module.replace("wren_", "")

  return wren_to_c_string(infile, wren_source_lines, module)



def main():
  files = glob.glob("src/modules/*.wren")
  with open("src/modules/wren_code.inc", "w") as f:

    for file in files:
      source = process_file(file)
      f.write(source + "\n")

main()
