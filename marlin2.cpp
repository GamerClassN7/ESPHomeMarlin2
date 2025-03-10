#include "marlin2.h"
#include "esphome/core/log.h"

namespace esphome {
    static const char *TAG = "marlin2";

    #ifdef USE_SENSOR
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
    #endif
    #ifdef USE_TEXT_SENSOR
        void Marlin2::add_text_sensor(const std::string& sName, text_sensor::TextSensor *sens) {
            text_sensors.push_back({sName, sens});
        }

        text_sensor::TextSensor* Marlin2::find_text_sensor(std::string key) {
            for (const auto& pair : text_sensors) {
                if (key == std::string(pair.first)) { // Convert char* to std::string for comparison
                    return pair.second;
                }
            }
            return nullptr; // Return nullptr if no match is found
        }
    #endif

    void Marlin2::setup() {
        MarlinOutput.reserve(256);
        MarlinOutput = "";

        MarlinResponseOutput.reserve(256);
        MarlinResponseOutput = "";

        MarlinTime.reserve(32);
        PrinterState.reserve(32);

        ESP_LOGD(TAG, "M155 S10");

        write_str("\r\n\r\nM155 S10\r\n");
        write_str("\r\n\r\nM117 ESP Home Connected!\r\n");
        flush();

        set_printer_state("IDLE");
    }

    void Marlin2::write(std::string gcode) {
        ESP_LOGD(TAG, "->GCODE: %s", gcode.c_str());
        write_str((std::string("\r\n\r\n") + gcode + std::string("\r\n")).c_str());
        flush();
    }

    void Marlin2::update() {
        while (available()) {
            char c = read();
            if( c == '\n' || c == '\r' ) {
                ESP_LOGD(TAG, "#%s#",MarlinOutput.c_str());    
                process_line();
            } else {
                MarlinOutput += c;
            }
        }

        if(millis() - millisProgress > 15000 && print_progress != 100)  {
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
                #ifdef USE_SENSOR
                    if (find_sensor("bed_temperature") != nullptr)
                        find_sensor("bed_temperature")->publish_state(bed_temperature);
                    
                    if (find_sensor("bed_set_temperature") != nullptr)
                        find_sensor("bed_set_temperature")->publish_state(bed_set_temperature);
                    
                    if (find_sensor("ext_temperature") != nullptr)
                        find_sensor("ext_temperature")->publish_state(ext_temperature);
                    
                    if (find_sensor("ext_set_temperature") != nullptr)
                        find_sensor("ext_set_temperature")->publish_state(ext_set_temperature);
                #endif
                #ifdef USE_TEXT_SENSOR
                    if(bed_set_temperature==0.0 && ext_set_temperature==0.0)  {
                        if(ext_temperature < 32.0 && bed_temperature < 32.0){ //TODO define constants for these
                            set_printer_state("IDLE");
                        }         
                        else if(ext_temperature < 150.0 && bed_temperature < 55.0){
                            set_printer_state("COOLING");
                        }
                    }
                    if(print_progress == 0.0 && (bed_set_temperature!=0.0 || ext_set_temperature!=0.0))  {
                        //print_time_offset = print_time save print time ofset to deduct from total value send to hass
                        set_printer_state("PREHEATING");
                    }
                #endif

                ESP_LOGD(TAG, "Bed Temperature=%.1f°C Ext Temperature=%.1f°C ", bed_temperature, ext_temperature);
            }

            //reset string for next line
            MarlinOutput="";
            return;
        }

        //Parse Progress of the print
        if(MarlinOutput.find("SD printing byte") == 0 ) {
            print_progress = process_progress_msg();
            #ifdef USE_SENSOR
                if (find_sensor("print_progress") != nullptr)
                    find_sensor("print_progress")->publish_state(print_progress);

                ESP_LOGD(TAG, "progress=%.1f", print_progress);
            #endif

            set_printer_state("PRINTING");
            //reset string for next line
            MarlinOutput="";
            return;
        }

