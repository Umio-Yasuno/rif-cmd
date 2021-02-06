#!/bin/sh

c++ rif-cmd.cpp set-param.cpp -Wall -L./RadeonImageFilter/Ubuntu18/Dynamic/ -L./RadeonImageFilter/Ubuntu18/Static/ -l RadeonImageFilters -o rif-cmd
