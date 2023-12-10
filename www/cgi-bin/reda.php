
<?php

if ($_SERVER['REQUEST_METHOD'] === 'POST') {

    $name = $_POST['name'];
    header("Content-Type: text/html");
    echo "\r\n";
    print("<html><body>");
    echo "Hello, " . $name . "!";
    print("</body></html>");
    exit(0);
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

