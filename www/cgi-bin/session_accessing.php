<?php
    // Start the session
    session_start();

    // Access session variables
    if (isset($_SESSION["user_id"]) && isset($_SESSION["username"]))
    {
        $user_id = $_SESSION['user_id'];
        $username = $_SESSION['username'];
        echo "User ID: $user_id<br>";
        echo "Username: $username";
    }
    else
        echo "The session is empty !!!!";


?>
