#!/bin/bash

export LD_LIBRARY_PATH=../Client:$LD_LIBRARY_PATH

./client1_async &
./client2_async &
./client3_async &
./client4_async &
./client5_async &

./client1_async &
./client2_async &
./client3_async &
./client4_async &
./client5_async &


./client1_sync &
./client2_sync &
./client3_sync &
./client4_sync &
./client5_sync &

./client1_sync &
./client2_sync &
./client3_sync &
./client4_sync &
./client5_sync &

wait

echo "\nClients ended execution."
