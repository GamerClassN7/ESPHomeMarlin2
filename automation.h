#pragma once

#include "marlin2.h"
#include "esphome/core/automation.h"

#include <vector>

namespace esphome {

template<typename... Ts> class Marlin2WriteAction : public Action<Ts...>, public Parented<Marlin2> {
    public:
        void set_data_template(std::function<std::vector<uint8_t>(Ts...)> func) {
            this->data_func_ = func;
            this->static_ = false;
        }
        void set_data_static(const std::vector<uint8_t> &data) {  
            this->data_static_ = data;
            this->static_ = true;
        }

        void play(Ts... x) override {
            if (this->static_) {
                this->parent_->write_array(this->data_static_);
            } else {
                auto val = this->data_func_(x...);
                this->parent_->write_array(val);
            }
        }

    protected:
        bool static_{false};
        std::function<std::vector<uint8_t>(Ts...)> data_func_{};
        std::vector<uint8_t> data_static_{};
};

}  // namespace esphome