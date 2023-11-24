#!/usr/bin/php-cgi
<?php

// function generateUniqueFileName($originalName) {
//     $timestamp = time();
//     $randomString = bin2hex(random_bytes(8)); // Generate a random string

//     $extension = pathinfo($originalName, PATHINFO_EXTENSION);

//     return "{$timestamp}_{$randomString}.{$extension}";
// }

// // Perform file handling and storage operations
// $uploadedFile = $_FILES['file'];
// $storagePath = '/path/to/uploads/';
// $uniqueFileName = generateUniqueFileName($uploadedFile['name']);
// $destination = $storagePath . $uniqueFileName;

// if (move_uploaded_file($uploadedFile['tmp_name'], $destination)) {
//     chmod($destination, 0644);
//     echo "File uploaded successfully!";
// } else {
//     echo "Error handling file upload.";
// }




// Accessing CGI environment variables in PHP
$requestMethod = $_SERVER['REQUEST_METHOD'];
// $queryString = $_SERVER['QUERY_STRING'];
$contentType = $_SERVER['CONTENT_TYPE'];



// if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    //     $uploadDir = 'uploads/';
    //     $uploadFile = $uploadDir . basename($_FILES['fileToUpload']['name']);
    
    //     if (move_uploaded_file($_FILES['fileToUpload']['tmp_name'], $uploadFile)) {
        //         echo 'File is valid, and was successfully uploaded.';
        //     } else {
            //         echo 'Upload failed.';
            //     }
            // }
    // header("Location: http://127.0.0.1:8081/form.html");
    // exit();

    echo "hellow webser from php";
?>