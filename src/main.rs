use tokio::task;

use rumqttc::{AsyncClient, EventLoop, MqttOptions, QoS};
use std::error::Error;
use std::time::Duration;

fn create_conn() -> (AsyncClient, EventLoop) {
    let mut mqttoptions = MqttOptions::new("ludhome-backend", "ludhome", 1883);
    mqttoptions
        .set_keep_alive(Duration::from_secs(10))
        .set_manual_acks(true)
        .set_clean_session(false)
        .set_credentials("ludovic", "sdiaea");

    AsyncClient::new(mqttoptions, 10)
}

#[tokio::main(flavor = "current_thread")]
async fn main() -> Result<(), Box<dyn Error>> {
    pretty_env_logger::init();

    let (client, mut eventloop) = create_conn();

    client
        .subscribe("hello/world", QoS::AtLeastOnce)
        .await
        .unwrap();

    task::spawn(async move {
       client
          .publish("hello/world", QoS::AtLeastOnce, false, "hi".as_bytes().to_vec())
          .await
          .unwrap();

        client.disconnect().await.unwrap();
    });

    loop {
        // get subscribed messages without acking
        let event = eventloop.poll().await;
        match &event {
            Ok(notif) => {
                println!("Event = {notif:?}");
            }
            Err(error) => {
                println!("Error = {error:?}");
                break;
            }
        }
    }

    Ok(())
}

