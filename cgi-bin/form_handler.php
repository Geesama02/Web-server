#!/usr/bin/php-cgi
<?php
// Required for CGI scripts to work properly
header("Content-Type: text/html");

// Read the request method
$request_method = $_SERVER['REQUEST_METHOD'];

if ($request_method === "GET") {
    // Show the form
    echo "<!DOCTYPE html>
    <html>
    <head><title>PHP CGI Form</title></head>
    <body>
        <h2>Enter Your Details</h2>
        <form method='POST' action='form_handler.php'>
            Name: <input type='text' name='name' required><br>
            Email: <input type='email' name='email' required><br>
            <input type='submit' value='Submit'>
        </form>
    </body>
    </html>";
} elseif ($request_method === "POST") {
    // Read and parse POST data
    $input_data = file_get_contents("php://input");
    parse_str($input_data, $post_data);

    // Check if form fields are set
    $name = isset($post_data['name']) ? htmlspecialchars($post_data['name']) : "Unknown";
    $email = isset($post_data['email']) ? htmlspecialchars($post_data['email']) : "Unknown";

    // Display the results
    echo "<!DOCTYPE html>
    <html>
    <head><title>Form Submission Result</title></head>
    <body>
        <h2>Form Submitted Successfully!</h2>
        <p><strong>Name:</strong> $name</p>
        <p><strong>Email:</strong> $email</p>
        <a href='form_handler.php'>Go Back</a>
    </body>
    </html>";
} else {
    // Handle unexpected request methods
    echo "<h3>405 Method Not Allowed</h3>";
}
?>
