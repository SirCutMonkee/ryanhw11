#!/usr/bin/env python3
# reconstructSentence.py
# by Joshua Ryan
# HW 11 problem 4
# To run this program use: reconstructSentence.py input1.txt input2.txt
# This program takes two line arguments: inputFile1 and inputFile2

import sys

input1 = open(sys.argv[1])
input2 = open(sys.argv[2])
outputFile = open("output.txt","w")

input1List = input1.read().strip().split()
input2List = input2.read().strip().split()
outputList = []
if len(input1List) >= len(input2List):
    listLen = len(input1List)
else:
    listLen = len(input2List)

for i in range(listLen):
    if input1List != []:
        outputList.append(input1List.pop())
    if input2List != []:
        outputList.append(input2List.pop())

for i in range(len(outputList)):
    outputFile.write(outputList[i])
    outputFile.write(" ")

input1.close()
input2.close()
outputFile.close()
