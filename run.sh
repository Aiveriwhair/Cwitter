#!/bin/sh
bash -c "make -C ./server/; ./server/server 5000"
bash -c "make -C ./client/; ./client/client 127.0.0.1 5000"
