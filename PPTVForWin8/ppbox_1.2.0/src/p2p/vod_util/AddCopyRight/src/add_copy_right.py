import os
import sys

copy_right = '''//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
'''

def add_copy_right(path):
  f = file( path , 'r+w')
  content = f.read()

  content = copy_right + content
  f.seek(0)
  f.truncate(0)
  f.write( content )
  f.close()

def walk_dir(dir,topdown=True):
    for root, dirs, files in os.walk(dir):
        for name in files:
          if len(name) > 4 and name[len(name) - 4:] == ".cpp":
            add_copy_right( os.path.join( root , name ) )
          
          if len(name) > 2 and name[len(name) - 2:] == ".h":
            add_copy_right( os.path.join( root , name ) )


walk_dir(".")

