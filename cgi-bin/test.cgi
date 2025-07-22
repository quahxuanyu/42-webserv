#!/usr/bin/env python3

import os

# Importing the 'cgi' module
import cgi
# Parse form data submitted via the CGI script
# form = cgi.FieldStorage()

# print("---CGI File Begin---")
# # Check if the "name" field is present in the form data
# if form.getvalue("file"):
#     # If present, retrieve the value and display a personalized greeting
#     file = form.getvalue("file")
#     print("Content Of File:\n")
#     print(file)
# print("---CGI File End---")


form = cgi.FieldStorage()
file_item = form["file"]

if file_item.filename:
    name = file_item.filename
    file_path = os.path.join("cgi-bin/storage", name)
    contents = file_item.file.read()
    with open(file_path, "wb") as f:
        f.write(contents)
    print("CGI Executed Succesfully!")