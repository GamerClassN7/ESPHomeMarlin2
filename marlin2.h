#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
//#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {

class Marlin2 : public PollingComponent, /*public text_sensor::TextSensor,*/ public uart::UARTDevice {
    public:
        void setup() override;
        float get_setup_priority() const override { return setup_priority::LATE; }
        void update() override;
        void add_sensor(const std::string& sName, sensor::Sensor *sens);
        //void add_text_sensor(const std::string& sName, text_sensor::TextSensor *sens);

        sensor::Sensor* find_sensor(std::string key);
        // text_sensor::TextSensor* find_text_sensor(std::string key);
    
    protected:
        void process_line();
        int process_temp_msg(float* ext_temperature, float* ext_set_temperature, float* bed_temperature, float* bed_set_temperature);
        float process_progress_msg();
        int process_print_time_msg(float* d, float* h, float* m, float* s, unsigned long* current, unsigned long* remaining);
        
        std::string MarlinOutput;
        std::string MarlinTime;
        std::string PrinterState;
        
        std::vector<std::pair<std::string, sensor::Sensor *>> sensors;
        // std::vector<std::pair<std::string, text_sensor::TextSensor *>> text_sensors;
    
    private:
        unsigned long millisProgress=0;
};

}  // namespace esphome