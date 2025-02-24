#ifndef CAN_COMM_HPP_
#define CAN_COMM_HPP_

#include "main.h"
#include "software_fifo.hpp"

namespace UserLib{

struct CanPacket{
    uint32_t packet_id;
    bool is_ext_id = false;
    bool is_remote_frame = false;
    size_t dlc;
    uint8_t main_data[8];
};

class CanComm{

private:
    CAN_HandleTypeDef *can;
    SoftwareFifo<CanPacket, 8> tx_soft_fifo;
    SoftwareFifo<CanPacket, 8> rx_soft_fifo;

    size_t rx_fifo_number;

public:
    CanComm(CAN_HandleTypeDef* _can, size_t _rx_fifo_number) : can(_can), rx_fifo_number(_rx_fifo_number) {
        // nothing to do
    }

    CAN_HandleTypeDef* get_handle(void){
        return can;
    }

    void start();
    void filter_set_std_list(uint32_t bank_number, uint16_t _id_1, uint16_t _id_2, uint16_t _id_3, uint16_t _id_4);

    size_t get_tx_busy_level();
    bool add_tx_fifo(CanPacket &packet);
    void tx();

    size_t get_rx_busy_level();
    void rx_interruption();
    bool rx(CanPacket &packet);



};

void CanComm::start(){
    HAL_CAN_Start(can);
    if(rx_fifo_number == 0) HAL_CAN_ActivateNotification(can, CAN_IT_RX_FIFO0_MSG_PENDING);
    if(rx_fifo_number == 1) HAL_CAN_ActivateNotification(can, CAN_IT_RX_FIFO1_MSG_PENDING);
    HAL_CAN_ActivateNotification(can, CAN_TX_MAILBOX0);
    HAL_CAN_ActivateNotification(can, CAN_TX_MAILBOX1);
    HAL_CAN_ActivateNotification(can, CAN_TX_MAILBOX2);
}

void CanComm::filter_set_std_list(uint32_t bank_number, uint16_t _id_1, uint16_t _id_2, uint16_t _id_3, uint16_t _id_4){
    CAN_FilterTypeDef filter;

    filter.FilterBank = bank_number;
    filter.FilterIdHigh = _id_1 << 5;
    filter.FilterIdLow = _id_2 << 5;
    filter.FilterMaskIdHigh = _id_3 << 5;
    filter.FilterMaskIdLow = _id_4 << 5;
    filter.FilterScale = CAN_FILTERSCALE_16BIT;
    filter.FilterMode = CAN_FILTERMODE_IDLIST;
    filter.SlaveStartFilterBank = 14;
    filter.FilterActivation = ENABLE;

    if(rx_fifo_number == 0) filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    if(rx_fifo_number == 1) filter.FilterFIFOAssignment = CAN_FILTER_FIFO1;

    HAL_CAN_ConfigFilter(can, &filter);
}

size_t CanComm::get_tx_busy_level(){
    return tx_soft_fifo.get_busy_level();
}

bool CanComm::add_tx_fifo(CanPacket &packet){
    bool is_ok;
    is_ok = tx_soft_fifo.input(packet);
    tx();
    return is_ok;
}

void CanComm::tx(){
    if(get_tx_busy_level() == 0){
        // nothing to do
    }
    else{
        CanPacket packet;
        CAN_TxHeaderTypeDef tx_header;
        uint32_t mailbox_number;

        tx_soft_fifo.output(packet);

        if(packet.is_ext_id == true){
            tx_header.IDE = CAN_ID_EXT;
            tx_header.ExtId = packet.packet_id;
            tx_header.StdId = 0;
        }
        else{
            tx_header.IDE = CAN_ID_STD;
            tx_header.ExtId = 0;
            tx_header.StdId = packet.packet_id;
        }
        tx_header.RTR = (packet.is_remote_frame) ? CAN_RTR_REMOTE : CAN_RTR_DATA;
        tx_header.DLC = packet.dlc;
        tx_header.TransmitGlobalTime = DISABLE;

        HAL_CAN_AddTxMessage(can, &tx_header, packet.main_data, &mailbox_number);
    }
}

size_t CanComm::get_rx_busy_level(){
    return rx_soft_fifo.get_busy_level();
}

void CanComm::rx_interruption(){
    CAN_RxHeaderTypeDef rx_header;
    CanPacket packet;

    uint32_t rx_fifo;

    if(rx_fifo_number == 0){
        rx_fifo = CAN_RX_FIFO0;
    }
    else if(rx_fifo_number == 1)
    {
        rx_fifo = CAN_RX_FIFO1;
    }
    else{
        // ??? こんなことは起こってはいけない
    }

    HAL_CAN_GetRxMessage(can, rx_fifo, &rx_header, packet.main_data);

    packet.dlc = rx_header.DLC;

    if(rx_header.IDE == CAN_ID_EXT){
        packet.is_ext_id = true;
        packet.packet_id = rx_header.ExtId;
    }
    else{
        packet.is_ext_id = false;
        packet.packet_id = rx_header.StdId;
    }

    if(rx_header.RTR == CAN_RTR_REMOTE){
        packet.is_remote_frame = true;
    }
    else{
        packet.is_remote_frame = false;
    }

    rx_soft_fifo.input(packet);
}

bool CanComm::rx(CanPacket &packet){
    return rx_soft_fifo.output(packet);
}





}

#endif /* CAN_COMM_HPP_ */