#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/uart/uart.h"
#include <vector>

namespace esphome {
namespace iwr6843 {

// TLV Types from IWR6843
#define MMWDEMO_OUTPUT_MSG_POINT_CLOUD                1
#define MMWDEMO_OUTPUT_MSG_TRACKERPROC_3D_TARGET_LIST 1010
#define MMWDEMO_OUTPUT_MSG_TRACKERPROC_TARGET_INDEX   1011
#define MMWDEMO_OUTPUT_MSG_TRACKERPROC_TARGET_HEIGHT  1012
#define MMWDEMO_OUTPUT_MSG_PRESCENCE_INDICATION       1021

// IWR6843 Frame Header Structure
struct __attribute__((packed)) IWR6843Header {
  uint16_t magic_word[4];      // 0x0102, 0x0304, 0x0506, 0x0708
  uint32_t version;
  uint32_t platform;           // 0xA6843
  uint32_t timestamp;
  uint32_t total_packet_len;
  uint32_t frame_number;
  uint32_t subframe_number;
  uint32_t chirp_margin;
  uint32_t frame_margin;
  uint32_t track_process_time;
  uint16_t num_tlvs;
  uint16_t checksum;
};

// TLV Header Structure
struct __attribute__((packed)) TLVHeader {
  uint32_t type;
  uint32_t length;
};

// Target Structure (from gtrack)
struct __attribute__((packed)) Target {
  uint32_t tid;
  float pos_x;
  float pos_y;
  float pos_z;
  float vel_x;
  float vel_y;
  float vel_z;
  float acc_x;
  float acc_y;
  float acc_z;
  float ec[16];  // Error covariance matrix
  float g;       // Gating gain
};

// Target Height Structure
struct __attribute__((packed)) TargetHeight {
  uint32_t tid;
  float height;
};

class IWR6843Component : public Component, 
                         public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, 
                                               spi::CLOCK_POLARITY_LOW, 
                                               spi::CLOCK_PHASE_LEADING, 
                                               spi::DATA_RATE_1MHZ> {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Configuration setters
  void set_uart(uart::UARTComponent *uart) { this->uart_ = uart; }
  void set_cs_pin(GPIOPin *pin) { this->cs_pin_ = pin; }
  void set_sop2_pin(GPIOPin *pin) { this->sop2_pin_ = pin; }
  void set_nrst_pin(GPIOPin *pin) { this->nrst_pin_ = pin; }
  void set_ceiling_height(uint16_t height) { this->ceiling_height_ = height; }
  void set_max_tracks(uint8_t max_tracks) { this->max_tracks_ = max_tracks; }
  void set_update_interval(uint32_t interval) { this->update_interval_ = interval; }

  // Data access
  bool has_presence() const { return this->presence_detected_; }
  uint8_t get_num_targets() const { return this->num_targets_; }
  const std::vector<Target>& get_targets() const { return this->targets_; }
  uint32_t get_frame_number() const { return this->frame_number_; }

  // Callbacks
  void add_on_presence_callback(std::function<void(bool)> &&callback) {
    this->presence_callbacks_.add(std::move(callback));
  }
  void add_on_target_callback(std::function<void(const Target&)> &&callback) {
    this->target_callbacks_.add(std::move(callback));
  }

 protected:
  // Hardware control
  bool reset_radar_();
  bool configure_radar_();
  bool send_config_line_(const char *line);
  
  // SPI data reading
  bool read_frame_();
  bool validate_header_(const IWR6843Header &header);
  void parse_tlv_(const uint8_t *data, uint32_t length);
  void parse_point_cloud_(const uint8_t *data, uint32_t length);
  void parse_target_list_(const uint8_t *data, uint32_t length);
  void parse_target_height_(const uint8_t *data, uint32_t length);
  void parse_presence_(const uint8_t *data, uint32_t length);

  // Configuration
  uart::UARTComponent *uart_{nullptr};
  GPIOPin *cs_pin_{nullptr};
  GPIOPin *sop2_pin_{nullptr};
  GPIOPin *nrst_pin_{nullptr};
  
  uint16_t ceiling_height_{250};
  uint8_t max_tracks_{5};
  uint32_t update_interval_{50};  // ms
  
  // State
  bool configured_{false};
  bool presence_detected_{false};
  uint8_t num_targets_{0};
  uint32_t frame_number_{0};
  uint32_t last_read_time_{0};
  
  std::vector<Target> targets_;
  std::vector<TargetHeight> target_heights_;
  
  // Data buffer
  static const size_t BUFFER_SIZE = 4096;
  uint8_t buffer_[BUFFER_SIZE];
  
  // Callbacks
  CallbackManager<void(bool)> presence_callbacks_;
  CallbackManager<void(const Target&)> target_callbacks_;
};

}  // namespace iwr6843
}  // namespace esphome

