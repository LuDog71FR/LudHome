#!/bin/bash

chmod +x *.sh

ln -s /mnt/data/ludhome/ludhome_mqtt_saver.service /etc/systemd/system/ludhome_mqtt_saver.service

systemctl daemon-reload

systemctl enable ludhome_mqtt_saver
systemctl start ludhome_mqtt_saver

