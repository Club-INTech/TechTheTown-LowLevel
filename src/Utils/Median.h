//
// Created by asphox on 06/05/18.
//

#ifndef TECHTHETOWN_LOWLEVEL_MEDIAN_H
#define TECHTHETOWN_LOWLEVEL_MEDIAN_H

#include <Arduino.h>
#include <array>
#include <algorithm>

template< typename T , unsigned int BUFFER_SIZE >
class Median
{
public:
    Median(){ reset(); }

    inline void reset(){ buffer.fill(0); }

    T value() const {
        if( BUFFER_SIZE & 1 ) //si impair
            return tmp[BUFFER_SIZE/2];
        else
            return (tmp[BUFFER_SIZE/2]+tmp[BUFFER_SIZE/2-1])/2;
    }

    void add(T val){
        for(unsigned int i = 1 ; i < BUFFER_SIZE ; i++ )
            buffer[i-1] = buffer[i];
        buffer[BUFFER_SIZE-1] = val;

        tmp = buffer;
        std::sort(tmp.begin(),tmp.end());
    }

private:
    std::array<T,BUFFER_SIZE> tmp;
    std::array<T,BUFFER_SIZE> buffer;
};


#endif //TECHTHETOWN_LOWLEVEL_MEDIAN_H
