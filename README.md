# A component that reads CSV values from the uart.

A configured uart is required.

Configure a list of sensors.  The index is required, the rest is the standard sensor config.

Example:
```yaml
sensor:
  - platform: serial_csv
    uart_id: my_uart   # optional
    bed_temperature:
      name: Current Bed Temp
    ext_temperature:
      name: Current Ext Temp
```


https://github.com/ssieb/esphome_components/tree/master/components/serial_csv
https://github.com/esphome/esphome/blob/dev/esphome/components/dht/sensor.py#L34
