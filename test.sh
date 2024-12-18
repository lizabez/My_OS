#!/bin/bash

K=100
currentTurn=5
player1_total=30
player2_total=25
experiments=100000

threads=(1 4 7 10 13 16)

program="./out/lab_2.out"

if [ ! -x "$program" ]; then
    echo "Исполняемый файл $program не найден или не является исполняемым."
    exit 1
fi

for num_threads in "${threads[@]}"; do
    echo "Запуск программы с $num_threads потоками..."
    $program $K $currentTurn $player1_total $player2_total $experiments $num_threads
    echo "--------------------------------------------------"
done