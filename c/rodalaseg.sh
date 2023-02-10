#!/bin/bash
echo "------------- COPIAR (ctrl-c) somente a partir da linha abaixo: -----------"

echo "Executando 10 vezes com [700000] msgs de [8]B (bloqueante):"    
for vez in {1..10}  # 10 vezes
do
  mpirun --hostfile hostfile.txt ./mybroadcast 700000 8 8 | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [10000] msgs de [1000]B bytes (bloqueante):"  
for vez in {1..10}  # 10 vezes
do
  mpirun --hostfile hostfile.txt ./mybroadcast 100000 1000 8 | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [25000] msgs de [4000]B (bloqueante):"  
for vez in {1..10}  # 10 vezes
do
  mpirun --hostfile hostfile.txt ./mybroadcast 25000 4000 8 | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [7000] msgs de [16000]B (bloqueante):"
for vez in {1..10}  # 10 vezes
do
  mpirun --hostfile hostfile.txt ./mybroadcast 7000 16000 8 | grep -oP '(?<=Latencia: )[^ ]*'
done