#include "cpp_main.hpp"

#include "string.h"



void wrap_cpp_main_init(void){
    UserCode::cpp_main_init();
}
void wrap_cpp_main_while(void){
    UserCode::cpp_main_while();
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == UserCode::hardtimer_1kHz.get_handle()){
        UserCode::hardtimer_1kHz.handle_callback();
    }
}

void usb_cdc_rx_callback(const uint8_t *input, uint32_t size){
    //UserCode::master_usb.add_rx_fifo(input, size);
    for(size_t i = 0; i < size; i++){
        UserCode::myreceiver.add_one_byte(input[i]);
    }
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if(hcan == UserCode::robomas_can.get_handle()){
        UserCode::robomas_can.rx_interruption();
    }
}


void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan){
	if(hcan == UserCode::robomas_can.get_handle()){
        UserCode::robomas_can.tx();
    }
}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan){
	if(hcan == UserCode::robomas_can.get_handle()){
        UserCode::robomas_can.tx();
    }
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan){
	if(hcan == UserCode::robomas_can.get_handle()){
        UserCode::robomas_can.tx();
    }
}






namespace UserCode{

    bool rm_start_flag = false;
    bool rm_monitor_flag = false;
    bool rm_motor_enable[4] = {false};

    int16_t rm_last_target_speed[4];
    int16_t rm_last_speed[4];
    int16_t rm_last_diff[4];

    double rm_speed_gain[4][3];

    void timer_interruption_1kHz(void){

        if(myreceiver.get_fifo_busy_level() > 0){
            UsbPacket tmp_usbpacket;
            MsgPacket tmp_msgpacket;
            myreceiver.get_usb_packet(tmp_usbpacket);
            decode_usb_to_msg(tmp_usbpacket, tmp_msgpacket);
            
            switch (tmp_msgpacket.packet_type)
            {
            case PACKET_PC_RM : {
                if(tmp_msgpacket.board_id == 0x03){
                    switch (tmp_msgpacket.command_type)
                    {
                    case REQ_STAET : {
                        rm_start_flag = true;

                        // rm_motor_enable[0] = tmp_msgpacket.data[0] & 0b00000001;
                        // rm_motor_enable[1] = tmp_msgpacket.data[0] & 0b00000010;
                        // rm_motor_enable[2] = tmp_msgpacket.data[0] & 0b00000100;
                        // rm_motor_enable[3] = tmp_msgpacket.data[0] & 0b00001000;
                        rm_motor_enable[0] = true;
                        rm_motor_enable[1] = true;
                        rm_motor_enable[2] = true;
                        rm_motor_enable[3] = true;
                        
                        MsgPacket this_msg;
                        UsbPacket this_usb;

                        this_msg.packet_type = PACKET_RM_PC;
                        this_msg.board_id = 0x03;
                        this_msg.command_type = RES_STAET;
                        
                        encode_msg_to_usb(this_msg, this_usb);

                        master_usb.tx(this_usb.data, 19);

                        break;
                    }
                    case COMMAND_SPEED : {
                        rm_last_target_speed[tmp_msgpacket.data[0]] = static_cast<int16_t>((tmp_msgpacket.data[1] << 8) | tmp_msgpacket.data[2]);

                        break;
                    }
                    case MONITOR_ENABLE : {
                        rm_monitor_flag = true;

                        break;
                    }
                    
                    default:
                        break;
                    }
                }
                break;
            }
            
            default:
                break;
            }
        }
        
        while(robomas_can.get_rx_busy_level() > 0){
            UserLib::CanPacket canpacket;
            RobomasFeedback feedback;
            robomas_can.rx(canpacket);
            decode_can_to_robomas(canpacket, feedback);

            rm_last_speed[feedback.id] = feedback.speed;

        }

        if(rm_start_flag){
            RobomasCommand this_command;
            UserLib::CanPacket this_can;

            for(size_t i = 0; i < 4; i ++){
                if(rm_motor_enable[i]){
                    this_command.current[i] = static_cast<int16_t>(rm_pid[i].culculate_output(rm_last_target_speed[i] - rm_last_speed[i]));
                }
                else{
                    this_command.current[i] = 0;
                }
            }

            encode_robomas_to_can(this_command, this_can);
            robomas_can.add_tx_fifo(this_can);
        }

        if(rm_monitor_flag){
            static size_t count;
            count ++;
            if(count == 10){
                count = 0;
                for (size_t i = 0; i < 4; i ++){
                    MsgPacket this_msg;
                    UsbPacket this_usb;
                    this_msg.packet_type = PACKET_RM_PC;
                    this_msg.board_id = 0x03;
                    this_msg.command_type = FEEDBACK_SPEED;
                    this_msg.data[0] = i;
                    this_msg.data[1] = rm_last_speed[i] << 4;
                    this_msg.data[2] = rm_last_speed[i] & 0x00ff;

                    encode_msg_to_usb(this_msg, this_usb);

                    master_usb.tx(this_usb.data, 19);
                }
            }
        }
        

    }
    
    void cpp_main_init(void){

        myled.Change_State(UserLib::LEDState::ON);
        HAL_Delay(3000);
        myled.Change_State(UserLib::LEDState::OFF);
        HAL_Delay(3000);

        robomas_can.filter_set_std_list(0, 0x201, 0x202, 0x203, 0x204);
        robomas_can.start();

        hardtimer_1kHz.set_callback(timer_interruption_1kHz);
        hardtimer_1kHz.start();
        
    }
    
    void cpp_main_while(void){
/*
        static UserLib::CanPacket m2006_speed;

        while(robomas_can.get_rx_busy_level() > 0){
            robomas_can.rx(m2006_speed);
        }
        
        
        UserLib::CanPacket m2006_move;

        m2006_move.is_ext_id = false;
        m2006_move.is_remote_frame = false;
        m2006_move.packet_id = 0x200;
        m2006_move.dlc = 8;
        m2006_move.main_data[0] = 10;
        m2006_move.main_data[1] = 0;
        m2006_move.main_data[2] = 10;
        m2006_move.main_data[3] = 0;
        m2006_move.main_data[4] = 10;
        m2006_move.main_data[5] = 0;
        m2006_move.main_data[6] = 10;
        m2006_move.main_data[7] = 0;

        robomas_can.add_tx_fifo(m2006_move);

        stlink.transmit_debug("now speed is %d ;\r\n", m2006_speed.main_data[1]);



        HAL_Delay(1);
*/
        stlink.transmit_debug("start_flag %d  monitor_flag %d  target_speed %d speed %d\r\n", rm_start_flag, rm_monitor_flag, rm_last_target_speed[0], rm_last_speed[1]);
        
    }
    

}


/*
まずやるべきこと
pidというかロボマス制御器の設計

*/




/*
書き換えた場所
CMakelists.txt
main.h
main.c
usbd_cdc_if.c
usbd_desc.c
*/