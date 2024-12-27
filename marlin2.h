#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {

class Marlin2 : public PollingComponent ,  public uart::UARTDevice {
    public:
        void setup() override;
        float get_setup_priority() const override { return setup_priority::LATE; }
        void update() override;
    
    protected:
        void process_line();
        int process_temp_msg(float* ext_temperature, float* ext_set_temperature, float* bed_temperature, float* bed_set_temperature);
        float process_progress_msg();
        int process_print_time_msg(int* d, int* h, int* m, unsigned long* current, unsigned long* remaining)
        std::string MarlinOutput;

        sensor::Sensor *bed_temperature_sensor;
        sensor::Sensor *ext_temperature_sensor;
        sensor::Sensor *print_progress_sensor;

    
    private:
        unsigned long millisProgress=0;
};

}  // namespace esphome