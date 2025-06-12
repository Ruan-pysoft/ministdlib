#!/bin/sh

gcc -ffreestanding -fno-builtin -Wno-builtin-declaration-mismatch -nostdlib -std=c89 -pedantic -I./src/ $@ -L. -lministd
