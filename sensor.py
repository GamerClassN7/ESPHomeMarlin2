import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_INDEX, CONF_SENSORS, CONF_HUMIDITY, CONF_MODEL, CONF_PIN, CONF_TEMPERATURE, STATE_CLASS_MEASUREMENT, UNIT_CELSIUS, UNIT_PERCENT, DEVICE_CLASS_TEMPERATURE

CODEOWNERS = ["@jonatanrek"]

DEPENDENCIES = ['uart']

serial_ns = cg.esphome_ns.namespace('serial')

Marlin2 = serial_ns.class_('Marlin2', cg.Component, sensor.Sensor, uart.UARTDevice)

CONFIG_SCHEMA = uart.UART_DEVICE_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(Marlin2),
        cv.Required(CONF_SENSORS): cv.ensure_list(
            cv.Optional("bed_current"): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        ),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    for conf in config[CONF_SENSORS]:
        sens = await sensor.new_sensor(conf)
        index = conf[CONF_INDEX]
        cg.add(var.add_sensor(index, sens))