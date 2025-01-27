mod mqtt_topic;
use mqtt_topic::Topic;
use dotenv::dotenv;
use std::env;
use sqlx::postgres::PgPoolOptions;
use sqlx::Pool;
use rumqttc::{AsyncClient, EventLoop, MqttOptions, QoS, Event, Incoming};
use sqlx::Postgres;
use std::error::Error;
use std::time::Duration;

#[tokio::main(flavor = "current_thread")]
async fn main() -> Result<(), Box<dyn Error>> {
    env_logger::init();
    log::info!("Starting LudHome MQTT Saver");
    
    // Load environment variables or from .env!file.
    dotenv().ok();
    let database_url = env::var("DATABASE_URL").expect("DATABASE_URL must be set");

    // Connecting to database
    let pool = PgPoolOptions::new()
        .max_connections(5)
        .connect(&database_url)
        .await?;

    // Connect to MQTT
    let (client, mut event_loop) = create_mqtt();
    client.subscribe("#", QoS::AtLeastOnce).await?;

    // Read messages from MQTT
    match read_messages(&mut event_loop, pool).await {
        Ok(_) => (),
        Err(e) => log::error!("Critical Error: {}", e),
    }

    log::info!("Stopping LudHome MQTT Saver");
    Ok(())
}

fn create_mqtt() -> (AsyncClient, EventLoop) {
    let mut mqttoptions = MqttOptions::new("read_all", "ludhome", 1883);
    mqttoptions
        .set_keep_alive(Duration::from_secs(10))
        .set_credentials("ludovic", "sdiaea");

    AsyncClient::new(mqttoptions, 10)
}

async fn read_messages(event_loop: &mut EventLoop, pool: Pool<Postgres>) -> Result<(), Box<dyn Error>> {
    loop {
        let event = event_loop.poll().await;

        match &event {
            Ok(notif) => {
                log::trace!("Event = {notif:?}");

                let message = match notif {
                  Event::Incoming(Incoming::Publish(message)) => message,
                      _ => continue,
                };

                let topic = &message.topic;
                let payload = String::from_utf8(message.payload.to_vec())?;

                parse_message(payload, topic.to_string(), &pool).await?;
            }

            Err(error) => {
                log::error!("Critical Error when polling message: {error:?}");
                break;
            }
        }
    }

    Ok(())
}

async fn parse_message(payload: String, topic: String, pool: &Pool<Postgres>) -> Result<(), Box<dyn Error>> {
    // topic: devices/<device_mqtt_id>/sensors/<sensor_name>/<data_name>
    // or topic: devices/<device_mqtt_id>/sensors/<sensor_name>/<category>/<data_name>
    // Example: Received message: 11 on topic: devices/13640837/sensors/temperature/value
    log::trace!("Received message: {} on topic: {}", payload, topic);

    match Topic::from_str(&topic) {
        Ok(topic) => {
            log::trace!("Device MQTT ID: {}", topic.device_mqtt_id);
            log::trace!("Sensor name: {}", topic.sensor_name);
            log::trace!("Data name: {}", topic.data_name);
            log::trace!("Category: {:?}", topic.category);   

            save_message(topic, payload, pool).await?;
        }
        Err(e) => {
            log::warn!("Error parsing topic: {}", e);
            return Ok(());
        }
    }

    Ok(())
}

async fn save_message(topic: Topic, payload: String, pool: &Pool<Postgres>) -> Result<(), Box<dyn Error>> {
    // If device does not exist, insert it
    let device = sqlx::query_file!("src/sql/device_select_by_mqtt_id.sql", topic.device_mqtt_id)
        .fetch_optional(pool)
        .await?;
    let device_id = match device {
        Some(device) => device.id,
        None => {
            let device = sqlx::query_file!("src/sql/device_insert.sql", topic.device_mqtt_id)
                .fetch_one(pool)
                .await?;

            device.id
        }
    };

    // If sensor does not exist, insert it
    let sensor = sqlx::query_file!("src/sql/sensor_select_by_name.sql", topic.sensor_name)
        .fetch_optional(pool)
        .await?;
    let sensor_id = match sensor {
        Some(sensor) => sensor.id,
        None => {
            let sensor = sqlx::query_file!("src/sql/sensor_insert.sql",
                device_id,
                topic.sensor_name)
                .fetch_one(pool)
                .await?;

            sensor.id
        }
    };

    // Insert sensor data
    sqlx::query_file!("src/sql/sensor_data_insert.sql",
        sensor_id,
        topic.category,
        topic.data_name,
        payload)
        .fetch_one(pool)
        .await?;

    Ok(())
}

