# Copyright (c) 2008 Hristo Bojinov
#
# You may use this code in any manner you want. The author does not
# make or imply any warranty that the code will behave in any specific
# way. USE AT YOUR OWN RISK. The author will not be liable for any damages
# whatsoever, resulting from the downloading and/or use of this code.

# WARNING: Originally written for Windows XP. However, likely to work
# elsewhere as well.

import sys
import os
import glob
import string
import re

# The first argument is the destination directory. Under that,
# the program will mimick the structure it finds under the current
# directory, which is assumed to be the source.
if sys.argv[1:] == []:
  print 'Usage:   convert.py <DEST_DIR>'
  print 'Please also make sure that ffmpeg[.exe] is in the current path.'
  raise SystemExit
else:
  dest_dir = sys.argv[1]
  print 'Using ', dest_dir, ' as the destination directory.'

# List all AVI files from all subdirectories
file_list = glob.glob('*/*.mov')

for file_src in file_list:
  file_src_split = os.path.split(file_src)

  dir_dst = os.path.join(dest_dir, file_src_split[0])
  if not os.path.isdir(dir_dst):
    os.makedirs(dir_dst)

  strip_ext_re = re.match("(.*)(\.[mM][oO][vV])$", file_src_split[1])
  if strip_ext_re == None:
    print 'File with no MOV extenstion: ', file_src_split[1], ' trying AVI...'
    strip_ext_re = re.match("(.*)(\.[aA][vV][iI])$", file_src_split[1])
    if strip_ext_re == None:
      print 'File with no supported extenstion: ', file_src_split[1], ' skipping!'
      continue
  
  file_dst = os.path.join(dir_dst, strip_ext_re.group(1)+'.mp4')
  if os.path.isfile(file_dst):
    print 'File exists: ', file_dst, ' Skipping!'
    continue
  
  print file_src, '->', file_dst
  os.system('ffmpeg -i ' + file_src +
            ' -s 480x270 -acodec aac -ab 96k ' +
            ' -vcodec libx264 -fpre libx264-slow.ffpreset -crf 22 -threads 0 ' +
            file_dst)

print 'Done.\n'
