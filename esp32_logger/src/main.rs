use std::{
    error::Error,
    sync::{
        atomic::{AtomicBool, Ordering},
        Arc,
    },
};
use tokio::net::UdpSocket;

#[derive(serde::Serialize, serde::Deserialize, schemars::JsonSchema, Debug)]
struct SensorReading {
    temperature: f32,
    humidity: f32,
    pressure: f32,
    altitude: f32,
    gas_resistance: f32,
}

foxglove::static_typed_channel!(
    pub(crate) ENVIRONMENT_CHANNEL,
    "environment",
    SensorReading
);

fn parse_sensor_reading(msg: &str) -> Result<SensorReading, Box<dyn Error>> {
    let reading: SensorReading = serde_json::from_str(msg)?;
    let reading = SensorReading {
        // Convert temperature from Celsius to Fahrenheit
        temperature: reading.temperature * 9.0 / 5.0 + 32.0,
        ..reading
    };
    Ok(reading)
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let done = Arc::new(AtomicBool::default());
    ctrlc::set_handler({
        let done = done.clone();
        move || {
            done.store(true, Ordering::Relaxed);
        }
    })
    .expect("Failed to set SIGINT handler");

    // Start WebSocket server in a separate task
    let ws_server = foxglove::WebSocketServer::new();
    let ws_handle = tokio::spawn(async move {
        ws_server
            .start()
            .await
            .expect("Failed to start WebSocket server");
    });

    // Create a filename with a readable timestamp
    let timestamp = chrono::Local::now().format("%Y-%m-%d_%H-%M-%S").to_string();
    let file_name = format!("environment_{}.mcap", timestamp);

    println!("Recording to file: {}", file_name);
    let mcap = foxglove::McapWriter::new()
        .create_new_buffered_file(file_name)
        .expect("Failed to start MCAP writer");

    let socket = UdpSocket::bind("0.0.0.0:8080").await?;
    println!("Listening on 0.0.0.0:8080");

    let mut buf = [0; 1024];

    while !done.load(Ordering::Relaxed) {
        let (len, _addr) = socket.recv_from(&mut buf).await?;
        let msg = String::from_utf8_lossy(&buf[..len]);
        let sensor_reading = parse_sensor_reading(&msg)?;

        ENVIRONMENT_CHANNEL.log(&sensor_reading);
    }

    mcap.close().expect("Failed to close MCAP writer");
    ws_handle.abort(); // Stop the WebSocket server
    Ok(())
}
