//main utilisé par les tests unitaires

#include <Arduino.h>
#include <unity.h>
#include <Com/OrderManager.h>

void setup(){

    delay(2000);

    UNITY_BEGIN();
}


void loop(){
    OrderManager& orderMgr=OrderManager::Instance();
    bool endTest=false;

    const char *stringFloat="-5631.3365";
    const char *stringNotFloat="-56-31.33.65";
    const char *stringInt="-5665";
    const char *stringNotInt="-563.1";
    //Test parse/split

    UNITY_END();
}

