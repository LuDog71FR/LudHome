SELECT id, device_id, name, unit, date_created
  FROM sensor
 WHERE name = $1;
