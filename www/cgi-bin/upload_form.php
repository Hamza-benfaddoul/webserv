<?php
    ini_set('post_max_size', '2000M');
    ini_set('upload_max_filesize', '2000M');
  function sendError()
  {
      header('status: 500 Internal Server Error');
      header('Content-Type: text/html');
      header('Content-Length: 56');
      print("<html><body>");
      echo '<h2>Internal Server Error</h2>';
      print("</body></html>");
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
            header('status: 200 OK');
            $size_body = 62 + strlen($pathToSend);
            header("Content-Length: " . $size_body);
            header("Content-Type: text/html");
            print("<html><body>");
            echo '<h2>File Upload Successful in: </h2>' . $pathToSend;
            print("</body></html>");
        } else {
            sendError();
        }
    } else {
        sendError();
    }
}

?>
