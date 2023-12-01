#!/bin/bash


# response="HTTP/1.1 200 OK\r\n"
# response="${response}Content-Type: image/jpeg\r\n"
# response="${response}\r\n\r\n"

echo "HTTP/1.1 200 OK"
echo "Content-Type: image/x-icon"
echo "\n"
# echo "$response"

# paste the full path to any image below
cat /nfs/homes/woumecht/Desktop/http2/www/bootstrap/favicon.ico
