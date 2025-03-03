#!/usr/bin/env python3

import sys
import os

# Read the Content-Length header to determine the body size
  ##
  ### Read the request body from stdin
 #content_length = int(os.environ.get('CONTENT_LENGTH', 0))
 #body = sys.stdin.read(content_length)
  ##
  ### Print the HTTP response header
  ##
   ##Print the received bod
 #print("Content-Type: text/html")
 #print()
 #print(body)
#!/usr/bin/env python3
 
 
 
 #print("CGI Environment Variables:\n")
 #!/usr/bin/env python3

#!/usr/bin/env python3
#!/usr/bin/env python3
import os
import sys
import datetime
import time
import random
import string

# Function to generate a random session ID
def generate_session_id(length=16):
    chars = string.ascii_letters + string.digits
    return ''.join(random.choice(chars) for _ in range(length))

# Function to get the current time as a timestamp
def get_current_time():
    return int(time.time())

# Parse cookies from HTTP_COOKIE environment variable
def parse_cookies(cookie_string):
    cookies = {}
    if not cookie_string:
        return cookies
    
    for cookie in cookie_string.split('; '):
        if '=' in cookie:
            key, value = cookie.split('=', 1)
            cookies[key] = value
    
    return cookies

# Get cookies from environment variable
cookie_string = os.environ.get('HTTP_COOKIE', '')
cookies = parse_cookies(cookie_string)

# Get the current timestamp
current_time = get_current_time()

# Check if we have a session cookie
has_valid_session = False
new_cookie = None

# Parse the session_id which contains both the ID and expiration time
if 'session_id' in cookies:
    parts = cookies['session_id'].split(':', 1)
    if len(parts) == 2:
        session_id, expiration_str = parts
        try:
            expiration_time = int(expiration_str)
            # Check if session is still valid
            if expiration_time > current_time:
                has_valid_session = True
            else:
                # Session expired, don't create a new one
                has_valid_session = False
                session_id = "EXPIRED"
                expiration_time = 0
        except ValueError:
            # Invalid expiration format
            session_id = "INVALID"
            expiration_time = 0
    else:
        # Malformed session_id
        session_id = "MALFORMED"
        expiration_time = 0
else:
    # No session yet, create a new one (expires in 1 hour)
    session_id = generate_session_id()
    expiration_time = current_time + 10  # 1 hour from now
    new_cookie = f"session_id={session_id}:{expiration_time}; Path=/"
    has_valid_session = True

# Parse query string for form submission
query_string = os.environ.get('QUERY_STRING', '')
form_data = {}
if query_string:
    for param in query_string.split('&'):
        if '=' in param:
            key, value = param.split('=', 1)
            form_data[key] = value

# Check if expire action was requested
manually_expired = False
if 'expire' in form_data and has_valid_session:
    # Force expiration
    session_id = "MANUALLY_EXPIRED"
    expiration_time = current_time - 1
    new_cookie = f"session_id={session_id}:{expiration_time}; Path=/"
    has_valid_session = False
    manually_expired = True

# Start output
print("Content-Type: text/html")
if new_cookie:
    print(f"Set-Cookie: {new_cookie}")
print("")

# Output the page content
print("<!DOCTYPE html>")
print("<html>")
print("<head>")
print("    <title>Session Test</title>")
print("    <style>")
print("        body { font-family: Arial, sans-serif; margin: 40px; }")
print("        .container { padding: 20px; border: 1px solid #ddd; border-radius: 5px; }")
print("        .info { margin-bottom: 15px; }")
print("        .expired { color: red; }")
print("        .valid { color: green; }")
print("    </style>")
print("</head>")
print("<body>")
print("    <div class='container'>")

if has_valid_session:
    # Show "still logged in" page
    print("        <h2 class='valid'>Session Active</h2>")
    print("        <div class='info'>You are logged in with an active session.</div>")
    
    # Calculate time left
    time_left = expiration_time - current_time
    expires_dt = datetime.datetime.fromtimestamp(expiration_time)
    
    print(f"        <div class='info'>Session ID: {session_id}</div>")
    print(f"        <div class='info'>Expires in: {time_left} seconds</div>")
    print(f"        <div class='info'>Expiration time: {expires_dt.strftime('%Y-%m-%d %H:%M:%S')}</div>")
    
    # Add a form to manually expire the session
    print("        <form method='get'>")
    print("            <input type='hidden' name='expire' value='true'>")
    print("            <input type='submit' value='Expire Session'>")
    print("        </form>")
else:
    # Show "expired" page
    print("        <h2 class='expired'>Session Expired</h2>")
    print("        <div class='info'>Your session has expired and no new session will be created.</div>")
    if session_id in ["EXPIRED", "MANUALLY_EXPIRED"]:
        print("        <div class='info'>To create a new session, please clear your cookies and reload the page.</div>")
    elif session_id in ["INVALID", "MALFORMED"]:
        print("        <div class='info'>Your session cookie is invalid. Please clear your cookies and reload the page.</div>")

# Show feedback for manual actions
if manually_expired:
    print("        <p>Session manually expired. No new session will be created.</p>")

# Debug information
print("        <hr>")
print("        <h3>Debug Information:</h3>")
print("        <pre>")
print(f"Current timestamp: {current_time}")
print(f"Current time: {datetime.datetime.fromtimestamp(current_time).strftime('%Y-%m-%d %H:%M:%S')}")

print("\nSession Status:")
print(f"Has valid session: {has_valid_session}")
print(f"Session ID: {session_id}")
if has_valid_session:
    print(f"Expiration time: {datetime.datetime.fromtimestamp(expiration_time).strftime('%Y-%m-%d %H:%M:%S')}")
else:
    print("Session expired")

print("\nEnvironment Variables:")
for key, value in sorted(os.environ.items()):
    if key in ['HTTP_COOKIE', 'QUERY_STRING', 'REQUEST_METHOD', 'REMOTE_ADDR', 'HTTP_USER_AGENT']:
        print(f"{key}: {value}")

print("\nParsed Cookies:")
for key, value in cookies.items():
    print(f"{key}: {value}")

if new_cookie:
    print("\nNew Cookie Being Set:")
    print(new_cookie)
print("        </pre>")

print("    </div>")
print("</body>")
print("</html>")
