#!/usr/bin/python3

import os

for key, value in os.environ.items():
    print(f"{key}: {value}")
