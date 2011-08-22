#! /bin/sh

n=${1:-5}
limit=${2:-255}

for i in `seq 1 1 $limit`
do
	echo "[38;$n;${i}mcolor $i"
done 
