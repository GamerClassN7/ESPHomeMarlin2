#pragma once

#include "marlin2.h"
#include "esphome/core/automation.h"

#include <vector>

namespace esphome {

template<typename... Ts> class Marlin2WriteGCodeAction : public Action<Ts...> {
    public:
        explicit Marlin2WriteGCodeAction(Marlin2 *marlin2) : marlin2_(marlin2) {}
        TEMPLATABLE_VALUE(std::string, gcode)

        void play(Ts... x) override {
            this->marlin2_->write_str("\r\n\r\nM117 Action Called!\r\n");;
        }
    
    
    protected:
        Marlin2 *marlin2_;
};

}  // namespace esphome