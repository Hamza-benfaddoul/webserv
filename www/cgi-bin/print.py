#!/usr/bin/env python3

import os
import sys
import cgi
import cgitb

# Enable detailed error messages
cgitb.enable()

def send_error():
    print("Status: 500 Internal Server Error\r\n")
    print("Content-Type: text/html\r\n")
    print("Content-Length: 56\r\n")
    print("<html><body>\r\n")
    print('<h2>Internal Server Error</h2>\r\n')
    print("</body></html>\r\n")
    sys.exit()

def main():
    # Accessing CGI environment variables in Python
    request_method = os.environ.get('REQUEST_METHOD', '')
    content_type = os.environ.get('CONTENT_TYPE', '')

    if request_method == 'POST':
        try:
            # Read binary data from stdin
            binary_data = sys.stdin.buffer.read()

            # Print the response
            print("Status: 200 OK\r\n")
            print("Content-Type: text/html\r\n")
            print("<html><body>\r\n")

            if len(binary_data) > 0:
                print('<h2>Successful in: </h2>' + binary_data + "\r\n")
            else:
                print('empty but why ?????????!!!!' + "\r\n")
            print("</body></html>\r\n")

        except Exception as e:
            send_error()

if __name__ == "__main__":
    main()
