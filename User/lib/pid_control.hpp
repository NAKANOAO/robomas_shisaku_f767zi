#ifndef PID_CONTROL_HPP_
#define PID_CONTROL_HPP_

#include "main.h"

namespace UserLib{

class PidControl{

private:
    float gain_p, gain_i, gain_d;
    float lim_min, lim_max;
    size_t loop_hz;
    float old_diff;
    float diff_integ;

public:

    PidControl(size_t _loop_hz, float _gain_p, float _gain_i, float _gain_d, float _lim_min, float _lim_max){
        gain_p = _gain_p;
        gain_i = _gain_i;
        gain_d = _gain_d;
        lim_min = _lim_min;
        lim_max = _lim_max;
        loop_hz = _loop_hz;
    }

    float culculate_output(float diff){
        float output = 0.0;
        diff_integ += (diff + old_diff) / 2.0000 * (1.000 / this -> loop_hz);
        output += diff * gain_p;
        output += (diff - old_diff) / (1.0000 / loop_hz) * gain_d;
        output += diff_integ * gain_i;
        
        return output;
    }

    void set_gain(float _gain_p, float _gain_i, float _gain_d){
        gain_p = _gain_p;
        gain_i = _gain_i;
        gain_d = _gain_d;
    }

    void set_lim(float _lim_min, float _lim_max){
        lim_min = _lim_min;
        lim_max = _lim_max;
    }

    void reset(void){
        old_diff = 0.0;
        diff_integ = 0.0;
    }

};

}

#endif /* PID_CONTROL_HPP_ */