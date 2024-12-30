import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import ns, Marlin2

CONF_MARLIN = "marlin2"
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_MARLIN): cv.use_id(Marlin2),
        cv.Optional(CONF_CONNECTED): text_sensor.text_sensor(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)

async def to_code(config):
    server = await cg.get_variable(config[CONF_MARLIN])
    for sName in ["printer_state"]:
        if sName in config:
            sens = await sensor.new_text_sensor(config[sName])
            cg.add(var.add_text_sensor(sName,sens))