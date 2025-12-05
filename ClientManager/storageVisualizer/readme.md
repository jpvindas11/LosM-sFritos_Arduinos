## Manual de Compilación y Uso

Este componente permite visualizar y exportar archivos de los logs de usuarios y sensores.
Incluye un Makefile que facilita la compilación en dos modos:

sensor → usa el disco sensorStorage.bin

user → usa el disco userLogsStorage.bin

## Compilación

Dentro de la carpeta:


_ClientManager/storageVisualizer/_



puedes compilar en dos modos:

* Modo Sensor

Compila el visualizador apuntando al archivo:

.../storage/sensorStorage.bin


Comando:

make sensor


Esto:

define la macro -DDISK_A

compila usando esa configuración

genera el ejecutable en bin/storageVisualizer

* Modo Usuario

Compila el visualizador apuntando al archivo:

_../UserLogs/userLogsStorage.bin_


Comando:

make user


Esto define la macro -DDISK_B.

## Ejecución

Una vez compilado, se puede ejecutar con:

make run


O directamente:

./bin/storageVisualizer

# Binarios precompilados

## Si está en la carpeta de release

Puede utilizar los binarios 

./bin/storageVisualizerUsers
./bin/storageVisualizerSensors

Para extraer respectivamente los datos de el servidor deseado