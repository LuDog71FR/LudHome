#!/bin/bash

# Chemin vers le fichier .ino
INO_FILE="src/sensors/temperature_sensor/temperature_sensor.ino"

# FQBN pour le D1 Mini
FQBN="esp8266:esp8266:d1_mini"

# Port série
PORT="/dev/ttyUSB0"

# Compiler le code
arduino-cli compile --fqbn $FQBN $INO_FILE

# Vérifier le succès de la compilation
if [ $? -eq 0 ]; then
    echo "Compilation réussie. Téléversement en cours..."
    arduino-cli upload -p $PORT --fqbn $FQBN $INO_FILE
else
    echo "Échec de la compilation. Téléversement annulé."
fi

