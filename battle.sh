#!/bin/sh
./battles.sh | grep wins! | cut -f1 | sort | uniq -c

