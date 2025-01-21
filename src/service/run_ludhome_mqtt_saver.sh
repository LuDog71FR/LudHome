#!/bin/bash

echo "Starting LudHome MQTT Saver"

cd /mnt/data/ludhome

export DATABASE_URL='postgres://ludhome:sdiaea@localhost/ludhome'
export RUST_LOG='info'

./ludhome_mqtt_saver_service

