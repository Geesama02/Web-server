#!/usr/bin/env python3

import os
import sys
import time
import uuid

# Constants for session management
SESSION_FILE = '/tmp/current_session'
SESSION_DURATION = 10  # 10 seconds for quick testing

def create_session():
    """
    Create a new session and store its details in a file.
    
    Returns:
        tuple: (session_id, creation_time)
    """
    # Generate unique session ID
    session_id = str(uuid.uuid4())
    creation_time = int(time.time())
    
    # Store session details
    with open(SESSION_FILE, 'w') as f:
        f.write(f"{session_id}\n{creation_time}")
    
    return session_id, creation_time

def validate_session():
    """
    Check if the existing session is valid.
    
    Returns:
        tuple: (is_valid, session_id, creation_time)
    """
    # Check if session file exists
    if not os.path.exists(SESSION_FILE):
        return False, None, None
    
    # Read session details
    with open(SESSION_FILE, 'r') as f:
        lines = f.read().strip().split('\n')
        if len(lines) != 2:
            return False, None, None
        
        session_id, creation_time = lines
        creation_time = int(creation_time)
    
    # Check if session has expired
    current_time = int(time.time())
    is_valid = (current_time - creation_time) <= SESSION_DURATION
    
    return is_valid, session_id, creation_time

def main():
    """
    Main CGI script to handle session management.
    """
    # Check session validity
    is_valid, session_id, creation_time = validate_session()
    
    # Prepare to send headers
    print("Content-Type: text/html")
    
    # If no valid session exists, create a new one
    if not is_valid or not session_id:
        session_id, creation_time = create_session()
        print(f"Set-Cookie: session_id={session_id}; Path=/; Max-Age={SESSION_DURATION}")
    
    # Print the HTML response
    print()  # Empty line to separate headers from body
    print("<!DOCTYPE html>")
    print("<html>")
    print("<head>")
    print("    <title>Session Management</title>")
    print("</head>")
    print("<body>")
    
    # Display session status
    current_time = int(time.time())
    if is_valid:
        time_left = SESSION_DURATION - (current_time - creation_time)
        print(f"    <h1>Session Active</h1>")
        print(f"    <p>Session ID: {session_id}</p>")
        print(f"    <p>Time remaining: {time_left} seconds</p>")
    else:
        print("    <h1>Session Expired</h1>")
        print("    <p>Your session has expired. A new session will be created on refresh.</p>")
    
    # Debug information
    print("    <hr>")
    print("    <h2>Debug Information</h2>")
    print("    <pre>")
    print(f"    Current Time: {current_time}")
    print(f"    Session Creation Time: {creation_time}")
    print(f"    Session Duration: {SESSION_DURATION} seconds")
    print(f"    Session Valid: {is_valid}")
    print("    </pre>")
    
    print("</body>")
    print("</html>")

# Standard Python CGI entry point
if __name__ == '__main__':
    main()
