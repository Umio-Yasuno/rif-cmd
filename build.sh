#!/bin/sh

c++ ./src/rif-cmd.cpp ./src/set-param.c -L./RadeonImageFilter/Ubuntu18/Dynamic/ -L./RadeonImageFilter/Ubuntu18/Static/ -l RadeonImageFilters -o rif-cmd
