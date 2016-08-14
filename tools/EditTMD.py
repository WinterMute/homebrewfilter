#!/usr/bin/env python
# -*- coding: utf-8 -*-
# version 1.0

import os
import sys
import re
import binascii
import argparse
import string

parser = argparse.ArgumentParser()
parser.add_argument('inputfile', action='store', help='Input a TMD file, or a directory that will be recursively searched for TMD files with filenames ending in \'tmd\' and \'tmd.out\'')
parser.add_argument('-title', action='store', dest='title', help='Enter a new Title ID, which must be 16 hexadecimal characters long')
version = parser.add_mutually_exclusive_group()
version.add_argument('-versionH', action='store', dest='versionH', help='Enter a new version number in hex. 0 to FFFF/0x0 to 0xFFFF')
version.add_argument('-versionD', action='store', type=int, dest='versionD', help='Enter a new version number in decimal. 0 to 65535')
arguments = parser.parse_args()

newversion = -1
newtitle = ''

if arguments.versionH is not None:
    if 0 <= int(arguments.versionH,16) <= 65535:
        newversion = int(arguments.versionH,16)
        versionPrintType = 'Hex'
    else:
        print 'The version number must be from 0 to FFFF - not ' + arguments.versionH
        sys.exit(0)

if arguments.versionD is not None:
    if 0 <= arguments.versionD <= 65535:
        newversion = arguments.versionD
        versionPrintType = 'Dec'
    else:
        print 'The version number must be from 0 to 65535 - not ' + str(arguments.versionD)
        sys.exit(0)

if arguments.title is not None:
    if (len(arguments.title) is 16) & all(c in string.hexdigits for c in arguments.title):
        newtitle = arguments.title
    else:
        print 'The Title ID must be 16 hexadecimal characters long'
        sys.exit(0)

print '*******\nEditTMD\n*******\n'

def tmdProcess(fpath):
    print '**********'
    with open(fpath) as f:
        tmd = f.read()
    tmdoffsets = [m.start() for m in re.finditer(b'Root-CA00000001-CP00000004', tmd)]
    if (len(tmdoffsets) != 1):
        print fpath + ' - Is this really a (decrypted) tmd file?'
    else:
        tmd = bytearray(tmd)
        offs = tmdoffsets[0]
        print fpath
        
        if newtitle:
            print 'Title ID: ' + binascii.hexlify(tmd[offs+0x4C:offs+0x54]) + " -> " + arguments.title
            tmd[offs+0x4C:offs+0x54] = binascii.a2b_hex(arguments.title)

        if newversion is not -1: # ' "if newversion:" is not good enough since the version can be 0, but it would still return false'
            if versionPrintType is 'Dec':
                print 'Version: ' + str(int(binascii.hexlify(tmd[offs+0x9C:offs+0x9E]),16)) + " -> " + str(newversion)
            if versionPrintType is 'Hex':
                print 'Version: ' + binascii.hexlify(tmd[offs+0x9C:offs+0x9E]).upper() + " -> " + "%0.4X" % newversion
            tmd[offs+0x9C:offs+0x9E] = binascii.a2b_hex("%0.4X" % newversion)
        
        with open(fpath, "wb") as f:
            f.write(tmd)
    print ''

if os.path.exists(arguments.inputfile):
    if (os.path.isdir(arguments.inputfile)):
        for dname, dirs, files in os.walk(arguments.inputfile):
            for fname in files:
                if(fname.lower().endswith('tmd')) or (fname.lower().endswith('tmd.out')):  # if(fname.lower() == 'tmd'):
                    fpath = os.path.join(dname, fname)
                    if os.path.isfile(fpath):
                        tmdProcess(fpath)

    elif os.path.isfile(arguments.inputfile):
        tmdProcess(arguments.inputfile)

print 'Finished'
