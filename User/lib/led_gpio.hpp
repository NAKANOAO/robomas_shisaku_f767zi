#ifndef LED_GPIO_HPP_
#define LED_GPIO_HPP_


extern "C"{
    #include "main.h"    
}


namespace UserLib{


enum class LEDState{
    ON = 1,
    OFF = 0
};

class LedControl{
private:
    GPIO_TypeDef *gpio_port;
    const uint16_t gpio_pin;

public:
    LedControl(GPIO_TypeDef *_gpio_port, const uint16_t _gpio_pin)
        : gpio_port(_gpio_port), gpio_pin(_gpio_pin){
    }

    void Change_State (const LEDState state);


};


void LedControl::Change_State (const LEDState state){
    if (state == LEDState::ON){
        HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_SET);
    }
    else{
        HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_RESET);
    }
}


}

#endif /* LED_GPIO_HPP_ */