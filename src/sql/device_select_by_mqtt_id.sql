SELECT id, mqtt_id, name, location, date_created
  FROM device
 WHERE mqtt_id = $1;
