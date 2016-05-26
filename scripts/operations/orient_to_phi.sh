#!/bin/bash

head -n1 $1
sed -n 2p $1|awk '{print $2,$1,$3}'
sed -n 3,`wc -l $1|cut -f1 -d' '`p $1|awk '{print $2,$1,$3}'|sort