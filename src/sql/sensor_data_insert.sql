INSERT INTO sensor_data (sensor_id, category, name, value) 
VALUES ($1, $2, $3, $4)
RETURNING id;
