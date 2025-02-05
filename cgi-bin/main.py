#!/usr/bin/env python3

import os
import time

# CGI script to generate a simple HTML response
print("Content-Type: text/html")  # HTTP header
print()  # Blank line indicating end of headers

# HTML content
print("<html>")
print("<head><title>Test CGI Script</title></head>")
print("<body>")
print("<h1>Hello from Python CGI!</h1>")
print("<p>This is a test CGI script.</p>")

# Display current time
print("<p>Current time: {}</p>".format(time.ctime()))

# Display environment variables
print("<h2>Environment Variables</h2>")
print("<ul>")
for key, value in os.environ.items():
    print(f"<li><strong>{key}:</strong> {value}</li>")
print("</ul>")

print("</body>")
print("</html>")