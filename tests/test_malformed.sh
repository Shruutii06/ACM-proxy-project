#!/bin/bash
echo "Testing malformed request..."
printf "BADREQUEST\r\n\r\n" | nc localhost 8888
