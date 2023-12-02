#!/usr/bin/php-cgi

<?php
    // Accessing CGI environment variables in PHP
    $requestMethod = $_SERVER['REQUEST_METHOD'];
    $contentType = $_SERVER['CONTENT_TYPE'];
    // $queryString = $_SERVER['QUERY_STRING'];
    function sendError()
    {
        header('status: 500 Internal Server Error');
        header('Content-Type: " . "text/html');
        header('Content-Length: 56');
        // echo "status: HTTP/1.1 500 Internal Server Error" . PHP_EOL;
        // echo "Content-Type: " . "text/html" . PHP_EOL;
        // echo "Content-Length: 56" . PHP_EOL;
        // echo PHP_EOL;
        print("<html><body>");
        echo '<h2>Internal Server Error</h2>';
        print("</body></html>");
    }
    // set_time_limit(300);
    if ($_SERVER['REQUEST_METHOD'] === 'POST') 
    {
        // Read binary content from stdin
        $stdin = fopen('php://stdin', 'r');
        // Read the entire content from stdin
        // $bodyContent = stream_get_contents($stdin);


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
            // while(!feof($outputFile))
            // {
            //     $readed = fread($stdin, 1024);
            //     file_put_contents($fileName, $readed, FILE_APPEND);
            //     // fwrite($outputFile, fread($stdin, 1024), 1024);
            //     // flush();
            // }
            fclose($outputFile);
            // fclose($stdin);
            header('status: 200 OK');
            $size_body = 62 + strlen($fileName);
            header("Content-Length: " . $size_body);
            header("Content-Type: text/html");
            print("<html><body>");
            echo '<h2>File Upload Successful in: </h2>' . $fileName;
            print("</body></html>");
        }
        else
        {
            die("error thats it");
            // sendError();
        }








        // Check if binary content is not empty
        // if ($bodyContent !== false && !empty($bodyContent)) {
        //     // Specify the directory where you want to store the uploaded files
        //     $uploadDir = '../uploads/';
        //     // $uploadDir = __DIR__ . '/uploads/';

        //     // Create the upload directory if it doesn't exist
        //     if (!file_exists($uploadDir)) {
        //         mkdir($uploadDir, 0777, true);
        //     }

        //     // Generate a unique filename (you may adjust this based on your needs)
        //     $extension = explode("/", $contentType);
        //     $fileName = $uploadDir . '_uploaded_file' . uniqid() . "." . $extension[1];

        //     // Write the binary content to the file
        //     if (file_put_contents($fileName, $bodyContent) !== false) {
        //         header('status: 200 OK');
        //         $size_body = 62 + strlen($fileName);
        //         header("Content-Length: " . $size_body);
        //         header("Content-Type: text/html");
                
        //         // echo "status: HTTP/1.1 200 Success" . PHP_EOL;
        //         // echo "Content-Type: " . "text/html" . PHP_EOL;
        //         // echo "Content-Length: " . $size_body . PHP_EOL;
        //         // echo PHP_EOL;
        //         print("<html><body>");
        //         echo '<h2>File Upload Successful in: </h2>' . $fileName;
        //         print("</body></html>");
        //     } else {
        //         sendError();
        //     }
        // } else {
        //     sendError();
        // }
    }
    exit();

?>