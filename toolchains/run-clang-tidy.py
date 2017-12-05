#!/usr/bin/env python3

import json
import os
import sys
import subprocess

BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE = range(8)

def make_absolute(f, directory):
	if os.path.isabs(f):
		return f
	return os.path.normpath(os.path.join(directory, f))

def printout(text, color=WHITE):
	seq = "\x1b[1;%dm" % (30+color) + text + "\x1b[0m"
	sys.stdout.write(seq)

database = json.load(open('compile_commands.json', 'r'))
files = [ make_absolute(entry['file'], entry['directory']) for entry in database ]

files.sort()

i = 0

for file in files:
	
	if not os.path.isfile(file):
		continue
	
	sys.stdout.write('[{:3.0f}%] '.format(100. * i / (len(files) - 1)))
	printout('Checking ' + file, GREEN)
	sys.stdout.write('\n')
	
	invocation = [ 'clang-tidy', '-p=' + os.getcwd() ] + sys.argv[1:] + [ file ]
	
	subprocess.call(invocation)
	
	i = i + 1
