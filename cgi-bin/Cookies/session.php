#!/usr/bin/env php
<?php
// Session configuration
define('SESSION_FILE', '/tmp/current_session');
define('SESSION_DURATION', 10);

// Create or retrieve session
$current_time = time();
$session_data = file_exists(SESSION_FILE) ? file_get_contents(SESSION_FILE) : null;

// Validate session
if ($session_data) {
    list($session_id, $creation_time) = explode("\n", $session_data);
    $is_valid = ($current_time - $creation_time) <= SESSION_DURATION;
} else {
    $is_valid = false;
}

// Generate new session if needed
if (!$is_valid) {
    $session_id = bin2hex(random_bytes(16));
    $creation_time = $current_time;
    file_put_contents(SESSION_FILE, "{$session_id}\n{$creation_time}");
    
    // Use echo for headers instead of header() function
    echo "Set-Cookie: session_id={$session_id}; Path=/; Max-Age=" . SESSION_DURATION . "\r\n";
}

// Output headers and content
echo "Content-Type: text/html\r\n\r\n";
?>
<!DOCTYPE html>
<html>
<head>
    <title>Simple Session</title>
</head>
<body>
    <?php if ($is_valid): ?>
        <?php $time_left = SESSION_DURATION - ($current_time - $creation_time); ?>
        <h1>Session Active</h1>
        <p>Session ID: <?php echo htmlspecialchars($session_id); ?></p>
        <p>Time remaining: <?php echo $time_left; ?> seconds</p>
    <?php else: ?>
        <h1>Session Expired</h1>
        <p>Your session has expired. Refresh to create a new session.</p>
    <?php endif; ?>

    <hr>
    <pre>
Current Time: <?php echo $current_time; ?>
Session Creation: <?php echo $creation_time; ?>
Session Duration: <?php echo SESSION_DURATION; ?> seconds
    </pre>
</body>
</html>
