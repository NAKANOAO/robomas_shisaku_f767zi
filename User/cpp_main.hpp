#ifndef CPP_MAIN_HPP_
#define CPP_MAIN_HPP_


#include "main.h"
#include "gpio.h"
#include "can.h"
#include "usart.h"
#include "tim.h"


#include "led_gpio.hpp"
#include "usb_lib.hpp"
#include "uart_lib.hpp"
#include "usb_receiver.hpp"
#include "can_lib.hpp"
#include "robomas_data_conversion.hpp"
#include "msg_def.hpp"
#include "timer_interruption.hpp"
#include "pid_control.hpp"
#include <array>

/*
・usbから目標値（rps）を受信する
・ロボマスを目標値にフィードバックする
・実際の速度値をusbに送信する
*/

namespace UserCode{
    
    UserLib::StLinkDebug stlink(&huart3, 100);
    UserLib::UsbComm<8> master_usb;
    UserLib::LedControl myled(LED_R_PORT, LED_R_PIN);
    UserLib::CanComm robomas_can(&hcan1, 0);
    UserLib::HardTimer hardtimer_1kHz(&htim7);
    UsbReceiver myreceiver;

    std::array<UserLib::PidControl, 4> rm_pid = {
        UserLib::PidControl(1000, 0.1, 0.0, 0.0, 10000, - 10000),
        UserLib::PidControl(1000, 0.1, 0.0, 0.0, 10000, - 10000),
        UserLib::PidControl(1000, 0.1, 0.0, 0.0, 10000, - 10000),
        UserLib::PidControl(1000, 0.1, 0.0, 0.0, 10000, - 10000)
    };
    
    void cpp_main_init(void);
    void cpp_main_while(void);
    void timer_interruption_1kHz(void);


}

#endif /* CPP_MAIN_HPP_ */