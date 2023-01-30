#!/bin/bash
echo "------------- COPIAR (ctrl-c) somente a partir da linha abaixo: -----------"

echo "Executando 10 vezes com [2650000] msgs de [8]B (bloqueante):"    
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./pingpong 2650000 8 | grep -oP '(?<=Throughput: )[^ ]*'
done
echo "Executando 10 vezes com [21500] msgs de [1000]B bytes (bloqueante):"  
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./pingpong 21500 1000 | grep -oP '(?<=Throughput: )[^ ]*'
done
echo "Executando 10 vezes com [5500] msgs de [4000]B (bloqueante):"  
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./pingpong 5500 4000 | grep -oP '(?<=Throughput: )[^ ]*'
done
echo "Executando 10 vezes com [1400] msgs de [16000]B (bloqueante):"
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./pingpong 1400 16000 | grep -oP '(?<=Throughput: )[^ ]*'
done
echo "Executando 10 vezes com [4600000] msgs de [8]B (não bloqueante):"
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./pingpong 4600000 8 -nbl | grep -oP '(?<=Throughput: )[^ ]*'
done
echo "Executando 10 vezes com [40000] msgs de [1000]B (não bloqueante):"
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./pingpong 40000 1000 -nbl | grep -oP '(?<=Throughput: )[^ ]*'
done
echo "Executando 10 vezes com [10000] msgs de [4000]B (não bloqueante):"
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./pingpong 10000 4000 -nbl | grep -oP '(?<=Throughput: )[^ ]*'
done
echo "Executando 10 vezes com [2300] msgs de [16000]B (não bloqueante):"
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./pingpong 2300 16000 -nbl | grep -oP '(?<=Throughput: )[^ ]*'
done