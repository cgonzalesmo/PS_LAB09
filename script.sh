#! /bin/bash
name="Creando instancias de cliente"
echo $name
for i in {1..10000};do
	for j in {0..49};do
		echo "Cliente  $i"
		./Cliente 127.0.0.1 14145 aleatorios/archivo$j archivo$j
	done
done
