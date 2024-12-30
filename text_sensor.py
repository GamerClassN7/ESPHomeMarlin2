import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import Marlin2

CONF_MARLIN = "marlin2"
CONFIG_SCHEMA =  cv.Schema(
    {
        cv.GenerateID(CONF_MARLIN): cv.use_id(Marlin2),
        cv.Optional("printer_state"): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
).extend(cv.polling_component_schema("15s"))

async def to_code(config):
    server = await cg.get_variable(config[CONF_MARLIN])

    for sName in ["printer_state"]:
        if sName in config:
            sens = await text_sensor.new_text_sensor(config[sName])
            cg.add(server.add_text_sensor(sName,sens))