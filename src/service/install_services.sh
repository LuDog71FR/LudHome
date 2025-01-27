#!/bin/bash

cd /mnt/data/ludhome

sudo chmod +x src/service/*.sh
sudo chown -R ludhome:ludhome .

sudo ln -s /mnt/data/ludhome/src/service/ludhome_mqtt_saver.service /etc/systemd/system/ludhome_mqtt_saver.service

sudo systemctl daemon-reload

sudo systemctl enable ludhome_mqtt_saver
sudo systemctl start ludhome_mqtt_saver
systemctl status ludhome_mqtt_saver

