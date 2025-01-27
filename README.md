<div align="center">

<img src="images/Logo.png" width="400px" />

# LudHome

[![Documentation](https://docs.rs/ludhome/badge.svg)][documentation]
[![Crates.io](https://img.shields.io/crates/v/ludhome.svg)](https://crates.io/crates/ludhome)
[![License](https://img.shields.io/crates/l/ludhome.svg)](https://github.com/lLuDog71FR/LudHome/blob/master/LICENSE)
[![Downloads](https://img.shields.io/crates/d/ludhome.svg)](https://crates.io/crates/ludhome)
[![Test Status](https://img.shields.io/github/actions/workflow/status/ludhome/test.yml?branch=master&event=push&label=test)](https://github.com/ludhome/actions)

</div>

# Description

An home assistant written in Rust. It is a personal project to help me to learn new technologies and to have fun.

It's separated in two main parts:

- The sensors, which are responsible to collect data and send it to the MQTT server
- The server, which is responsible to serve the data thru the Web and to read data from the MQTT server


## Features

- [ ] Web REST API to serve the data
- [ ] Web UI to display the data and dashboards
- [x] MQTT client to receive data from sensors
- Sensors:
    - [x] Temperature/Humidity sensor

