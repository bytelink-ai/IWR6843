"""Sensor platform for IWR6843."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    STATE_CLASS_MEASUREMENT,
    UNIT_EMPTY,
)
from . import IWR6843Component, CONF_ID as IWR6843_ID

DEPENDENCIES = ["iwr6843"]

CONF_IWR6843_ID = "iwr6843_id"
CONF_NUM_TARGETS = "num_targets"
CONF_FRAME_NUMBER = "frame_number"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_IWR6843_ID): cv.use_id(IWR6843Component),
        cv.Optional(CONF_NUM_TARGETS): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_FRAME_NUMBER): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_IWR6843_ID])
    
    if CONF_NUM_TARGETS in config:
        sens = await sensor.new_sensor(config[CONF_NUM_TARGETS])
        # Update in component loop
        cg.add(parent.set_num_targets_sensor(sens))
    
    if CONF_FRAME_NUMBER in config:
        sens = await sensor.new_sensor(config[CONF_FRAME_NUMBER])
        cg.add(parent.set_frame_number_sensor(sens))

