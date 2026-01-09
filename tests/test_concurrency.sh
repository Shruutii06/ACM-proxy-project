#!/bin/bash
echo "Testing concurrent requests..."
for i in {1..10}; do
  curl -x localhost:8888 http://example.com &
done
wait
