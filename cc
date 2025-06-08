#!/bin/sh

gcc -nostdlib -std=c89 -pedantic -I./src/ $@ -L. -lministd
