#ifndef SOFTWARE_FIFO_HPP_
#define SOFTWARE_FIFO_HPP_

#include "main.h"

namespace UserLib{

template<typename T, size_t buf_size>
class SoftwareFifo{
private:
    const size_t BUF_SIZE = 1 << buf_size;
    const size_t BUF_MASK = BUF_SIZE - 1;
    T buffer[1 << buf_size];
    size_t data_head = 0;
    size_t data_tail = 0;
    size_t total_data = 0;

public:
    bool input(const T &input_data){
        buffer[data_head] = input_data;
        data_head = (data_head + 1) & BUF_MASK;
        total_data ++;
        if(total_data > BUF_SIZE){
            total_data = BUF_SIZE;
            data_tail = data_head;
            return false;
        }else{
            return true;
        }
    }

    bool output(T &output_data){
        if(total_data > 0){
            output_data = buffer[data_tail];
            data_tail = (data_tail + 1) & BUF_MASK;
            total_data --;
            return true;
        }else{
            return false;
        }
    }

    size_t get_free_level(){
        return BUF_SIZE - total_data;
    }

    size_t get_busy_level(){
        return total_data;
    }

    void reset(){
        data_head = 0;
        data_tail = 0;
        total_data = 0;
    }

};

}


#endif /* SOFTWARE_FIFO_HPP_ */