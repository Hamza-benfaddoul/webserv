<?php
    // Start the session
    session_start();

    // Set session variables
    $_SESSION['user_id'] = 123;
    $_SESSION['username'] = 'john_doe';

    echo 'Session created.';
    exit();
?>
