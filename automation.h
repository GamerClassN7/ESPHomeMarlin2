#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "marlin2.h"

namespace esphome {
    static const char *TAG = "marlin2";

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


    template<typename... Ts> class PrintFileAction : public Action<Ts...> {
        public:
            explicit PrintFileAction(Marlin2 *marlin2) : marlin2_(marlin2) {}
            TEMPLATABLE_VALUE(std::string, value)

            void play(Ts... x) override {
                std::string file_name = this->marlin2_->to_dos_name(this->value_.value(x...));
                ESP_LOGD(TAG, "->FILE: %s", file_name.c_str());
                this->marlin2_->write(str_sprintf("M32 P !%s#", file_name.c_str()));
            }

        protected:
            Marlin2 *marlin2_;
    };
}  // namespace esphome