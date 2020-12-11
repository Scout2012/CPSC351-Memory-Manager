# !/bin/sh

echo "Compiling memmgr.c..."
gcc memmgr.c -o memmgr
echo "Compilation completed"

echo "Running Memory Manager"
./memmgr
