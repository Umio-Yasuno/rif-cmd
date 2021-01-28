#!/bin/sh

g++ rif-cmd.cpp set-param.cpp -L./RadeonImageFilter/Ubuntu18/Dynamic/ -L./RadeonImageFilter/Ubuntu18/Static/ -lRadeonImageFilters -o rif-cmd
