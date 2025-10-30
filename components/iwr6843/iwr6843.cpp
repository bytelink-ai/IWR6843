#include "iwr6843.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/application.h"

namespace esphome {
namespace iwr6843 {

static const char *const TAG = "iwr6843";

// Configuration commands for overhead 3D people tracking
static const char *CONFIG_COMMANDS[] = {
    "sensorStop",
    "flushCfg",
    "dfeDataOutputMode 1",
    "channelCfg 15 7 0",
    "adcCfg 2 1",
    "adcbufCfg -1 0 1 1 1",
    "profileCfg 0 60 359 7 57.14 0 0 70 1 256 5209 0 0 158",
    "chirpCfg 0 0 0 0 0 0 0 1",
    "chirpCfg 1 1 0 0 0 0 0 2",
    "chirpCfg 2 2 0 0 0 0 0 4",
    "frameCfg 0 2 16 0 100 1 0",
    "lowPower 0 0",
    "guiMonitor -1 1 1 0 0 0 1",
    "cfarCfg -1 0 2 8 4 3 0 15 1",
    "cfarCfg -1 1 0 4 2 3 1 15 1",
    "multiObjBeamForming -1 1 0.5",
    "clutterRemoval -1 0",
    "calibDcRangeSig -1 0 -5 8 256",
    "extendedMaxVelocity -1 0",
    "bpmCfg -1 0 0 1",
    "lvdsStreamCfg -1 0 0 0",
    "compRangeBiasAndRxChanPhase 0.0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0",
    "measureRangeBiasAndRxChanPhase 0 1.5 0.2",
    "CQRxSatMonitor 0 3 4 121 0",
    "CQSigImgMonitor 0 127 4",
    "analogMonitor 0 0",
    "aoaFovCfg -1 -90 90 -90 90",
    "cfarFovCfg -1 0 0 8.92",
    "cfarFovCfg -1 1 -1 1.00",
    "calibData 0 0 0",
    "staticBoundaryBox -3 3 2 5.5 0 3",
    "boundaryBox -2.5 2.5 0.5 6 0 2.8",
    "sensorPosition 2.8 0 0",
    "gatingParam 3 1.5 1.5 0",
    "stateParam 3 3 20 1500 5 1500",
    "allocationParam 60 250 0.25 20 0.5 20",
    "maxAcceleration 0.1 0.1 0.1",
    "trackingCfg 1 2 800 20 52 96 60 40",
    "presenceBoundaryBox -3 3 0.5 6 0.25 2.8",
    "sensorStart",
    nullptr
};

void IWR6843Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up IWR6843...");
  
  // Configure pins (CS pin is handled by SPIDevice)
  this->sop2_pin_->setup();
  this->sop2_pin_->digital_write(true);  // SOP2 high for functional mode
  
  this->nrst_pin_->setup();
  this->nrst_pin_->digital_write(true);  // Not in reset
  
  // Initialize SPI
  this->spi_setup();
  
  // Reset radar
  if (!this->reset_radar_()) {
    ESP_LOGE(TAG, "Failed to reset radar");
    this->mark_failed();
    return;
  }
  
  // Wait for boot with watchdog feeding
  ESP_LOGI(TAG, "Waiting for radar boot...");
  for (int i = 0; i < 10; i++) {
    App.feed_wdt();
    delay(100);
  }
  
  // Configure radar
  ESP_LOGI(TAG, "Starting radar configuration...");
  if (!this->configure_radar_()) {
    ESP_LOGE(TAG, "Failed to configure radar");
    this->mark_failed();
    return;
  }
  
  this->configured_ = true;
  ESP_LOGI(TAG, "IWR6843 setup complete");
}

void IWR6843Component::loop() {
  if (!this->configured_) {
    return;
  }
  
  uint32_t now = millis();
  if (now - this->last_read_time_ < this->update_interval_) {
    return;
  }
  
  this->last_read_time_ = now;
  
  // Read frame from SPI
  if (this->read_frame_()) {
    // Process successful frame read
    ESP_LOGV(TAG, "Frame %d: %d targets, presence: %d", 
             this->frame_number_, this->num_targets_, this->presence_detected_);
    
    // Update sensors
    if (this->num_targets_sensor_ != nullptr) {
      this->num_targets_sensor_->publish_state(this->num_targets_);
    }
    if (this->frame_number_sensor_ != nullptr) {
      this->frame_number_sensor_->publish_state(this->frame_number_);
    }
    
    // Update track sensors
    this->update_track_sensors_();
  }
}

void IWR6843Component::dump_config() {
  ESP_LOGCONFIG(TAG, "IWR6843:");
  LOG_PIN("  SOP2 Pin: ", this->sop2_pin_);
  LOG_PIN("  NRST Pin: ", this->nrst_pin_);
  ESP_LOGCONFIG(TAG, "  Ceiling Height: %d cm", this->ceiling_height_);
  ESP_LOGCONFIG(TAG, "  Max Tracks: %d", this->max_tracks_);
  ESP_LOGCONFIG(TAG, "  Update Interval: %d ms", this->update_interval_);
  
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Configuration FAILED!");
  }
}

