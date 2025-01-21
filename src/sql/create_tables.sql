-- This file contains the SQL commands to create the tables to store MQTT messages in the database.
--
-- Examples:
-- MQTT: /ludhome/devices/13640837/battery/voltage/value=3.57
-- device: mqtt_id=13640837, sensor: name==battery, sensor_data: category=voltage, name=value, value=3.57
--
-- MQTT: /ludhome/devices/13640837/temperature/value=15
-- device: mqtt_id=13640837, sensor: name==temperature, sensor_data: category=, name=value, value=15
--
DROP TABLE IF EXISTS sensor_data;
DROP TABLE IF EXISTS sensor;
DROP TABLE IF EXISTS device;

drop index if exists idx_device_name_lower;
drop index if exists idx_sensor_name_lower;
drop index if exists idx_sensor_data_name_lower;

CREATE TABLE IF NOT EXISTS device (
    id                      SERIAL          PRIMARY KEY,
    mqtt_id                 VARCHAR(20)     UNIQUE NOT NULL,

    name                    VARCHAR(30)     , -- Example: temperature/humidity sensor, smart plug ...
    location                VARCHAR(30)     , -- Example: living room, bedroom, kitchen ...
    
    date_created            TIMESTAMP       WITH TIME ZONE NOT NULL DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS sensor (
    id                      SERIAL          PRIMARY KEY,
    device_id               INTEGER         NOT NULL,
    name                    VARCHAR(30)     NOT NULL, -- Example: temperature, humidity, battery, rssi ...
    unit                    VARCHAR(20)     ,
    date_created            TIMESTAMP       WITH TIME ZONE NOT NULL DEFAULT NOW(),

    FOREIGN KEY (device_id) REFERENCES device (id)
);

CREATE TABLE IF NOT EXISTS sensor_data (
    id                      SERIAL          PRIMARY KEY,
    sensor_id               INTEGER         NOT NULL,
    category                VARCHAR(30)     , -- Example: voltage, percentage ...
    name                    VARCHAR(30)     NOT NULL, -- Example: value, status ...
    value                   VARCHAR(20)     ,
    date_created            TIMESTAMP       WITH TIME ZONE NOT NULL DEFAULT NOW(),

    FOREIGN KEY (sensor_id) REFERENCES sensor (id)
);

CREATE INDEX idx_device_name_lower ON device (lower(name));
CREATE INDEX idx_sensor_name_lower ON sensor (lower(name));
CREATE INDEX idx_sensor_data_name_lower ON sensor_data (lower(name));

