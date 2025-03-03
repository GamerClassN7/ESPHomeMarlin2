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
Marlin2WriteAction = uart_ns.class_("Marlin2WriteAction", automation.Action)

CONFIG_SCHEMA = cv.All(
    cv.Schema({
            cv.GenerateID(): cv.declare_id(Marlin2),
    })
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA),
)

MARLIN2_ACTION_SCHEMA = maybe_simple_id(
    {
        cv.Required(CONF_ID): cv.use_id(Marlin2),
    }
)

@automation.register_action(
    "marlin2.write",
    Marlin2WriteAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(): cv.use_id(UARTComponent),
            cv.Required(CONF_DATA): cv.templatable(validate_raw_data),
        },
        key=CONF_DATA,
    ),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)