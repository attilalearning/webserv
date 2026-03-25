#!/bin/bash

# Configuration
PORT=8080
HOST="localhost"
NUM_CONNS=20
SLEEP_BETWEEN_CONNS=0.1

echo "--- Starting Churn Test: $NUM_CONNS connections ---"

# 1. Open connections in the background
pids=()
for i in $(seq 1 $NUM_CONNS); do
    # Open a connection that stays open by waiting for input
    # 'nc' will stay open until the process is killed
    sleep $SLEEP_BETWEEN_CONNS
    nc $HOST $PORT & 
    pids+=($!)
    echo "Opened connection $i (PID: ${pids[-1]})"
done

echo "--- All connections established. Shuffling PIDs for random closure ---"

# 2. Randomize the order of the PIDs
shuffled_pids=($(shuf -e "${pids[@]}"))

# 3. Kill connections one by one with a small delay
for pid in "${shuffled_pids[@]}"; do
    echo "Closing connection PID: $pid"
    kill $pid
    sleep 0.2 # Small delay to let the server process the POLLHUP/recv=0
done

echo "--- Churn Test Complete ---"
