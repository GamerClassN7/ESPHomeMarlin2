import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_INDEX, CONF_SENSORS, CONF_HUMIDITY, CONF_MODEL, CONF_PIN, CONF_TEMPERATURE, STATE_CLASS_MEASUREMENT, UNIT_CELSIUS, UNIT_PERCENT, DEVICE_CLASS_TEMPERATURE

CODEOWNERS = ["@jonatanrek"]

DEPENDENCIES = ['uart']

CONF_BED_TEMPERATURE = "bed_temperature"

serial_ns = cg.esphome_ns.namespace('serial')

Marlin2 = serial_ns.class_('Marlin2', cg.Component, sensor.Sensor, uart.UARTDevice)

CONFIG_SCHEMA = uart.UART_DEVICE_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(Marlin2),
        cv.Optional(CONF_BED_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_BED_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_BED_TEMPERATURE])
        cg.add(var.set_temperature_sensor(sens))