#[derive(Debug)]
pub struct Topic {
    pub device_mqtt_id: String,
    pub sensor_name: String,
    pub data_name: String,
    pub category: Option<String>,
}

impl Topic {
    pub fn from_str(topic: &str) -> Result<Self, &'static str> {
        let parts: Vec<&str> = topic.split('/').collect();

        if parts.len() < 5 || parts.len() > 6 {
            return Err("Invalid topic format");
        }

        if parts[0] != "devices" {
            return Err("Invalid topic prefix");
        }

        let device_mqtt_id = parts[1].to_string();
        let sensor_name = parts[3].to_string();
        let data_name = parts.last().unwrap().to_string();

        let category = if parts.len() == 6 {
            Some(parts[4].to_string())
        } else {
            None
        };

        Ok(Topic {
            device_mqtt_id,
            sensor_name,
            data_name,
            category,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_topic() {
        let topic = Topic::from_str("devices/13640837/sensors/temperature/value").unwrap();
        assert_eq!(topic.device_mqtt_id, "13640837");
        assert_eq!(topic.sensor_name, "temperature");
        assert_eq!(topic.data_name, "value");
        assert_eq!(topic.category, None);
    }

    #[test]
    fn test_topic_with_category() {
        let topic = Topic::from_str("devices/13640837/sensors/temperature/ambient/value").unwrap();
        assert_eq!(topic.device_mqtt_id, "13640837");
        assert_eq!(topic.sensor_name, "temperature");
        assert_eq!(topic.data_name, "value");
        assert_eq!(topic.category, Some("ambient".to_string()));
    }

    #[test]
    fn test_topic_invalids() {
        assert!(Topic::from_str("device/13640837/sensors/temperature/value").is_err());
        assert!(Topic::from_str("devices/13640837/hello").is_err());
    }
}

