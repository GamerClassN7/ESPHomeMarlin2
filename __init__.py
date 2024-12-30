import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import (
    CONF_ID, 
    CONF_INDEX, 
    CONF_SENSORS, 
    CONF_HUMIDITY, 
    CONF_MODEL, 
    CONF_PIN, 
    CONF_TEMPERATURE, 
    UNIT_CELSIUS, 
    UNIT_PERCENT, 
    UNIT_SECOND, 
    STATE_CLASS_MEASUREMENT, 
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_DURATION,
)

CODEOWNERS = ["@jonatanrek"]
DEPENDENCIES = ['uart']

Marlin2 = cg.esphome_ns.class_('Marlin2', cg.Component)

CONFIG_SCHEMA = cv.All(
    cv.Schema( {
            cv.GenerateID(): cv.declare_id(Marlin2),
    })
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)