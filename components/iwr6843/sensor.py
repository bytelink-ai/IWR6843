"""Sensor platform for IWR6843."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    STATE_CLASS_MEASUREMENT,
    UNIT_EMPTY,
    UNIT_METER,
)
from . import IWR6843Component, CONF_ID as IWR6843_ID

# Custom unit for velocity (not available in esphome.const)
UNIT_METER_PER_SECOND = "m/s"

DEPENDENCIES = ["iwr6843"]

CONF_IWR6843_ID = "iwr6843_id"
CONF_NUM_TARGETS = "num_targets"
CONF_FRAME_NUMBER = "frame_number"

# Track-spezifische Sensoren
CONF_TRACK_ID = "track_id"
CONF_POSITION_X = "position_x"
CONF_POSITION_Y = "position_y"
CONF_POSITION_Z = "position_z"
CONF_VELOCITY_X = "velocity_x"
CONF_VELOCITY_Y = "velocity_y"
CONF_VELOCITY_Z = "velocity_z"
CONF_DISTANCE = "distance"

TRACK_SENSOR_SCHEMA = cv.Schema({
    cv.Required(CONF_TRACK_ID): cv.int_range(min=0, max=19),
    cv.Optional(CONF_POSITION_X): sensor.sensor_schema(
        unit_of_measurement=UNIT_METER,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_POSITION_Y): sensor.sensor_schema(
        unit_of_measurement=UNIT_METER,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_POSITION_Z): sensor.sensor_schema(
        unit_of_measurement=UNIT_METER,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_VELOCITY_X): sensor.sensor_schema(
        unit_of_measurement=UNIT_METER_PER_SECOND,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_VELOCITY_Y): sensor.sensor_schema(
        unit_of_measurement=UNIT_METER_PER_SECOND,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_VELOCITY_Z): sensor.sensor_schema(
        unit_of_measurement=UNIT_METER_PER_SECOND,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_DISTANCE): sensor.sensor_schema(
        unit_of_measurement=UNIT_METER,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
})

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
        cv.Optional("tracks"): cv.ensure_list(TRACK_SENSOR_SCHEMA),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_IWR6843_ID])
    
    if CONF_NUM_TARGETS in config:
        sens = await sensor.new_sensor(config[CONF_NUM_TARGETS])
        cg.add(parent.set_num_targets_sensor(sens))
    
    if CONF_FRAME_NUMBER in config:
        sens = await sensor.new_sensor(config[CONF_FRAME_NUMBER])
        cg.add(parent.set_frame_number_sensor(sens))
    
    # Track Sensoren
    if "tracks" in config:
        for track_config in config["tracks"]:
            track_id = track_config[CONF_TRACK_ID]
            
            if CONF_POSITION_X in track_config:
                sens = await sensor.new_sensor(track_config[CONF_POSITION_X])
                cg.add(parent.set_track_sensor(track_id, 0, sens))  # 0 = X
            
            if CONF_POSITION_Y in track_config:
                sens = await sensor.new_sensor(track_config[CONF_POSITION_Y])
                cg.add(parent.set_track_sensor(track_id, 1, sens))  # 1 = Y
            
            if CONF_POSITION_Z in track_config:
                sens = await sensor.new_sensor(track_config[CONF_POSITION_Z])
                cg.add(parent.set_track_sensor(track_id, 2, sens))  # 2 = Z
            
            if CONF_VELOCITY_X in track_config:
                sens = await sensor.new_sensor(track_config[CONF_VELOCITY_X])
                cg.add(parent.set_track_sensor(track_id, 3, sens))  # 3 = VX
            
            if CONF_VELOCITY_Y in track_config:
                sens = await sensor.new_sensor(track_config[CONF_VELOCITY_Y])
                cg.add(parent.set_track_sensor(track_id, 4, sens))  # 4 = VY
            
            if CONF_VELOCITY_Z in track_config:
                sens = await sensor.new_sensor(track_config[CONF_VELOCITY_Z])
                cg.add(parent.set_track_sensor(track_id, 5, sens))  # 5 = VZ
            
            if CONF_DISTANCE in track_config:
                sens = await sensor.new_sensor(track_config[CONF_DISTANCE])
                cg.add(parent.set_track_sensor(track_id, 6, sens))  # 6 = Distance

