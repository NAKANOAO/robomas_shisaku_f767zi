#ifndef USB_RECEIVER_HPP_
#define USB_RECEIVER_HPP_

#include "main.h"
#include "data_conversion.hpp"
#include "usb_lib.hpp"
#include "software_fifo.hpp"


namespace UserCode{

class UsbReceiver{
private:
    UserLib::SoftwareFifo<UsbPacket, 8> rx_fifo;
    uint8_t preserved_data[64];
    size_t packet_tail = 0;

public:
    bool add_one_byte(const uint8_t &input_data){

        if(input_data == '\0'){
            preserved_data[packet_tail] = '\0';
            packet_tail ++;
            if(packet_tail == 19){
                UsbPacket this_packet;
                for(size_t i = 0; i < 19; i++){
                    this_packet.data[i] = preserved_data[i];
                }
                rx_fifo.input(this_packet);
                for(size_t i = 0; i < sizeof(preserved_data); i ++){
                    preserved_data[i] = 0;
                }
                packet_tail = 0;
            }
            else{
                for(size_t i = 0; i < sizeof(preserved_data); i ++){
                    preserved_data[i] = 0;
                }
                packet_tail = 0;
                return false;
            }
        }
        else{
            preserved_data[packet_tail] = input_data;
            packet_tail ++;
        }

        return true;
    }

    size_t get_fifo_busy_level(){
        return rx_fifo.get_busy_level();
    }

    bool get_usb_packet(UsbPacket &user_buf){
        return rx_fifo.output(user_buf);
    }

    
};



}

#endif /* USB_RECEIVER_HPP_ */