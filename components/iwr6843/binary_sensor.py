"""Binary sensor platform for IWR6843."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_OCCUPANCY,
    DEVICE_CLASS_PRESENCE,
)
from . import IWR6843Component, CONF_ID as IWR6843_ID

DEPENDENCIES = ["iwr6843"]

CONF_IWR6843_ID = "iwr6843_id"
CONF_PRESENCE = "presence"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_IWR6843_ID): cv.use_id(IWR6843Component),
        cv.Optional(CONF_PRESENCE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_PRESENCE
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_IWR6843_ID])
    
    if CONF_PRESENCE in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_PRESENCE])
        cg.add(parent.add_on_presence_callback(lambda x: sens.publish_state(x)))

