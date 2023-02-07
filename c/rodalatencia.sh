#!/bin/bash
echo "------------- COPIAR (ctrl-c) somente a partir da linha abaixo: -----------"

echo "Executando 10 vezes com [2650000] msgs de [8]B (bloqueante):"    
for vez in {1..10}  # 10 vezes
do
  mpirun --hostfile hostfile.txt ./pingpong 700000 8 8 | grep -oP '(?<=total_time_in_micro: )[^ ]*'
done
echo "Executando 10 vezes com [21500] msgs de [1000]B bytes (bloqueante):"  
for vez in {1..10}  # 10 vezes
do
  mpirun --hostfile hostfile.txt ./pingpong 10000 1000 8 | grep -oP '(?<=total_time_in_micro: )[^ ]*'
done
echo "Executando 10 vezes com [5500] msgs de [4000]B (bloqueante):"  
for vez in {1..10}  # 10 vezes
do
  mpirun --hostfile hostfile.txt ./pingpong 25000 4000 8 | grep -oP '(?<=total_time_in_micro: )[^ ]*'
done
echo "Executando 10 vezes com [1400] msgs de [16000]B (bloqueante):"
for vez in {1..10}  # 10 vezes
do
  mpirun --hostfile hostfile.txt ./pingpong 7000 16000 8 | grep -oP '(?<=total_time_in_micro: )[^ ]*'
done