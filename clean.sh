#!/bin/bash

bii clean
bii find
bii deps --details
bii cpp:configure
cp -r ./netbeans_prbkp/nbproject ./cmake
