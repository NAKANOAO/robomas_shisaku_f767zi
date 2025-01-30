#include "LED_GPIO.hpp"

namespace UserLib{

    void LedControl::Change_State (const LEDState state){
        if (state == LEDState::ON){
            HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_SET);
        }
        else{
            HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_RESET);
        }
    }

}