# Development

## prerequisites

- Rust
- Cargo
- OpenSSL
- Mosquitto

## Install Rust

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

## Install OpenSSL

For Debian/Ubuntu:

```bash
sudo apt-get install libssl-dev
```

For Arch Linux:

```bash
sudo pacman -S openssl
```

## Install Mosquitto

For Debian/Ubuntu:

```bash
sudo apt-get install mosquitto mosquitto-clients
```

For Arch Linux:

```bash
sudo pacman -S mosquitto
```

## SQLx

SQLx + Cargo command line with Postgres support:

```bash
cargo install sqlx-cli --no-default-features --features native-tls,postgres
```

SQLx prepare to cache the database schema:

```bash
sqlx prepare
```

## Build

```bash
cargo Build
```

## Build for release

```bash
cargo build --release
```

## Build for Raspberry Pi

add ARM target to the toolchain:

```bash
rustup target add armv7-unknown-linux-gnueabihf
```

Before building, install the cross-compilation toolchain:

For Debian/Ubuntu:

```bash
sudo apt-get install gcc-arm-linux-gnueabihf
```

For Arch Linux:

```bash
yay -S arm-linux-gnueabihf-gcc
```

Last, build for ARM:

```bash
cargo build --target=arm-unknown-linux-gnueabihf --release
```

copy the binary to the Raspberry Pi:

```bash
scp target/arm-unknown-linux-gnueabihf/release/ludhome_mqtt_saver ludhome@ludhome:/mnt/data/ludhome/
```

## Build on the Raspberry Pi itself

First install the Rust toolchain:

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

Ensure libssl-dev is installed:

```bash
sudo apt-get install libssl-dev
```

Then, build the project:

```bash
cargo build --release
```

## Test

```bash
cargo test
```

## Run

```bash
RUST_LOG=info cargo run
```

