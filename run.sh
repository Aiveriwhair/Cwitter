#!/bin/sh
gnome-terminal -x sh -c "make -C ./server/; ./server/server 5000"
gnome-terminal -x sh -c "make -C ./client/; ./client/client 127.0.0.1 5000"
