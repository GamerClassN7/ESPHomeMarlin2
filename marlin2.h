#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

namespace esphome {

class Marlin2 : public PollingComponent, public uart::UARTDevice {
    public:
        Marlin2() = default;


        #ifdef USE_SENSOR
            void add_sensor(const std::string& sName, sensor::Sensor *sens);
            sensor::Sensor* find_sensor(std::string key);
        #endif
        #ifdef USE_TEXT_SENSOR
            void add_text_sensor(const std::string& sName, text_sensor::TextSensor *tSens);
            text_sensor::TextSensor* find_text_sensor(std::string key);
        #endif

        float get_setup_priority() const override { return setup_priority::LATE; }
        void setup() override;
        void update() override;

    protected:
        std::string MarlinOutput;
        std::string MarlinTime;
        std::string PrinterState;

        float print_progress = 0;
        double print_time_offset = 0;

        
        #ifdef USE_SENSOR
            std::vector<std::pair<std::string, sensor::Sensor *>> sensors;
        #endif
        #ifdef USE_TEXT_SENSOR
            std::vector<std::pair<std::string, text_sensor::TextSensor *>> text_sensors;
        #endif

        void process_line();
        void set_printer_state(std::string status);
        int process_temp_msg(float* ext_temperature, float* ext_set_temperature, float* bed_temperature, float* bed_set_temperature);
        float process_progress_msg();
        int process_print_time_msg(double* current, double* remaining, float progress);
    
    private:
        unsigned long millisProgress=0;
};

}  // namespace esphome