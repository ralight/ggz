#!/bin/sh

export LD_LIBRARY_PATH=../jggzdmod/:$LD_LIBRARY_PATH
java -cp classes:../jggzdmod/classes CoffeeToe
