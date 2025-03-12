#!/usr/bin/php
<?php
// Set the content type to HTML
header('Content-Type: text/html');

// Function to display the environment variables in a table
function showEnvVars($envVars) {
    echo "<table border='1' cellpadding='5' cellspacing='0' style='width: 100%; margin-top: 20px;'>";
    echo "<thead><tr><th>Environment Variable</th><th>Value</th></tr></thead><tbody>";
    foreach ($envVars as $key => $value) {
        echo "<tr><td>" . htmlspecialchars($key) . "</td><td>" . htmlspecialchars($value) . "</td></tr>";
    }
    echo "</tbody></table>";
}

// Function to display a form for additional query string input
function showForm($queryString = '') {
    echo <<<HTML
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>PHP CGI Environment Viewer</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                background-color: #f4f4f9;
                margin: 0;
                padding: 0;
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
                flex-direction: column;
            }
            .form-container {
                background-color: white;
                padding: 20px;
                border-radius: 8px;
                box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
                width: 320px;
                margin-top: 20px;
            }
            h1 {
                color: #4CAF50;
                text-align: center;
            }
            input[type="text"], input[type="submit"] {
                width: 100%;
                padding: 12px;
                margin: 10px 0;
                border: 1px solid #ddd;
                border-radius: 5px;
                font-size: 16px;
            }
            input[type="submit"] {
                background-color: #4CAF50;
                color: white;
                border: none;
                cursor: pointer;
            }
            input[type="submit"]:hover {
                background-color: #45a049;
            }
        </style>
    </head>
    <body>
        <h1>PHP CGI Environment Viewer</h1>
        <form method="GET">
            <div class="form-container">
                <label for="query_string">Enter Query String:</label>
                <input type="text" name="query_string" id="query_string" value="$queryString" placeholder="key=value">
                <input type="submit" value="Submit">
            </div>
        </form>
HTML;
}

// Check if there is a query string and display it
$queryString = isset($_GET['query_string']) ? $_GET['query_string'] : '';

// Initialize the environment variables array
$envVars = getenv(); // Get all environment variables as an associative array

// Show the form (with the query string value, if set)
showForm($queryString);

// Display the environment variables in a table
showEnvVars($envVars);
?>