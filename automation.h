#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "marlin2.h"

namespace esphome {

template<typename... Ts> class Marlin2WriteAction : public Action<Ts...> {
    public:
        explicit Marlin2WriteAction(Marlin2 *marlin2) : marlin2_(marlin2) {}
        TEMPLATABLE_VALUE(std::string, value)

        void play(Ts... x) override {
            this->marlin2_->write("M117 Action Called!");
        }

    protected:
        Marlin2 *marlin2_;
};

}  // namespace esphome