#ifndef USB_COMM_HPP_
#define USB_COMM_HPP_

#include "main.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "software_fifo.hpp"


namespace UserLib{


template<size_t buf_size>
class UsbComm{
private:
    SoftwareFifo<uint8_t, buf_size> rx_fifo;

public:
    void tx(uint8_t* tx_data, uint16_t data_len){
        CDC_Transmit_FS(tx_data, data_len);
    }

    size_t get_rx_busy_level(){
        return rx_fifo.get_busy_level();
    }

    bool add_rx_fifo(const uint8_t* rx_data, uint32_t len){
        bool is_success = false;
        for (uint32_t i = 0; i < len; i++){
            is_success = rx_fifo.input(rx_data[i]);
        }
        return is_success;
    }

    bool rx(uint8_t &rx_buf){
        return rx_fifo.output(rx_buf);
    }

};

}


#endif /* USB_COMM_HPP_ */