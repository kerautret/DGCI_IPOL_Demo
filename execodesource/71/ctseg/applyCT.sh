#!/bin/sh

convert $1  source.pgm;
convert $2 mask.pgm;

ctseg source.pgm mask.pgm $3



