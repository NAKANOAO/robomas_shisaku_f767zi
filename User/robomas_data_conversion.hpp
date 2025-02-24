#ifndef ROBOMAS_DATA_CONVERSION_HPP_
#define ROBOMAS_DATA_CONVERSION_HPP_

#include "main.h"
#include "can_lib.hpp"

namespace UserCode{


struct RobomasCommand{
    int16_t current[4];
};

struct RobomasFeedback{
    size_t id;
    uint16_t angle;
    int16_t speed;
    int16_t current;
};

void decode_can_to_robomas(UserLib::CanPacket can, RobomasFeedback &feedback){

    feedback.id = can.packet_id - 0x200;

    uint16_t uint_speed = (can.main_data[2] << 4) & can.main_data[3];
    uint16_t uint_current = (can.main_data[4] << 4) & can.main_data[5];

    feedback.angle = (can.main_data[0] << 4) & can.main_data[1];
    feedback.speed = static_cast<int16_t>(uint_speed);
    feedback.current = static_cast<int16_t>(uint_current);

}

void encode_robomas_to_can(RobomasCommand command, UserLib::CanPacket &can){

    can.is_ext_id = false;
    can.is_remote_frame = false;
    can.dlc = 8;
    can.packet_id = 0x200;

    uint16_t uint_current[4];
    uint_current[0] = static_cast<uint16_t>(command.current[0]);
    uint_current[1] = static_cast<uint16_t>(command.current[1]);
    uint_current[2] = static_cast<uint16_t>(command.current[2]);
    uint_current[3] = static_cast<uint16_t>(command.current[3]);

    can.main_data[0] = uint_current[0] >> 4;
    can.main_data[1] = uint_current[0] & 0x00ff;
    can.main_data[2] = uint_current[1] >> 4;
    can.main_data[3] = uint_current[1] & 0x00ff;
    can.main_data[4] = uint_current[2] >> 4;
    can.main_data[5] = uint_current[2] & 0x00ff;
    can.main_data[6] = uint_current[3] >> 4;
    can.main_data[7] = uint_current[3] & 0x00ff;

}

}


#endif /* ROBOMAS_DATA_CONVERSION_HPP_ */