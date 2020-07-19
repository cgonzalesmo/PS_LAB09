import random as r
import os

directorio = './files/aleatorio/'

if not os.path.exists(directorio):
    os.makedirs(directorio)

contador = 50
letras = 'abcdefghijklmnopqrstuvwxyz123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'

for i in range(contador):
    tamano = r.randint(1000000,10000000)
    archivo = open(directorio + 'archivo' + str(i), 'w+')

    texto = ""
    for j in range(tamano):
        texto = texto + r.choice(letras)

    archivo.write(texto)
    archivo.close()