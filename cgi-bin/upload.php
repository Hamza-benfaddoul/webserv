#!/usr/bin/php-cgi
<?php
    // Accessing CGI environment variables in PHP
    $requestMethod = $_SERVER['REQUEST_METHOD'];
    $contentType = $_SERVER['CONTENT_TYPE'];
    // $queryString = $_SERVER['QUERY_STRING'];

    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        // Read binary content from stdin
        $bodyContent = stream_get_contents(STDIN);

        // Check if binary content is not empty
        if ($bodyContent !== false && !empty($bodyContent)) {
            // Specify the directory where you want to store the uploaded files
            $uploadDir = 'www/uploads/';
            // $uploadDir = __DIR__ . '/uploads/';

            // Create the upload directory if it doesn't exist
            if (!file_exists($uploadDir)) {
                mkdir($uploadDir, 0777, true);
            }

            // Generate a unique filename (you may adjust this based on your needs)
            $fileName = $uploadDir . '_uploaded_file' . uniqid();

            // Write the binary content to the file
            if (file_put_contents($fileName, $bodyContent) !== false) {
                echo 'File uploaded successfully! Stored as: ' . $fileName;
            } else {
                echo 'Error storing file.';
            }
        } else {
            echo 'No binary content received.';
        }
    }

    // header("Location: http://127.0.0.1:6666/");
    echo "hellow webser from php";
    exit();

?>