#!/bin/bash
echo "Testing blocked domain..."
curl -x localhost:8888 http://bbc.co.uk
