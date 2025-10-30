"""ESPHome component for TI IWR6843 mmWave Radar with Single/Dual-UART interface."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import uart
from esphome.const import (
    CONF_ID,
)

CODEOWNERS = ["@bytelink-ai"]
DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "binary_sensor"]

CONF_CONFIG_UART_ID = "config_uart_id"
CONF_DATA_UART_ID = "data_uart_id"
CONF_SOP2_PIN = "sop2_pin"
CONF_NRST_PIN = "nrst_pin"
CONF_CEILING_HEIGHT = "ceiling_height"
CONF_MAX_TRACKS = "max_tracks"
CONF_UPDATE_INTERVAL = "update_interval"

iwr6843_ns = cg.esphome_ns.namespace("iwr6843")
IWR6843Component = iwr6843_ns.class_(
    "IWR6843Component", cg.Component
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(IWR6843Component),
        cv.Required(CONF_CONFIG_UART_ID): cv.use_id(uart.UARTComponent),
        cv.Required(CONF_DATA_UART_ID): cv.use_id(uart.UARTComponent),
        cv.Optional(CONF_SOP2_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_NRST_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_CEILING_HEIGHT, default=250): cv.int_range(min=100, max=500),
        cv.Optional(CONF_MAX_TRACKS, default=5): cv.int_range(min=1, max=20),
        cv.Optional(CONF_UPDATE_INTERVAL, default="50ms"): cv.positive_time_period_milliseconds,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    # Config UART (CLI Port)
    config_uart = await cg.get_variable(config[CONF_CONFIG_UART_ID])
    cg.add(var.set_config_uart(config_uart))
    
    # Data UART (Data Port - can be same as config_uart for Single-UART mode)
    data_uart = await cg.get_variable(config[CONF_DATA_UART_ID])
    cg.add(var.set_data_uart(data_uart))
    
    # Check if Single-UART mode (same UART for both)
    if config[CONF_CONFIG_UART_ID] == config[CONF_DATA_UART_ID]:
        cg.add(var.set_single_uart_mode(True))
    
    # SOP2 Pin (Boot mode) - optional
    if CONF_SOP2_PIN in config:
        sop2_pin = await cg.gpio_pin_expression(config[CONF_SOP2_PIN])
        cg.add(var.set_sop2_pin(sop2_pin))
    
    # NRST Pin (Reset) - optional
    if CONF_NRST_PIN in config:
        nrst_pin = await cg.gpio_pin_expression(config[CONF_NRST_PIN])
        cg.add(var.set_nrst_pin(nrst_pin))
    
    # Parameters
    cg.add(var.set_ceiling_height(config[CONF_CEILING_HEIGHT]))
    cg.add(var.set_max_tracks(config[CONF_MAX_TRACKS]))
    cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))

