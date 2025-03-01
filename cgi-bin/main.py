#!/usr/bin/env python3

import sys
import os

# Read the Content-Length header to determine the body size
 #
 ## Read the request body from stdin
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
body = sys.stdin.read(content_length)
 #
 ## Print the HTTP response header
 #
  #Print the received bod
print("Content-Type: text/html")
print()
print(body)
#!/usr/bin/env python3
 #
 #import os
 #
 #
 #print("CGI Environment Variables:\n")
 #
 ## Print all environment variables
 #for key, value in os.environ.items():
 #   print(f"{key}: {value}")



