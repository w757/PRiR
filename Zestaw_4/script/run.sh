#!/bin/bash
export KM="java -jar /cluster-app/clientOMP.jar 192.168.1.16 Simulation.h Simulation.cpp"
export TMP_KM="eval $KM | grep -o '[0-9]*\.a\.out'"


$TMP_KM > x.txt
#sbatch skrypt | grep -oP 'Submitted batch job (\d+)' | awk '{print $NF}'
job_id=$( sbatch skrypt | grep -oP 'Submitted batch job (\d+)' | awk '{print $NF}')
echo $job_id
cp Simulation.cpp /cluster-home/rzeszute_1206727/4_zadanie/5/result/Simulation_"$job_id".cpp

squeue
