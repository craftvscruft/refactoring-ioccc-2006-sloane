#!/bin/bash

set -euo pipefail

LOG_FILE=test/data/log.txt
GOLDEN_FILE=test/data/golden.txt
rm -f $LOG_FILE

make

# Run the donut program for 5 seconds and capture its output
(./sloane | head -n 5000 || true) > $LOG_FILE

echo "Captured $LOG_FILE"
sed -i 's/\x1b\[/\\e\[/g' $LOG_FILE # Convert control sequences
# sed -i '5001,$ d' $LOG_FILE # Discard after line limit

echo "Comparing results..."
diff $GOLDEN_FILE $LOG_FILE

echo Success!