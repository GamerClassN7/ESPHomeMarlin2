#include "marlin2.h"
#include "esphome/core/log.h"

namespace esphome {
namespace serial {

static const char *TAG = "serial.marlin2";

void Marlin2::loop() {
    // while (this->available()) {
    //     uint8_t c;
    //     this->read_byte(&c);

    //     if (c == '\r')
    //         continue;

    //     if (c == '\n')
    //         this->parse_values_();
    //     else
    //         this->rx_message_.push_back(c);
    // }
}

// void Marlin2::parse_values_() {
//     std::string s(this->rx_message_.begin(), this->rx_message_.end());
//     ESP_LOGV(TAG, s);
// }

// void Marlin2::dump_config() {
//     ESP_LOGCONFIG("", "Serial CSV Reader");
//     for (auto sens : this->sensors_) {
//         ESP_LOGCONFIG(TAG, "Index %d", sens.first);
//         LOG_SENSOR(TAG, "", sens.second);
//     }
// }

}  // namespace serial
}  // namespace esphome