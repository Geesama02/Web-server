#!/usr/bin/env python3

import os

# CGI header
print("Content-Type: text/html")  # Indicate that the response is plain text
print()  # Blank line to separate headers from content

# Print all environment variables
print("Environment Variables:")
for key, value in os.environ.items():
    print(f"{key} = {value}\n")
    print()
    