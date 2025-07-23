#!/usr/bin/env python3

import os
import cgi
import sys

form = cgi.FieldStorage()
file_item = form["file"]

if file_item.filename:
	file_name = file_item.filename
	file_path = os.path.join("cgi-bin/storage", file_name)
	contents = file_item.file.read()
	with open(file_path, "wb") as f:
		f.write(contents)
	print("Content-Type: text/plain\r\n\r\n")
	print("CGI Executed Succesfully!")
else:
	print("Content-Type: text/plain\r\n\r\n")
	print("Error: No file uploaded.")
	sys.exit(1)