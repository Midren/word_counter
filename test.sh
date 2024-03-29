#!/usr/bin/env bash
if [ "$#" -ne 5 ]; then
    echo "Illegal number of parameters"
    echo "Example of usage:"
    echo "./itos_exp <number of repetitions> <path single thread exe> <path multithread exe> <path to file> <number of threads>"
else
    printf "infile=$4\nout_by_a=res_a.txt\nout_by_n=res_n.txt\nthreads=1\n" > config.dat
    ./$2 config.dat > etalon_a.txt
    mv res_a.txt etalon_a.txt
    mv res_n.txt etalon_n.txt
    for thread in `seq 1 $5`;
    do
        printf "infile=$4\nout_by_a=res_a.txt\nout_by_n=res_n.txt\nthreads=$thread\n" > config.dat
        gt=10000000
        for i in `seq 1 $1`;
        do
            t=$(./$3 config.dat)
            IFS=" "
            read -ra time <<< "$t"
            t=${time[1]}
            res=$(diff -y res_a.txt etalon_a.txt)
            if [ $? -ne 0 ]
            then 
                echo Method $method is not working well 
                break
                gt="-"
            fi
            res=$(diff -y res_n.txt etalon_n.txt)
            if [ $? -ne 0 ]
            then 
                echo Method $method is not working well 
                break
                gt="-"
            fi
            gt=$(($t >$gt ? $gt:$t ))
        done
        echo Time with $thread threads is $gt
    done
    rm etalon_a.txt etalon_n.txt res_a.txt res_n.txt
fi
