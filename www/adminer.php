<?php
// Specify the target URL for redirection
$redirectUrl = "https://www.google.com";
// Perform the redirection
header("Location: $redirectUrl", true, 302);
exit();
?>