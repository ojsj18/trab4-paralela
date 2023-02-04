#!/bin/bash
echo "------------- COPIAR (ctrl-c) somente a partir da linha abaixo: -----------"

echo "Executando 10 vezes com [2650000] msgs de [8]B (bloqueante):"    
for vez in {1..10}  # 10 vezes
do
   mpirun --hostfile hostfile.txt ./pingpong 2650000 8 8 | grep -oP '(?<=Throughput: )[^ ]*'
done
echo "Executando 10 vezes com [21500] msgs de [1000]B bytes (bloqueante):"  
for vez in {1..10}  # 10 vezes
do
   mpirun --hostfile hostfile.txt ./pingpong 21500 1000 8 | grep -oP '(?<=Throughput: )[^ ]*'
done
echo "Executando 10 vezes com [5500] msgs de [4000]B (bloqueante):"  
for vez in {1..10}  # 10 vezes
do
   mpirun --hostfile hostfile.txt ./pingpong 5500 4000 8 | grep -oP '(?<=Throughput: )[^ ]*'
done
echo "Executando 10 vezes com [1400] msgs de [16000]B (bloqueante):"
for vez in {1..10}  # 10 vezes
do
   mpirun --hostfile hostfile.txt ./pingpong 1400 16000 8 | grep -oP '(?<=Throughput: )[^ ]*'
done