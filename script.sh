#!/bin/bash
for i in {0..100};do
	for  j in {0..49}; do
		./bin/cliente 127.0.0.1 4535 ../files/aleatorio/archivo$j archivo$j
	done
done
