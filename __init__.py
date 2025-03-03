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
    CONF_DATA,
)
from esphome import pins, automation

CODEOWNERS = ["@jonatanrek"]
DEPENDENCIES = ['uart']
CONF_MARLIN2_ID = "marlin2_id"

Marlin2 = cg.esphome_ns.class_('Marlin2', cg.Component)
Marlin2WriteAction = Marlin2.class_("Marlin2WriteAction", automation.Action)

CONFIG_SCHEMA = cv.All(
    cv.Schema({
            cv.GenerateID(): cv.declare_id(Marlin2),
    })
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA),
)

def validate_raw_data(value):
    if isinstance(value, str):
        return value.encode("utf-8")
    if isinstance(value, str):
        return value
    if isinstance(value, list):
        return cv.Schema([cv.hex_uint8_t])(value)
    raise cv.Invalid("data must either be a string wrapped in quotes or a list of bytes")

@automation.register_action(
    "marlin2.write",
    Marlin2WriteAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(): cv.declare_id(Marlin2),
            cv.Required(CONF_DATA): cv.templatable(validate_raw_data),
        },
        key=CONF_DATA,
    ),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)