bool IWR6843Component::reset_radar_() {
  ESP_LOGI(TAG, "Resetting radar...");
  
  // SOP2 high = functional mode (not flashing mode)
  this->sop2_pin_->digital_write(true);
  
  // Reset sequence
  this->nrst_pin_->digital_write(false);
  delay(100);
  this->nrst_pin_->digital_write(true);
  delay(100);
  
  return true;
}

bool IWR6843Component::configure_radar_() {
  ESP_LOGI(TAG, "Configuring radar via UART...");
  
  // Send configuration commands
  for (int i = 0; CONFIG_COMMANDS[i] != nullptr; i++) {
    // Feed watchdog to prevent timeout during long configuration
    App.feed_wdt();
    yield();
    
    if (!this->send_config_line_(CONFIG_COMMANDS[i])) {
      ESP_LOGE(TAG, "Failed to send config: %s", CONFIG_COMMANDS[i]);
      return false;
    }
    delay(20);  // Short wait between commands
  }
  
  ESP_LOGI(TAG, "Radar configuration complete");
  return true;
}

bool IWR6843Component::send_config_line_(const char *line) {
  ESP_LOGD(TAG, "TX: %s", line);
  
  // Send line via UART
  this->uart_->write_str(line);
  this->uart_->write_byte('\n');
  this->uart_->flush();
  
  // Wait for response with watchdog feeding
  uint32_t timeout = millis() + 300;
  std::string response = "";
  
  while (millis() < timeout) {
    // Feed watchdog every iteration
    App.feed_wdt();
    
    if (this->uart_->available()) {
      uint8_t c;
      this->uart_->read_byte(&c);
      if (c == '\n') {
        if (response.length() > 0) {
          ESP_LOGV(TAG, "RX: %s", response.c_str());
          // Check for "Done" response
          if (response.find("Done") != std::string::npos) {
            return true;
          }
          response = "";
        }
      } else if (c >= 32 && c < 127) {
        response += (char)c;
      }
    }
    delay(1);  // Small delay to prevent tight loop
  }
  
  return true;  // Continue even without explicit "Done"
}

bool IWR6843Component::read_frame_() {
  // Read header (enable/disable handle CS pin automatically)
  this->enable();
  this->read_array(this->buffer_, sizeof(IWR6843Header));
  this->disable();
  
  IWR6843Header *header = reinterpret_cast<IWR6843Header*>(this->buffer_);
  
  // Validate header
  if (!this->validate_header_(*header)) {
    return false;
  }
  
  // Read rest of packet
  uint32_t remaining = header->total_packet_len - sizeof(IWR6843Header);
  if (remaining > BUFFER_SIZE - sizeof(IWR6843Header)) {
    ESP_LOGE(TAG, "Packet too large: %d bytes", header->total_packet_len);
    return false;
  }
  
  this->enable();
  this->read_array(this->buffer_ + sizeof(IWR6843Header), remaining);
  this->disable();
  
  // Update frame number
  this->frame_number_ = header->frame_number;
  
  // Parse TLVs
  uint8_t *tlv_data = this->buffer_ + sizeof(IWR6843Header);
  uint32_t offset = 0;
  
  for (uint16_t i = 0; i < header->num_tlvs && offset < remaining; i++) {
    TLVHeader *tlv = reinterpret_cast<TLVHeader*>(tlv_data + offset);
    
    this->parse_tlv_(tlv_data + offset + sizeof(TLVHeader), tlv->length);
    
    offset += sizeof(TLVHeader) + tlv->length;
  }
  
  return true;
}

bool IWR6843Component::validate_header_(const IWR6843Header &header) {
  // Check magic word
  if (header.magic_word[0] != 0x0102 || 
      header.magic_word[1] != 0x0304 ||
      header.magic_word[2] != 0x0506 || 
      header.magic_word[3] != 0x0708) {
    ESP_LOGV(TAG, "Invalid magic word");
    return false;
  }
  
  // Check platform
  if (header.platform != 0xA6843) {
    ESP_LOGW(TAG, "Unexpected platform: 0x%X", header.platform);
  }
  
  return true;
}

