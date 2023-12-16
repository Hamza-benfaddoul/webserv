<?php
    setcookie("Auction_Item", "Luxury Car", time() + 2 * 24 * 60 * 60); 
?> 

<!DOCTYPE html> 
<html> 
<body> 
	<?php 
	if (isset($_COOKIE["Auction_Item"])) 
	{ 
		echo "Auction Item is a " . $_COOKIE["Auction_Item"]; 
	} 
	else
	{ 
		echo "No items for auction."; 
	} 
	?> 
	<p> 
		<strong>Note:</strong> 
		You might have to reload the page 
		to see the value of the cookie. 
	</p> 

</body> 
</html>
