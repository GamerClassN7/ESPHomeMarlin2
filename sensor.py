import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.components import sensor
# from esphome.components import text_sensor
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
from . import Marlin2

CONF_BED_TEMPERATURE = "bed_temperature"
CONF_BED_SET_TEMPERATURE = "bed_set_temperature"

CONF_EXT_TEMPERATURE = "ext_temperature"
CONF_EXT_SET_TEMPERATURE = "ext_set_temperature"

CONF_PRINT_PROGRESS = "print_progress"
CONF_PRINT_TIME = "print_time"
CONF_PRINT_TIME_REMAINING = "print_time_remaining"

CONF_MARLIN = "marlin2"
CONFIG_SCHEMA =  cv.Schema(
    {
        cv.GenerateID(CONF_MARLIN): cv.use_id(Marlin2),
        cv.Optional(CONF_BED_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BED_SET_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_EXT_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_EXT_SET_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_PRINT_PROGRESS): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_PRINT_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECOND,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_PRINT_TIME_REMAINING): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECOND,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
).extend(cv.polling_component_schema("15s"))

async def to_code(config):
    server = await cg.get_variable(config[CONF_MARLIN])

    for sName in [CONF_BED_TEMPERATURE, CONF_BED_SET_TEMPERATURE, CONF_EXT_TEMPERATURE, CONF_EXT_SET_TEMPERATURE, CONF_PRINT_PROGRESS, CONF_PRINT_TIME, CONF_PRINT_TIME_REMAINING]:
        if sName in config:
            sens = await sensor.new_sensor(config[sName])
            cg.add(server.add_sensor(sName,sens))