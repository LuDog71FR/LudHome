INSERT INTO sensor (device_id, name) VALUES ($1, $2) RETURNING id;

