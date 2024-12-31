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
CONF_MARLIN2_ID = "marlin2_id"

Marlin2 = cg.esphome_ns.class_('Marlin2', cg.Component)

CONFIG_SCHEMA = cv.All(
    cv.Schema({
            cv.GenerateID(): cv.declare_id(Marlin2),
    })
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA),
)

@automation.register_action("marlin2.set_bed_temperature", SetTemperatureAction, SWITCH_ACTION_SCHEMA)
async def marlin2_set_bed_temperature_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)