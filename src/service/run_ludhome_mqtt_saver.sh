#!/bin/bash
source ~/.bashrc

echo "Starting LudHome MQTT Saver"

cd /mnt/data/ludhome

export RUST_LOG='info'
target/release/ludhome_mqtt_saver

