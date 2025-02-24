#ifndef DATA_CONVERSION_HPP_
#define DATA_CONVERSION_HPP_

#include "main.h"


namespace UserCode{

struct MsgPacket{
    uint8_t packet_type;
    uint8_t board_id;
    uint8_t command_type;
    uint8_t data[7];
};

struct UsbPacket{
    uint8_t data[19];
};

constexpr size_t USB_PACKET_SIZE = 19;
constexpr uint8_t INVALID_HAX = 0xfe;
constexpr uint8_t INVALID_ASCII = 'x';
constexpr uint8_t END_MARKER = 0xff;

uint8_t ascii_to_hax(uint8_t input){
    if(input == '\0') return END_MARKER;
    if('0' <= input && input <= '9') return input - '0';
    if('a' <= input && input <= 'f') return input - 'a' + 10;
    return INVALID_HAX;
}

uint8_t hax_to_ascii(uint8_t input){
    if(input == END_MARKER) return '\0';
    // if(0<= input && input <= 9) return '0' + input;
    if(input <= 9) return '0' + input;
    if(0x0a <= input && input <= 0x0f) return 'a' + (input - 10);
    return INVALID_ASCII;
}

bool decode_usb_to_msg(UsbPacket input_data, MsgPacket &output_data){
    bool decode_success = true;
    uint8_t hax[USB_PACKET_SIZE];

    for(size_t head = 0; head < USB_PACKET_SIZE; head ++){
        hax[head] = ascii_to_hax(input_data.data[head]);
    }

    for(size_t head = 0; head < (USB_PACKET_SIZE - 1); head ++){
        if(hax[head] > 0x0f){
            decode_success = false;
        }
    }
    if(hax[18] != 0xff){
        decode_success = false;
    }

    output_data.packet_type = hax[0];
    output_data.board_id = hax[1];
    output_data.command_type = ((hax[2] & 0x0f) << 4) | (hax[3] & 0x0f);
    output_data.data[0] = ((hax[4] & 0x0f) << 4) | (hax[5] & 0x0f);
    output_data.data[1] = ((hax[6] & 0x0f) << 4) | (hax[7] & 0x0f);
    output_data.data[2] = ((hax[8] & 0x0f) << 4) | (hax[9] & 0x0f);
    output_data.data[3] = ((hax[10] & 0x0f) << 4) | (hax[11] & 0x0f);
    output_data.data[4] = ((hax[12] & 0x0f) << 4) | (hax[13] & 0x0f);
    output_data.data[5] = ((hax[14] & 0x0f) << 4) | (hax[15] & 0x0f);
    output_data.data[6] = ((hax[16] & 0x0f) << 4) | (hax[17] & 0x0f);

    return decode_success;

}

void encode_msg_to_usb(MsgPacket input_data, UsbPacket &output_data){
    uint8_t usb_packet_hax[USB_PACKET_SIZE];

    usb_packet_hax[0] = input_data.packet_type;
    usb_packet_hax[1] = input_data.board_id;
    usb_packet_hax[2] = input_data.command_type >> 4;
    usb_packet_hax[3] = input_data.command_type & 0x0f;
    usb_packet_hax[4] = input_data.data[0] >> 4;
    usb_packet_hax[5] = input_data.data[0] & 0x0f;
    usb_packet_hax[6] = input_data.data[1] >> 4;
    usb_packet_hax[7] = input_data.data[1] & 0x0f;
    usb_packet_hax[8] = input_data.data[2] >> 4;
    usb_packet_hax[9] = input_data.data[2] & 0x0f;
    usb_packet_hax[10] = input_data.data[3] >> 4;
    usb_packet_hax[11] = input_data.data[3] & 0x0f;
    usb_packet_hax[12] = input_data.data[4] >> 4;
    usb_packet_hax[13] = input_data.data[4] & 0x0f;
    usb_packet_hax[14] = input_data.data[5] >> 4;
    usb_packet_hax[15] = input_data.data[5] & 0x0f;
    usb_packet_hax[16] = input_data.data[6] >> 4;
    usb_packet_hax[17] = input_data.data[6] & 0x0f;
    usb_packet_hax[18] = END_MARKER;

    for(size_t head = 0; head < USB_PACKET_SIZE; head ++){
        output_data.data[head] = hax_to_ascii(usb_packet_hax[head]);
    }

}

}

#endif /* DATA_CONVERSION_HPP*/