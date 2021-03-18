#!/bin/sh

c++ ./src/rif-cmd.cpp ./src/set-param.c -Wall -Wextra -L./RadeonImageFilter/Ubuntu18/Dynamic/ -L./RadeonImageFilter/Ubuntu18/Static/ -l RadeonImageFilters -o rif-cmd
# c++ -c ./src/rif-cmd.cpp ./src/set-param.c -Wall -Wextra -L./RadeonImageFilter/Ubuntu18/Dynamic/ -L./RadeonImageFilter/Ubuntu18/Static/ -l RadeonImageFilters
# c++ rif-cmd.o set-param.o -Wall -Wextra -L./RadeonImageFilter/Ubuntu18/Dynamic/ -L./RadeonImageFilter/Ubuntu18/Static/ -l RadeonImageFilters -o rif-cmd
