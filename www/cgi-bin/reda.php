
<?php

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // $stdin = fopen('php://stdin', 'r');
    // $fileContent = file_get_contents('php://stdin');
    // Parse the content into an associative array
    // parse_str($fileContent, $data);

    // $name = $data['name'];
    $name = $_POST['name'];
    // $name = "walid";
    header('status: 200 OK');
    // $size_body = 62 + strlen($fileName);
    $size_body = 34 + strlen($name);
    header("Content-Length: " . $size_body);
    header("Content-Type: text/html");
    print("<html><body>");
    echo "Hello, " . $name . "!";
    print("</body></html>");
}

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PHP Form Test</title>
</head>
<body>
    <form method="POST" action="">
        <label for="name">Name:</label>
        <input type="text" id="name" name="name">
        <input type="submit" value="Submit">
    </form>


</body>
</html>

