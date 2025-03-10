#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "marlin2.h"

namespace esphome {

template<typename... Ts> class WriteAction : public Action<Ts...> {
    public:
        explicit WriteAction(Marlin2 *marlin2) : marlin2_(marlin2) {}
        TEMPLATABLE_VALUE(std::string, value)

        void play(Ts... x) override {
            this->marlin2_->write(this->value_.value(x...));
        }

    protected:
        Marlin2 *marlin2_;
};

}  // namespace esphome