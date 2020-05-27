#!/bin/bash

NOW=$(date+%Y-%m-%d)

echo $NOW

FILE = "joy_test_$NOW"

gcc -o $FILE joy2can.c canSend.c lib.c lib.h


