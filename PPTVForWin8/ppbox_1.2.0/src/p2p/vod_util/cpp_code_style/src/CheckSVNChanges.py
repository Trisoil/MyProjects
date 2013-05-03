#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pysvn
import svn_cmd
import sys
import subprocess
import os

class MySvnCmd(svn_cmd.SvnCommand):
    def __init__( self, progname ):
        svn_cmd.SvnCommand.__init__( self, progname )
        
    def status( self, args ):
        self.initClient( args.getOptionalValue( '--config-dir', '' ) )
        self.client.set_auth_cache( args.getBooleanOption( '--no-auth-cache', False ) )
        self.pysvn_testing = args.getOptionalValue( '--pysvn-testing', '99.99.99' )
        self.debug_enabled = args.getBooleanOption( '--debug', True )

        recurse = args.getBooleanOption( '--non-recursive', False )
        verbose = args.getBooleanOption( '--verbose', True )
        quiet = args.getBooleanOption( '--quiet', True )
        ignore = args.getBooleanOption( '--no-ignore', False )
        update = args.getBooleanOption( '--show-updates', True )

        file_list = []
        positional_args = args.getPositionalArgs( 0 )
        all_files = self.client.status( positional_args[1] , recurse=recurse, get_all=verbose, ignore=ignore, update=update )
        for f in all_files:
            # 判断文件的状态
            if f.text_status == pysvn.wc_status_kind.modified or f.text_status == pysvn.wc_status_kind.modified:
                file_list.append(f.path)

        return file_list

def do_check_files_lint( file_list ):
    # 在这里检测所有新增的或是修改过的文件的状态
    has_erro = False
    if len(file_list) == 0:
        print 'no file changes found'
        
    for f in file_list:
        # 检测是否是CPP或H文件
        if len(f) < 4 and len(f) > 2:
            # check .h
            if f[ len(f) - 2 : ] != ".h":
                continue
        elif len(f) >= 4:
            if f[ len(f) - 2 : ] != ".h" and f[ len(f) - 4 : ] != ".cpp":
                continue
        else:
            continue
        
        # get current path
        cpath = os.path.split( sys.argv[0] )
        cmd_line = os.path.join( cpath[0] , 'cpplint.exe ' ) 
#        cmd_line = "python cpplint.py "
        
        cmd_line += f
        print cmd_line
        process = subprocess.Popen( cmd_line , shell=True  )
        process.wait()
        has_erro = process.returncode != 0
        
    if has_erro:
        return 1
    else:
        return 0

def main() :
    try:
        cmd = MySvnCmd( "python" )
        path = "."
        if len(sys.argv) == 2:
            path = sys.argv[1]

        print 'checking: '
        print path
        
        args = svn_cmd.SvnArguments( ['stat', path] )
        file_list = cmd.status(args)
        return do_check_files_lint(file_list)
    except:
        pass
        return 0

if __name__ == '__main__':
    ret = main()
    sys.exit( ret )

