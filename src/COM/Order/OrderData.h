//
// Created by asphox on 30/04/18.
//

#ifndef TECHTHETOWN_LOWLEVEL_ORDERDATA_H
#define TECHTHETOWN_LOWLEVEL_ORDERDATA_H

#include <Arduino.h>
#include <vector>

class OrderData {

private:
    std::vector<String> orderData;
public:
    OrderData() : orderData(std::vector<String>()) {}
    void push_back(const String& s) {
        orderData.push_back(s);
    }
    const char* pop() {
        const char* buffer = orderData.at(orderData.size()).c_str();
        orderData.pop_back();
        return buffer;
    }
    const char* at(uint8_t i) {
        return orderData.at(i).c_str();
    }
    const char* operator[](uint8_t i){
        return at(i+1);
    }
    void clear() {
        orderData.clear();
    }
    inline uint8_t size() {
        return orderData.size();
    }
    inline uint8_t nbrParams() {
        return size()-1;
    }
};

#endif //TECHTHETOWN_LOWLEVEL_ORDERDATA_H
