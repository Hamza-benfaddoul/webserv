#!/usr/bin/php-cgi

<?php
    // Accessing CGI environment variables in PHP
    set_time_limit(60);
    $requestMethod = $_SERVER['REQUEST_METHOD'];
    $contentType = $_SERVER['CONTENT_TYPE'];
    function sendError()
    {
        header('status: 500 Internal Server Error');
        header('Content-Type: " . "text/html');
        header('Content-Length: 56');
        print("<html><body>");
        echo '<h2>Internal Server Error</h2>';
        print("</body></html>");
    }
    // set_time_limit(300);
    if ($_SERVER['REQUEST_METHOD'] === 'POST') 
    {
        $stdin = fopen('php://stdin', 'r');
        $uploadDir = "../uploads/";
        if (!file_exists($uploadDir))
            mkdir($uploadDir, 0777, true);
        // generate unique name
        $extension = explode("/", $contentType);
        $fileName = $uploadDir . '_uploaded_file'. uniqid() . "." . $extension[1];
        $outputFile = fopen($fileName, "wb");
        
        if ($outputFile)
        {
            while ($chunk = fread($stdin, 8192)) {
                fwrite($outputFile, $chunk);
            }
            fclose($outputFile);
            header("Content-Type: text/html");
            // echo "\r\n\r\n";
            echo "the file is uploaded successfuly in : " . $fileName;
            exit(0);
        }
        else
        {
            header("Content-Type: text/html");
            // echo "\r\n\r\n";
            echo "Failed to upload the file!";
            exit(1);
        }
    }
    exit(0);

?>