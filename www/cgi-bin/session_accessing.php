<?php 
  
session_start(); 
  
echo 'The Name of the student is :' . $_SESSION["Name"] . '<br>';  
echo 'The Roll number of the student is :' . $_SESSION["Rollnumber"] . '<br>'; 
  
?> 