#include "marlin2.h"
#include "esphome/core/log.h"

namespace esphome {

static const char *TAG = "marlin2";

    void Marlin2::setup() {
        MarlinOutput.reserve(256);
        MarlinOutput = "";

        ESP_LOGD(TAG, "M155 S10");

        write_str("\r\n\r\nM155 S10\r\n");
        write_str("\r\n\r\nM117 Hello World!\r\n");
        flush();
    }

    void  Marlin2::update() {
        while (available()) {
            char c = read();
            if( c == '\n' || c == '\r' ) {
                process_line();
            } else {
                MarlinOutput += c;
            }
        }

        if(millis() - millisProgress > 15000 )  {
            millisProgress = millis();

            ESP_LOGD(TAG, "M27");
            ESP_LOGD(TAG, "M31");

            write_str("M27\r\nM31\r\n");
        }
    }

    void  Marlin2::process_line() {
        if(MarlinOutput.size() < 3) {
            MarlinOutput="";
            return;
        }

        if(!MarlinOutput.compare("ok") || !MarlinOutput.compare(" ok")) {
            MarlinOutput="";
            return;
        }

        //Parse periodic Temperature read out message
        if(
            MarlinOutput.find(" T:")    == 0 ||
            MarlinOutput.find("T:")     == 0 ||
            MarlinOutput.find("ok T:")  == 0 ||
            MarlinOutput.find(" ok T:") == 0
        ) {
            float ext_temperature, ext_set_temperature, bed_temperature, bed_set_tempertaure;
            if (process_temp_msg(&ext_temperature, &ext_set_temperature, &bed_temperature, &bed_set_tempertaure) != 0)  {
                //bed_temperature_sensor->publish_state(bed_temperature);
                // bed_temperature->publish_state(bed_set_tempertaure);
                //ext_temperature_sensor->publish_state(ext_temperature);
                // ext_temperature->publish_state(ext_set_temperature);
                ESP_LOGD(TAG, "Bed Temperature=%.1f°C Ext Temperature=%.1f°C ", bed_temperature, ext_temperature);
            }

            //reset string for next line
            MarlinOutput="";
            return;
        }

        //Parse Progress of the print
        if(MarlinOutput.find("SD printing byte") == 0 ) {
            float progress = process_progress_msg();
            //print_progress_sensor->publish_state(progress);
            ESP_LOGD(TAG, "progress=%.1f", progress);

            //reset string for next line
            MarlinOutput="";
            return;
        }

        //Parse Printitme
        if(MarlinOutput.find("echo:Print time: ") == 0) {
            int d=0, h=0, m=0, s=0;
            unsigned long current=0, remaining=0;

            if (process_print_time_msg(&d, &h, &m, &current, &remaining) != 0)  {
            }

            //reset string for next line
            MarlinOutput="";
            return;
        }

        ESP_LOGD(TAG, "#%s#",MarlinOutput.c_str());        
        MarlinOutput="";
        return;
    }

    int Marlin2::process_temp_msg(float* ext_temperature, float* ext_set_temperature, float* bed_temperature, float* bed_set_temperature) {
        float dc;

        while(MarlinOutput.find(' ') != std::string::npos)
            MarlinOutput.erase(MarlinOutput.find(' '), 1);

        while(MarlinOutput.find("ok") != std::string::npos)
            MarlinOutput.erase(MarlinOutput.find("ok"), 2);
        
        if(sscanf(MarlinOutput.c_str() ,"T:%f/%fB:%f/%f", ext_temperature, ext_set_temperature, bed_temperature, bed_set_temperature) == 4 )
            return 1;

        if(sscanf(MarlinOutput.c_str() ,"T:%f/%f(%f)B:%f/%f(%f)", ext_temperature, ext_set_temperature, &dc, bed_temperature, bed_set_temperature, &dc) == 6 )
            return 2;

        if(sscanf(MarlinOutput.c_str() ,"T:%f/%fT0:%f/%fT1:%f/%fB:%f/%f", ext_temperature, ext_set_temperature, &dc, &dc, &dc, &dc, bed_temperature, bed_set_temperature) == 8 )
            return 3;  
            
        if(sscanf(MarlinOutput.c_str() ,"T0:%f/%fT1:%f/%fB:%f/%f", ext_temperature, ext_set_temperature, &dc, &dc, bed_temperature, bed_set_temperature) == 6 )
            return 4;                       

        return 0;
    }

    float Marlin2::process_progress_msg(){
        float current = std::stoi(MarlinOutput.substr(17));
        float total = std::stoi(MarlinOutput.substr(MarlinOutput.find('/')+1));
        
        if (total==0)  {
            return 0.0;
        }
        
        return ((float) current / (float) total) * 100.0;
    }

    int Marlin2::process_print_time_msg(int* d, int* h, int* m, unsigned long* current, unsigned long* remaining){
        return 0;
    }

}  // namespace esphome