#include "marlin2.h"
#include "esphome/core/log.h"

namespace esphome {

static const char *TAG = "marlin2";

void Marlin2::setup() {
    this->write_str("\r\n\r\nM155 S10\r\n");
    ESP_LOGV(TAG, "M155 S10");
}

void Marlin2::loop() {
    while (this->available()) {
        uint8_t c;
        this->read_byte(&c);

        if (c == '\r')
            continue;

        if (c == '\n')
            this->parse_values_();
        else
            this->rx_message_.push_back(c);
    }
}

void Marlin2::parse_values_() {
    std::string s(this->rx_message_.begin(), this->rx_message_.end());
    ESP_LOGV(TAG, s);
}

void Marlin2::dump_config() {
    ESP_LOGCONFIG(TAG, "Serial CSV Reader");
}

}  // namespace esphome