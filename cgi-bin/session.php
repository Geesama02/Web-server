<?php
// Function to generate a random session ID
function generate_session_id($length = 16) {
    $chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
    $sessionId = '';
    for ($i = 0; $i < $length; $i++) {
        $sessionId .= $chars[mt_rand(0, strlen($chars) - 1)];
    }
    return $sessionId;
}

// Function to get the current time as a timestamp
function get_current_time() {
    return time();
}

// Parse cookies from HTTP_COOKIE environment variable
function parse_cookies($cookie_string) {
    $cookies = [];
    if (empty($cookie_string)) {
        return $cookies;
    }

    $cookie_parts = explode('; ', $cookie_string);
    foreach ($cookie_parts as $cookie) {
        if (strpos($cookie, '=') !== false) {
            list($key, $value) = explode('=', $cookie, 2);
            $cookies[$key] = $value;
        }
    }

    return $cookies;
}

// Function to store session data in a file
function store_session($session_id, $expiration_time) {
    $session_file = "/tmp/session_$session_id.txt";
    file_put_contents($session_file, "$session_id:$expiration_time");
}

// Function to validate session data from a file
function validate_session($session_id) {
    $session_file = "/tmp/session_$session_id.txt";
    if (!file_exists($session_file)) {
        return [false, 0];
    }

    $data = file_get_contents($session_file);
    if (strpos($data, ':') !== false) {
        list($stored_id, $expiration_str) = explode(':', $data, 2);
        if ($stored_id === $session_id) {
            $expiration_time = (int)$expiration_str;
            return [true, $expiration_time];
        }
    }
    return [false, 0];
}

// Function to delete a session file
function delete_session($session_id) {
    $session_file = "/tmp/session_$session_id.txt";
    if (file_exists($session_file)) {
        unlink($session_file);
    }
}

// Get cookies from environment variable
$cookie_string = isset($_SERVER['HTTP_COOKIE']) ? $_SERVER['HTTP_COOKIE'] : '';
$cookies = parse_cookies($cookie_string);

// Get the current timestamp
$current_time = get_current_time();

// Check if we have a session cookie
$has_valid_session = false;
$new_cookie = null;

// Parse the session_id which contains both the ID and expiration time
if (isset($cookies['session_id'])) {
    $session_id = $cookies['session_id'];
    list($is_valid, $expiration_time) = validate_session($session_id);
    if ($is_valid) {
        // Check if session is still valid
        if ($expiration_time > $current_time) {
            $has_valid_session = true;
        } else {
            // Session expired, delete the session file
            delete_session($session_id);
            $has_valid_session = false;
            $session_id = "EXPIRED";
            $expiration_time = 0;
        }
    } else {
        // Invalid session
        $session_id = "INVALID";
        $expiration_time = 0;
    }
} else {
    // No session yet, create a new one (expires in 10 seconds)
    $session_id = generate_session_id();
    $expiration_time = $current_time + 10;  // 10 seconds from now
    store_session($session_id, $expiration_time);
    $new_cookie = "session_id=$session_id; Path=/";
    $has_valid_session = true;
}

// Parse query string for form submission
$query_string = isset($_SERVER['QUERY_STRING']) ? $_SERVER['QUERY_STRING'] : '';
parse_str($query_string, $form_data);

// Check if expire action was requested
$manually_expired = false;
if (isset($form_data['expire']) && $has_valid_session) {
    // Force expiration
    delete_session($session_id);
    $session_id = "MANUALLY_EXPIRED";
    $expiration_time = $current_time - 1;
    $new_cookie = "session_id=$session_id; Path=/";
    $has_valid_session = false;
    $manually_expired = true;
}

// Start output with proper HTTP headers
echo "Content-Type: text/html\r\n";
if ($new_cookie) {
    echo "Set-Cookie: $new_cookie\r\n";
}
echo "\r\n"; // Blank line to separate headers from body
?>
<!DOCTYPE html>
<html>
<head>
    <title>Session Test</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        .container { padding: 20px; border: 1px solid #ddd; border-radius: 5px; }
        .info { margin-bottom: 15px; }
        .expired { color: red; }
        .valid { color: green; }
    </style>
</head>
<body>
    <div class='container'>
    <?php if ($has_valid_session): ?>
        <!-- Show "still logged in" page -->
        <h2 class='valid'>Session Active</h2>
        <div class='info'>You are logged in with an active session.</div>

        <?php
        // Calculate time left
        $time_left = $expiration_time - $current_time;
        $expires_dt = date('Y-m-d H:i:s', $expiration_time);
        ?>

        <div class='info'>Session ID: <?php echo htmlspecialchars($session_id); ?></div>
        <div class='info'>Expires in: <?php echo $time_left; ?> seconds</div>
        <div class='info'>Expiration time: <?php echo $expires_dt; ?></div>

        <!-- Add a form to manually expire the session -->
        <form method='get'>
            <input type='hidden' name='expire' value='true'>
            <input type='submit' value='Expire Session'>
        </form>
    <?php else: ?>
        <!-- Show "expired" page -->
        <h2 class='expired'>Session Expired</h2>
        <div class='info'>Your session has expired and no new session will be created.</div>
        <?php if (in_array($session_id, ["EXPIRED", "MANUALLY_EXPIRED"])): ?>
            <div class='info'>To create a new session, please clear your cookies and reload the page.</div>
        <?php elseif (in_array($session_id, ["INVALID", "MALFORMED"])): ?>
            <div class='info'>Your session cookie is invalid. Please clear your cookies and reload the page.</div>
        <?php endif; ?>
    <?php endif; ?>

        <!-- Debug information -->
        <hr>
        <h3>Debug Information:</h3>
        <pre>
Current timestamp: <?php echo $current_time; ?>
Current time: <?php echo date('Y-m-d H:i:s', $current_time); ?>

Session Status:
Has valid session: <?php echo $has_valid_session ? 'true' : 'false'; ?>
Session ID: <?php echo htmlspecialchars($session_id); ?>
<?php if ($has_valid_session): ?>
Expiration time: <?php echo date('Y-m-d H:i:s', $expiration_time); ?>
<?php else: ?>
Session expired
<?php endif; ?>

Environment Variables:
<?php
$env_vars = ['HTTP_COOKIE', 'QUERY_STRING', 'REQUEST_METHOD', 'REMOTE_ADDR', 'HTTP_USER_AGENT'];
foreach ($env_vars as $key) {
    if (isset($_SERVER[$key])) {
        echo "{$key}: " . htmlspecialchars($_SERVER[$key]) . "\n";
    }
}
?>

Parsed Cookies:
<?php
foreach ($cookies as $key => $value) {
    echo "{$key}: " . htmlspecialchars($value) . "\n";
}
?>

<?php if ($new_cookie): ?>
New Cookie Being Set:
<?php echo htmlspecialchars($new_cookie); ?>
<?php endif; ?>
        </pre>
    </div>
</body>
</html>
