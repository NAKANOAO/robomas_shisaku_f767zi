#ifndef CPP_MAIN_HPP_
#define CPP_MAIN_HPP_


extern "C"{
    #include "main.h"
    #include "gpio.h"
    #include "can.h"
    #include "usart.h"
}

#include "LED_GPIO.hpp"



namespace UserCode{

    UserLib::LedControl myled(LED_R_PORT, LED_R_PIN);
    void cpp_main_init(void);
    void cpp_main_while(void);

}

#endif /* CPP_MAIN_HPP_ */