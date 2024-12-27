#include "marlin2.h"
#include "esphome/core/log.h"

namespace esphome {
    static const char *TAG = "marlin2";

    void Marlin2::add_sensor(const std::string& sName, sensor::Sensor *sens) {
        sensors.push_back({sName, sens});
    }

    sensor::Sensor* Marlin2::find_sensor(std::string key) {
        for (const auto& pair : sensors) {
            if (key == std::string(pair.first)) { // Convert char* to std::string for comparison
                return pair.second;
            }
        }
        return nullptr; // Return nullptr if no match is found
    }

    // void Marlin2::add_text_sensor(const std::string& sName, text_sensor *sens) {
    //     text_sensors.push_back({sName, sens});
    // }

    // text_sensor* Marlin2::find_text_sensor(std::string key) {
    //     for (const auto& pair : text_sensors) {
    //         if (key == std::string(pair.first)) { // Convert char* to std::string for comparison
    //             return pair.second;
    //         }
    //     }
    //     return nullptr; // Return nullptr if no match is found
    // }

    void Marlin2::setup() {
        MarlinOutput.reserve(256);
        MarlinOutput = "";

        MarlinTime.reserve(32);
        PrinterState.reserve(32);

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
            float ext_temperature, ext_set_temperature, bed_temperature, bed_set_temperature;
            if (process_temp_msg(&ext_temperature, &ext_set_temperature, &bed_temperature, &bed_set_temperature) != 0)  {
                
                if (find_sensor("bed_temperature") != nullptr)
                    find_sensor("bed_temperature")->publish_state(bed_temperature);
                
                if (find_sensor("bed_set_temperature") != nullptr)
                    find_sensor("bed_set_temperature")->publish_state(bed_set_temperature);
                
                if (find_sensor("ext_temperature") != nullptr)
                    find_sensor("ext_temperature")->publish_state(ext_temperature);
                
                if (find_sensor("ext_set_temperature") != nullptr)
                    find_sensor("ext_set_temperature")->publish_state(ext_set_temperature);

                ESP_LOGD(TAG, "Bed Temperature=%.1f°C Ext Temperature=%.1f°C ", bed_temperature, ext_temperature);
            }

            // if(bed_set_temperature==0.0 && ext_set_temperature==0.0)  {
            //         if(ext_temperature < 32.0 && bed_temperature < 32.0)         //TODO define constants for these
            //             //if (find_text_sensor("printer_status") != nullptr)
            //                 //find_text_sensor("printer_status")->publish_state("IDLE");
            //         else if(ext_temperature < 150.0 && bed_temperature < 55.0)
            //             //if (find_text_sensor("printer_status") != nullptr)
            //                 //find_text_sensor("printer_status")->publish_state("COOLING");
            //     }
            //     if(bed_set_temperature!=0.0 || ext_set_temperature!=0.0)  {
            //         //if (find_text_sensor("printer_status") != nullptr)
            //             //find_text_sensor("printer_status")->publish_state("PREHEATING");
            //     }
        

            //reset string for next line
            MarlinOutput="";
            return;
        }

        //Parse Progress of the print
        if(MarlinOutput.find("SD printing byte") == 0 ) {
            float print_progress = process_progress_msg();

            if (find_sensor("print_progress") != nullptr)
                find_sensor("print_progress")->publish_state(print_progress);

            ESP_LOGD(TAG, "progress=%.1f", print_progress);

            //reset string for next line
            MarlinOutput="";
            return;
        }

        //Parse Printitme
        if(MarlinOutput.find("echo:Print time: ") == 0) {
            int* d=0, h=0, m=0, s=0;
            unsigned long current=0, remaining=0;

            if (process_print_time_msg(&d, &h, &m, &s, &current, &remaining) != 0)  {
                if (find_sensor("print_time") != nullptr)
                    find_sensor("print_time")->publish_state(current);

                if (find_sensor("print_time_remaining") != nullptr)
                    find_sensor("print_time_remaining")->publish_state(remaining);

                ESP_LOGD(TAG, "time=%lu remaining=%lu", current, remaining);
            }

            //reset string for next line
            MarlinOutput="";
            return;
        }

        //Print Finished
        if(MarlinOutput.find("Done printing") != std::string::npos) {                
            //if (find_text_sensor("printer_status") != nullptr)
                //find_text_sensor("printer_status")->publish_state("FINISHED");

            ESP_LOGD(TAG, "Print Finished");

            //reset string for next line
            MarlinOutput="";
            return;
        }

        //Print Paused
        if(MarlinOutput.find("Printer halted") != std::string::npos) {                
            //if (find_text_sensor("printer_status") != nullptr)
                //find_text_sensor("printer_status")->publish_state("PAUSED");

            ESP_LOGD(TAG, "Print Finished");

            //reset string for next line
            MarlinOutput="";
            return;
        }

        //Print Stoped
        if(MarlinOutput.find("Print Aborted") != std::string::npos) {                
            //if (find_text_sensor("printer_status") != nullptr)
                //find_text_sensor("printer_status")->publish_state("ABOARTED");

            ESP_LOGD(TAG, "Print Finished");

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

    int Marlin2::process_print_time_msg(int d, int h, int m, int s, unsigned long* current, unsigned long* remaining){
        MarlinTime = MarlinOutput.substr(16);
            
        ESP_LOGD(TAG,MarlinTime.c_str());
        
        if (sscanf(MarlinTime.c_str() ,"%dd %dh %dm %ds", &d, &h, &m, &s)!=4)  {
            d=0;
            if (sscanf(MarlinTime.c_str() ,"%dh %dm %ds", &h, &m, &s)!=3)  {
                d=0; h=0;
                if (sscanf(MarlinTime.c_str() ,"%dm %ds", &m, &s)!=2)  {
                    d=0; h=0; m=0;
                    if (sscanf(MarlinTime.c_str() ,"%ds", &s)!=1)  {
                        MarlinOutput="";
                        return;
                    }
                }
            }
        }
        
        current = static_cast<float>((d*24*60*60) + (h*60*60) + (m*60) + (s));
            

        return 0;
    }

}  // namespace esphome