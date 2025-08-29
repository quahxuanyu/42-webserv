#!/usr/bin/env python3
import os
import json

print("Content-Type: application/json\r\n\r\n")

directory = "cgi-bin/storage"  # change to your target directory
files = [f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]

print(json.dumps(files))
