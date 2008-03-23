#!/bin/sh

export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
java -cp classes TicTacToe
