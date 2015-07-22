pedalerIA64
===========

Efectos
--------------

* Delay simple:
  * ~~C: hecho.~~
  * ~~ASM: hecho.~~
  * ~~Diferencias de audio entre C y ASM en STEREO: hecho.~~
  * ~~Generar varios ejemplos con argumentos variados y ponerlos en una carpeta para cerrar el efecto: hecho.~~

* Delay multiple:
  * No hecho

* Flanger:
  * ~~C: hecho.~~
  * ~~ASM: hecho.~~
  * ~~Corroborar si hay diferencias: hecho.~~
  * ~~Probar con varios parametros: hecho.~~
  
* Vibrato:
  * ~~Hecho en C.~~
  * ASM: falta.
  * Ver diferencias.
  * Agregar tiempo restante al buffer.

* Bitcrusher:
  * ~~C: hecho.~~
  * ASM: no hecho.
  * Corroborar diferencias.
  * Ejemplos con varios parametros.
	
* WahWah:
  * ~~C: hecho.~~
  * Mejorar codigo hecho en C, no está igual a matlab.
  * ASM: no hecho.
  * Corroborar diferencias.
  * Ejemplos con varios parametros.


ToDo
--------------

* Entrada del programa:
  * Mejor manejo de parámetros, para que se pueda usar más de un efecto - que en la segunda pasada abra el archivo de salida anterior, no el de entrada.
  * Chequear valores que puede tomar cada parámetro según el efecto.
  * Que el audio de salida dure más (input + delay)

* Evaluación de performance:
  * Tomar cantidad de iteraciones como parámetro y correrlo esa cantidad de veces