void IWR6843Component::parse_tlv_(const uint8_t *data, uint32_t length) {
  const TLVHeader *tlv = reinterpret_cast<const TLVHeader*>(data - sizeof(TLVHeader));
  
  switch (tlv->type) {
    case MMWDEMO_OUTPUT_MSG_POINT_CLOUD:
      this->parse_point_cloud_(data, length);
      break;
      
    case MMWDEMO_OUTPUT_MSG_TRACKERPROC_3D_TARGET_LIST:
      this->parse_target_list_(data, length);
      break;
      
    case MMWDEMO_OUTPUT_MSG_TRACKERPROC_TARGET_HEIGHT:
      this->parse_target_height_(data, length);
      break;
      
    case MMWDEMO_OUTPUT_MSG_PRESCENCE_INDICATION:
      this->parse_presence_(data, length);
      break;
      
    default:
      ESP_LOGV(TAG, "Unknown TLV type: %d", tlv->type);
      break;
  }
}

void IWR6843Component::parse_point_cloud_(const uint8_t *data, uint32_t length) {
  // Point cloud parsing (if needed)
  ESP_LOGV(TAG, "Point cloud: %d bytes", length);
}

void IWR6843Component::parse_target_list_(const uint8_t *data, uint32_t length) {
  uint8_t num_targets = length / sizeof(Target);
  
  if (num_targets > this->max_tracks_) {
    num_targets = this->max_tracks_;
  }
  
  this->num_targets_ = num_targets;
  this->targets_.clear();
  
  const Target *targets = reinterpret_cast<const Target*>(data);
  
  for (uint8_t i = 0; i < num_targets; i++) {
    this->targets_.push_back(targets[i]);
    
    ESP_LOGD(TAG, "Target %d (TID %d): pos(%.2f, %.2f, %.2f) vel(%.2f, %.2f, %.2f)",
             i, targets[i].tid,
             targets[i].pos_x, targets[i].pos_y, targets[i].pos_z,
             targets[i].vel_x, targets[i].vel_y, targets[i].vel_z);
    
    // Trigger callback
    this->target_callbacks_.call(targets[i]);
  }
}

void IWR6843Component::parse_target_height_(const uint8_t *data, uint32_t length) {
  uint8_t num_heights = length / sizeof(TargetHeight);
  
  this->target_heights_.clear();
  const TargetHeight *heights = reinterpret_cast<const TargetHeight*>(data);
  
  for (uint8_t i = 0; i < num_heights; i++) {
    this->target_heights_.push_back(heights[i]);
    ESP_LOGV(TAG, "Target %d height: %.2f cm", heights[i].tid, heights[i].height);
  }
}

void IWR6843Component::parse_presence_(const uint8_t *data, uint32_t length) {
  if (length >= 4) {
    uint32_t presence = *reinterpret_cast<const uint32_t*>(data);
    bool detected = presence > 0;
    
    if (this->presence_detected_ != detected) {
      this->presence_detected_ = detected;
      ESP_LOGI(TAG, "Presence: %s", detected ? "DETECTED" : "NONE");
      
      // Trigger callback
      this->presence_callbacks_.call(detected);
    }
  }
}

void IWR6843Component::set_track_sensor(uint8_t track_id, uint8_t sensor_type, sensor::Sensor *sensor) {
  this->track_sensors_[track_id][sensor_type] = sensor;
}

void IWR6843Component::update_track_sensors_() {
  // Update all registered track sensors
  for (auto &track_entry : this->track_sensors_) {
    uint8_t track_id = track_entry.first;
    auto &sensors = track_entry.second;
    
    // Get track data
    if (track_id < this->targets_.size()) {
      const Target &target = this->targets_[track_id];
      
      // Calculate distance
      float distance = sqrt(target.pos_x * target.pos_x + 
                           target.pos_y * target.pos_y + 
                           target.pos_z * target.pos_z);
      
      // Update each sensor type
      for (auto &sensor_entry : sensors) {
        uint8_t sensor_type = sensor_entry.first;
        sensor::Sensor *sens = sensor_entry.second;
        
        if (sens != nullptr) {
          switch (sensor_type) {
            case 0:  // X position
              sens->publish_state(target.pos_x);
              break;
            case 1:  // Y position
              sens->publish_state(target.pos_y);
              break;
            case 2:  // Z position
              sens->publish_state(target.pos_z);
              break;
            case 3:  // X velocity
              sens->publish_state(target.vel_x);
              break;
            case 4:  // Y velocity
              sens->publish_state(target.vel_y);
              break;
            case 5:  // Z velocity
              sens->publish_state(target.vel_z);
              break;
            case 6:  // Distance
              sens->publish_state(distance);
              break;
          }
        }
      }
    } else {
      // Track not present - publish NaN or 0
      for (auto &sensor_entry : sensors) {
        sensor::Sensor *sens = sensor_entry.second;
        if (sens != nullptr) {
          sens->publish_state(NAN);  // Not available
        }
      }
    }
  }
}

}  // namespace iwr6843
}  // namespace esphome

