#ESPHome - Marlin2
### A component that reads Marlin status and messages from the uart.
![alt text](image.png)

A configured uart is required.

Example:
```yaml
sensor:
  - platform: marlin2
    uart_id: uart_bus #optional
    bed_temperature:
      name: Current Bed Temp
    bed_set_temperature:
      name: Set Bed Temp
    ext_temperature:
      name: Current Ext Temp
    ext_set_temperature:
      name: Set Ext Temp
    print_progress:
      name: Progress
```

Notes:
* https://github.com/ssieb/esphome_components/tree/master/components/serial_csv
* https://github.com/esphome/esphome/blob/dev/esphome/components/dht/sensor.py#L34
* https://github.com/mulcmu/esphome-marlin-uart

Full COnfiguration:
```yaml
esphome:
  name: vasek-ender-3
  friendly_name: vasek-ender-3

external_components:
  - source: components

esp8266:
  board: d1_mini

# Enable logging
logger:
  baud_rate: 0
  esp8266_store_log_strings_in_flash: False

# Enable Home Assistant API
api:
  encryption:
    key: ""

ota:
  - platform: esphome
    password: ""

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

  manual_ip:
    static_ip: 192.168.1.252
    gateway: 192.168.0.1
    subnet: 255.255.255.0

  # Enable fallback hotspot (captive portal) in case wifi connection fails
  ap:
    ssid: "Vasek-Ender-3 Fallback Hotspot"
    password: ""

captive_portal:

web_server:
  port: 80
  version: 3
  
uart:
  id: uart_bus
  tx_pin: GPIO1
  rx_pin: GPIO3
  baud_rate: 115200

switch:
  - platform: restart
    name: "Restart Printer"

sensor:
  - platform: uptime
    type: seconds
    name: Uptime Sensor

  - platform: marlin2
    uart_id: uart_bus
    bed_temperature:
      name: Current Bed Temp
    bed_set_temperature:
      name: Set Bed Temp
    ext_temperature:
      name: Current Ext Temp
    ext_set_temperature:
      name: Set Ext Temp
```