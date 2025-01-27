#!/bin/bash

cd /mnt/data/ludhome

chmod +x *.sh
chown -R ludhome:ludhome .

ln -s src/service/ludhome_mqtt_saver.service /etc/systemd/system/ludhome_mqtt_saver.service

systemctl daemon-reload

systemctl enable ludhome_mqtt_saver
systemctl start ludhome_mqtt_saver
systemctl status ludhome_mqtt_saver

