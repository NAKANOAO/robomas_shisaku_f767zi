#include "cpp_main.hpp"



void wrap_cpp_main_init(void){
    UserCode::cpp_main_init();
}
void wrap_cpp_main_while (void){
    UserCode::cpp_main_while();
}


namespace UserCode{
    void cpp_main_init(void){
        // nothing to do
    }
    void cpp_main_while(void){
        myled.Change_State(UserLib::LEDState::ON);
        HAL_Delay(3000);
        myled.Change_State(UserLib::LEDState::OFF);
        HAL_Delay(3000);
    }

}