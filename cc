#!/bin/sh

gcc -ffreestanding -fno-builtin -nostdlib -std=c89 -pedantic -I./include/ -Wall -Wextra -Wno-builtin-declaration-mismatch -O2 $@ -Lbuild/release/ -lministd
