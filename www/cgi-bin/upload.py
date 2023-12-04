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
            form = cgi.FieldStorage()
            upload_dir = "www/uploads/"

            if not os.path.exists(upload_dir):
                os.makedirs(upload_dir, 0o777, True)

            # Generate unique name
            content_type_parts = content_type.split("/")
            file_extension = content_type_parts[1] if len(content_type_parts) == 2 else "txt"
            file_name = upload_dir + '_uploaded_file' + str(os.getpid()) + "." + file_extension

            # Read binary data from stdin
            binary_data = sys.stdin.buffer.read()
            # Write binary data to a file
            with open(file_name, 'wb') as output_file:
                output_file.write(binary_data)

            # Print the response
            print("Status: 200 OK\r\n")
            size_body = 62 + len(file_name)
            print("Content-Length: " + str(size_body) + "\r\n")
            print("Content-Type: text/html\r\n")
            print("<html><body>\r\n")
            print('<h2>File Upload Successful in: </h2>' + file_name + "\r\n")
            print("</body></html>\r\n")

        except Exception as e:
            send_error()

if __name__ == "__main__":
    main()
