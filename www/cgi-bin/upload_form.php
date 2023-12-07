<?php
    ini_set('post_max_size', '2000M');
    ini_set('upload_max_filesize', '2000M');
  function sendError()
  {
      header('Content-Type: text/html');
      echo "\r\n\r\n";
      echo "Failed to upload the file";
  }

// Check if the form was submitted
if ($_SERVER["REQUEST_METHOD"] == "POST") {
   
    // Check if file was uploaded without errors
    if (isset($_FILES["file"]) && $_FILES["file"]["error"] == UPLOAD_ERR_OK) {
        // Set the desired path to store the file
        $desired_path = '../uploads/';
        if (!is_dir($desired_path)) {
            mkdir($desired_path, 0777, true);
        }

        // Get the file details
        $file_name = $_FILES["file"]["name"];
        $file_tmp = $_FILES["file"]["tmp_name"];
        
        // Set the destination path to save the file
        $destination = $desired_path . $file_name;
        $pathToSend = "uploads/" . $file_name;
        // Move the uploaded file to the desired path
        if (move_uploaded_file($file_tmp, $destination)) {
            header("Content-Type: text/html");
            echo '<h2>File Upload Successful in: </h2>' . $pathToSend;
        } else {
            sendError();
        }
    } else {
        sendError();
    }
}

?>
