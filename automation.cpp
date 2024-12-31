#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/switch/switch.h"

namespace esphome {

template<typename... Ts> class SetTemperatureAction : public Action<Ts...> {
    public:
        explicit SetTemperatureAction(Marlin2 *a_marlin2) : switch_(a_marlin2) {}

    void play(Ts... x) override { this->switch_->turn_on(); }

    protected:
        Switch *switch_;
};
}  // namespace esphome