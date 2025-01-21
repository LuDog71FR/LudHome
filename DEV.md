# Development

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

## Test

```bash
cargo test
```

## Run

```bash
RUST_LOG=info cargo run
```

