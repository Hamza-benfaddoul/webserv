#!/usr/bin/php-cgi
<?php

    // Accessing CGI environment variables in PHP
    $requestMethod = $_SERVER['REQUEST_METHOD'];
    $contentType = $_SERVER['CONTENT_TYPE'];
    // $queryString = $_SERVER['QUERY_STRING'];


    // build function that send error 
    function sendError()
    {
        echo "response << HTTP/1.1 500 Internal Server Error" . PHP_EOL;
        echo "Content-Type: " . "text/html" . PHP_EOL;
        $size_body = 56;
        echo "Content-Length: " . $size_body . PHP_EOL;
        echo PHP_EOL;
        print("<html><body>");
        echo '<h2>Internal Server Error</h2>';
        print("</body></html>");
    }


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
                echo "HTTP/1.1 200 Success" . PHP_EOL;
                echo "Content-Type: " . "text/html" . PHP_EOL;
                $size_body = 63 + strlen($fileName);
                echo "Content-Length: " . $size_body . PHP_EOL;
                echo PHP_EOL;
                print("<html><body>");
                echo '<h2>File Upload Successful in: </h2>' . $fileName;
                print("</body></html>");
            } else {
                sendError();
            }
        } else {
            sendError();
        }
    }

    exit();

?>