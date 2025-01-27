#!/bin/bash

echo "Starting LudHome MQTT Saver"

cd /mnt/data/ludhome

export DATABASE_URL='postgres://ludhome@localhost/ludhome'
export RUST_LOG='info'

target/release/ludhome_mqtt_saver