        //Parse Printitme
        if(MarlinOutput.find("echo:Print time: ") == 0) {
            double current=0;
            double remaining=0;
            if (process_print_time_msg(&current, &remaining, print_progress) != 0)  {
                #ifdef USE_SENSOR    
                    if (find_sensor("print_time") != nullptr)
                        find_sensor("print_time")->publish_state(current);

                    if (find_sensor("print_time_remaining") != nullptr)
                        find_sensor("print_time_remaining")->publish_state(remaining);
                #endif

                ESP_LOGD(TAG, "time=%f remaining=%f", current, remaining);
            }

            //reset string for next line
            MarlinOutput="";
            return;
        }

       //Print From SD Card Started
        if(MarlinOutput.find("File selected") != std::string::npos) {                
            set_printer_state("PRINTING");

            //reset string for next line
            MarlinOutput="";
            return;
        }

        //Print Finished
        if(MarlinOutput.find("Done printing") != std::string::npos) {       
            print_progress = 100;
            #ifdef USE_SENSOR    
                if (find_sensor("print_progress") != nullptr)
                    find_sensor("print_progress")->publish_state(print_progress);

                if (find_sensor("print_time_remaining") != nullptr)
                    find_sensor("print_time_remaining")->publish_state(0);
            #endif
            #ifdef USE_TEXT_SENSOR         
                set_printer_state("FINISHED");
            #endif            

            //reset string for next line
            MarlinOutput="";
            return;
        }

        // //Print Paused
        // if(MarlinOutput.find("Printer halted") != std::string::npos) {                
        //     set_printer_state("PAUSED");

        //     //reset string for next line
        //     MarlinOutput="";
        //     return;
        // }

        // //Print Stoped              
        if(MarlinOutput.find("Print Aborted") != std::string::npos) {                
            set_printer_state("STOPPED");

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

        while(MarlinOutput.find(" ") != std::string::npos)
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
        
        return (((float) current * 100.0) / (float) total);
    }

    int Marlin2::process_print_time_msg(double* current, double* remaining, float progress){
        MarlinTime = MarlinOutput.substr(16);
        float d = 0, h = 0, m = 0, s = 0; 
        
        ESP_LOGD(TAG,MarlinTime.c_str());
        
        if (sscanf(MarlinTime.c_str() ,"%fd %fh %fm %fs", &d, &h, &m, &s)!=4)  {
            d=0;
            if (sscanf(MarlinTime.c_str() ,"%fh %fm %fs", &h, &m, &s)!=3)  {
                d=0; h=0;
                if (sscanf(MarlinTime.c_str() ,"%fm %fs", &m, &s)!=2)  {
                    d=0; h=0; m=0;
                    if (sscanf(MarlinTime.c_str() ,"%fs", &s)!=1)  {
                        return 0;
                    }
                }
            }
        }

        *current = round(((d)*24*60*60) + ((h)*60*60) + ((m)*60) + (s));

        if(progress != 0.0 && progress != 100.0) {
            *remaining = (((100 * *current) / round(progress)) - *current);
        }           
    
        return 1;
    }

    void Marlin2::set_printer_state(std::string status){
        #ifdef USE_TEXT_SENSOR         
            // if (!PrinterState.compare(status))
            //     return; 

            if (find_text_sensor("printer_state") != nullptr){
                find_text_sensor("printer_state")->publish_state(status);
            }
            
            ESP_LOGD(TAG, "Printer Status %s", status.c_str());
            // PrinterState = status;
        #endif
    }

    std::string Marlin2::to_dos_name(std::string filename) {
        std::string shortName;
        size_t dotPos = filename.find_last_of('.');

        // Extract name and extension
        std::string namePart = (dotPos != std::string::npos) ? filename.substr(0, dotPos) : filename;
        std::string extPart = (dotPos != std::string::npos) ? filename.substr(dotPos + 1) : "";

        // Truncate name to 6 characters + ~1
        if (namePart.length() > 6) {
            namePart = namePart.substr(0, 6) + "~1";
        }

        // Truncate extension to 3 characters
        if (extPart.length() > 3) {
            extPart = extPart.substr(0, 3);
        }

        // Construct 8.3 filename
        shortName = namePart;
        if (!extPart.empty()) {
            shortName += "." + extPart;
        }

        // Convert to uppercase (DOS filenames are case-insensitive, usually stored in uppercase)
        std::transform(shortName.begin(), shortName.end(), shortName.begin(), ::toupper);

        return shortName;
    }

}  // namespace esphome