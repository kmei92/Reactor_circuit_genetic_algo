#PBS -N test.cpp
#PBS -l walltime=1:00:00
#PBS -l select=1:ncpus=4:mem=1GB


gcc ~/acse-4-project-3-platinum/build/main.o

