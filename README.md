# A component that reads CSV values from the uart.

A configured uart is required.

Configure a list of sensors.  The index is required, the rest is the standard sensor config.

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


* https://github.com/ssieb/esphome_components/tree/master/components/serial_csv
* https://github.com/esphome/esphome/blob/dev/esphome/components/dht/sensor.py#L34
* https://github.com/mulcmu/esphome-marlin-uart