#!/bin/bash

# Replace "your_program" with the actual name of your program
PROGRAM_NAME="webserv"

# Get the process ID (PID) of the program
PID=$(pgrep "$PROGRAM_NAME")

# Check if the program is running
if [ -n "$PID" ]; then
    # Use lsof to list open file descriptors and close them
    lsof -p "$PID" | awk '{if($4 == "0r" || $4 == "0u") print "/proc/'$PID'/fd/"$4}' | xargs -I {} sh -c 'exec {}<&-'
    echo "Closed all file descriptors for $PROGRAM_NAME with PID $PID."
else
    echo "$PROGRAM_NAME is not running."
fi

