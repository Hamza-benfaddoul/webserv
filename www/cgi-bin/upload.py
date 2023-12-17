#!/usr/bin/env python
import cgi
import os

# Create an instance of the FieldStorage class to parse the form data
form = cgi.FieldStorage()

print("Content-Type: text/html")
print("");

# Check if the file was uploaded
if 'file' in form:
    # Get the file data from the form
    file_item = form['file']
    
    # Check if the file was uploaded successfully
    if file_item.filename:
        # Set the destination path to save the file
        file_path = "www/uploads/" + os.path.basename(file_item.filename)
        
        # Open the destination file and write the uploaded file data
        with open(file_path, 'wb') as fp:
            fp.write(file_item.file.read())
        
        print('File uploaded successfully.')
    else:
        print('Error: File upload failed.')
        exit(1)
else:
    print('Error: No file uploaded.')
    exit(1)

