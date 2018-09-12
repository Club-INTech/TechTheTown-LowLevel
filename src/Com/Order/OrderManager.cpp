#include "OrderManager.h"
#include "Actuators/ActuatorValues.h"

OrderManager::OrderManager():
        hookList(HookList()),
        orderData(OrderData()),
        sensorMgr(SensorMgr::Instance()),
        motionControlSystem(MotionControlSystem::Instance()),
        actuatorsMgr(ActuatorsMgr::Instance()),
        highLevel(ComMgr::Instance())
{

    memset(readMessage, 0, RX_BUFFER_SIZE);
    isSendingUS = true;
    hooksEnabled = true;
    HLWaiting = false;
    highLevel.printfln(DEBUG_HEADER,"Communications ready");
}

void OrderManager::communicate() {

    if (highLevel.read(readMessage)) {
        execute(readMessage);
    }

    memset(readMessage, 0, RX_BUFFER_SIZE);

    static Metro checkMovement = Metro(10);
    static Metro checkHooksTimer = Metro(20);


    if (checkMovement.check())
    {
        if (!motionControlSystem.sentMoveAbnormal() && motionControlSystem.isMoveAbnormal()) {//Si on est bloqué et qu'on n'a pas encore prévenu
            motionControlSystem.setMoveAbnormalSent(true);
            highLevel.sendEvent("unableToMove p");
        }
        else if (motionControlSystem.sentMoveAbnormal() && !motionControlSystem.isMoveAbnormal()) {//Si on est plus bloqué et qu'on avait prévenu
            motionControlSystem.setMoveAbnormalSent(false);
        }
    }

    if (checkHooksTimer.check() && hooksEnabled)
    {
        checkHooks();
        executeHooks();
    }

    static Metro sendPos = Metro(50);
    if (com_options & COM_OPTIONS::ETHERNET_W && sendPos.check()) {
        if (motionControlSystem.isMoving()) {
            float posToSend[3]={motionControlSystem.getX(), motionControlSystem.getY(), motionControlSystem.getAngleRadian()};
            highLevel.sendPosition(posToSend);
        } else {
            if (motionControlSystem.previousIsMoving()){
                highLevel.sendEvent("stoppedMoving");
                motionControlSystem.setPreviousIsMoving(false);
            }
        }
    }
}




void OrderManager::execute(const char* orderToExecute)
{
//	#ifdef DEBUG                    /*A LAISSER COMMENTÉ
    char str_order[RX_BUFFER_SIZE];
//    #else                          *TANT QU'ON RESTE EN ORDRES
//    int order;                     *SOUS FORME STRINGS
//    #endif                        \*m'voyez
    char orderBuffer[RX_BUFFER_SIZE];
    strcpy(orderBuffer, orderToExecute);

    int8_t n_param = split(orderBuffer, orderData,
                           SEPARATOR);        //Sépare l'ordre en plusieurs mots, n_param=nombre de paramètres

    if (n_param >= 0) {
        //#ifdef DEBUG
        strcpy(str_order, orderData.at(0));
        //#else
        //order = parseInt(orderData.at(0));
        //#endif //DEBUG

        AbstractOrder* order;

        if (!strcmp(str_order, "?"))            //Ping
        {
            order = new ORDER_PING;
        }
        else if (!strcmp(str_order, "j"))       //Le HL attend l'activation du Jumper
        {
            order = new ORDER_J;
        }
        else if (!strcmp(str_order, "f"))
        {
            order = new ORDER_F;
        }
        else if (!strcmp(str_order, "?xyo"))		//Renvoie la position du robot (en mm et radians)
        {
            order = new ORDER_XYO;
        }
        else if (!strcmp(str_order, "d"))		//Ordre de déplacement rectiligne (en mm)
        {
            order = new ORDER_D;
        }
        else if (!strcmp(str_order, "t")) {
            order = new ORDER_T;
        }
        else if (!strcmp(str_order, "stop")) {
            order = new ORDER_STOP;
        }
        else if (!strcmp(str_order, "emergencyStop")) {
            order = new ORDER_EMERGENCYSTOP;
        }
        else if (!strcmp(str_order, "resumeEmergencyStop")) {
            order = new ORDER_RESUMEEMERGENCYSTOP;
        }
        else if (!strcmp(str_order, "cx")) {
            order = new ORDER_CX;
        }
        else if (!strcmp(str_order, "cy")) {
            order = new ORDER_CY;
        }
        else if (!strcmp(str_order, "co")) {
            order = new ORDER_CO;
        }
        else if (!strcmp(str_order, "cxyo")) {
            order = new ORDER_CXYO;
        }
        else if (!strcmp(str_order, "ctv")) {
            order = new ORDER_CTV;
        }
        else if (!strcmp(str_order, "crv")) {
            order = new ORDER_CRV;
        }
        else if (!strcmp(str_order, "ctrv")) {
            order = new ORDER_CTRV;
        }
        else if (!strcmp(str_order, "efm")) {
            order = new ORDER_EFM;
        }
        else if (!strcmp(str_order, "dfm")) {
            order = new ORDER_DFM;
        }
        else if (!strcmp(str_order, "ct0")){        //Désactiver l'asservissement en translation
            order = new ORDER_CT0;
        }
        else if (!strcmp(str_order, "ct1")){		//Activer l'asservissement en translation
            order = new ORDER_CT1;
        }
        else if (!strcmp(str_order, "cr0")){		//Désactiver l'asservissement en rotation
            order = new ORDER_CR0;
        }
        else if (!strcmp(str_order, "cr1")){		//Activer l'asservissement en rotation
            order = new ORDER_CR1;
        }
        else if (!strcmp(str_order, "cv0")){		//Désactiver l'asservissement en vitesse
            order = new ORDER_CV0;
        }
        else if (!strcmp(str_order, "cv1")){		//Activer l'asservissement en vitesse
            order = new ORDER_CV1;
        }
        else if (!strcmp(str_order, "cod")) {
            order = new ORDER_COD;
        }
        else if (!strcmp(str_order, "pfdebug"))
        {
            order = new ORDER_PFDEBUG;
        }
        else if (!strcmp(str_order, "rawpwm"))
        {
            order = new ORDER_RAWPWM;
        }
        else if (!strcmp(str_order, "getpwm")) {
            order = new ORDER_GETPWM;
        }
        else if (!strcmp(str_order, "errors")) {
            order = new ORDER_ERRORS;
        }
        else if (!strcmp(str_order, "rawspeed")) {
            order = new ORDER_RAWSPEED;
        }
        else if (!strcmp(str_order, "rawposdata")){
            order = new ORDER_RAWPOSDATA;
        }
        else if (!strcmp(str_order, "montlhery")) {
            order = new ORDER_MONTLHERY;
        }
        else if (!strcmp(str_order, "av")) {
            order = new ORDER_AV;
        }
        else if (!strcmp(str_order, "rc")) {
            order = new ORDER_RC;
        }
        else if (!strcmp(str_order, "td")) {
            order = new ORDER_TD;
        }
        else if (!strcmp(str_order, "tg")) {
            order = new ORDER_TG;
        }
        else if (!strcmp(str_order, "sstop")){
            order = new ORDER_SSTOP;
        }
        else if (!strcmp(str_order, "toggle")) {
            order = new ORDER_TOGGLE;
        }
        else if (!strcmp(str_order, "display")) {
            order = new ORDER_DISPLAY;
        }
        else if (!strcmp(str_order, "kpt")) {
            order = new ORDER_KPT;
        } else if (!strcmp(str_order, "kdt")) {
            order = new ORDER_KDT;
        } else if (!strcmp(str_order, "kit")) {
            order = new ORDER_KIT;
        }
        else if (!strcmp(str_order, "kpr")) {
            order = new ORDER_KPR;
        } else if (!strcmp(str_order, "kir")) {
            order = new ORDER_KIR;
        } else if (!strcmp(str_order, "kdr")) {
            order = new ORDER_KDR;
        }
        else if (!strcmp(str_order, "kpg")) {
            order = new ORDER_KPG;
        }
        else if (!strcmp(str_order, "kig")) {
            order = new ORDER_KIG;
        }
        else if (!strcmp(str_order, "kdg")) {
            order = new ORDER_KDG;
        }
        else if (!strcmp(str_order, "kpd")) {
            order = new ORDER_KPD;
        }
        else if (!strcmp(str_order, "kid")) {
            order = new ORDER_KID;
        }
        else if (!strcmp(str_order, "kdd")) {
            order = new ORDER_KDD;
        }
        else if (!strcmp(str_order, "AXm")) {
            order = new ORDER_AXM;
        }
        else if (!strcmp(str_order, "AXGm")) {
            order = new ORDER_AXGM;
        }
        else if (!strcmp(str_order, "AXs")) {
            order = new ORDER_AXS;
        }
        else if (!strcmp(str_order, "AXGs")) {
            order = new ORDER_AXGS;
        }
        else if (!strcmp(str_order, "blbAvbei")) {
            order = new ORDER_BLBAVBEI;
        }
        else if (!strcmp(str_order, "blbAv")) {
            order = new ORDER_BLBAV;
        }
        else if (!strcmp(str_order, "rlbAv")) {
            order = new ORDER_RLBAV;
        }
        else if (!strcmp(str_order, "blbArbei")) {
            order = new ORDER_BLBARBEI;
        }
        else if (!strcmp(str_order, "blbAr")) {
            order = new ORDER_BLBAR;
        }
        else if (!strcmp(str_order, "rlbAr")) {
            order = new ORDER_RLBAR;
        }
        else if (!strcmp(str_order, "flpAv")) {
            order = new ORDER_FLPAV;
        }
        else if (!strcmp(str_order, "olpAv")) {
            order = new ORDER_OLPAV;
        }
        else if (!strcmp(str_order, "flpAr")) {
            order = new ORDER_FLPAR;
        }
        else if (!strcmp(str_order, "olpAr")) {
            order = new ORDER_OLPAR;
        }
        else if (!strcmp(str_order, "olpAvp")) {
            order = new ORDER_OLPAVP;
        }
        else if (!strcmp(str_order, "olpArp")) {
            order = new ORDER_OLPARP;
        }
        else if (!strcmp(str_order, "alp")) {
            order = new ORDER_ALP;
        }
        else if (!strcmp(str_order, "dlp")) {
            order = new ORDER_DLP;
        }
        else if (!strcmp(str_order, "aeAv")) {
            order = new ORDER_AEAV;
        }
        else if (!strcmp(str_order, "deAv")) {
            order = new ORDER_DEAV;
        }
        else if (!strcmp(str_order, "aeAr")) {
            order = new ORDER_AEAR;
        }
        else if (!strcmp(str_order, "deAr")) {
            order = new ORDER_DEAR;
        }
        else if (!strcmp(str_order, "sus")){
            order = new ORDER_SUS;
        }
        else if (!strcmp(str_order, "ccAv")) {
            order = new ORDER_CCAV;
        }
        else if (!strcmp(str_order, "ccAr")) {
            order = new ORDER_CCAR;
        }
        else if (!strcmp(str_order, "bde")) {
            order = new ORDER_BDE;
        }
        else if (!strcmp(str_order, "bdd")) {
            order = new ORDER_BDD;
        }
        else if (!strcmp(str_order, "nh")) {
            order = new ORDER_NH;
        }
        else if (!strcmp(str_order, "eh")) {
            order = new ORDER_EH;
        }
        else if (!strcmp(str_order, "dh")) {
            order = new ORDER_DH;
        }
        else {
            order = new ORDER_UNKOWN;
        }

        order->exec(orderData);

        delete order;

    }


    checkHooks();
}

/**
*	Sépare une courte chaîne de caractères(RX_BUFFER_SIZE) selon un séparateur, dans un tableau output (au plus 4 mots)
*/

int8_t OrderManager::split(char* input, OrderData& output, const char* separator) {
    char *token;
    int8_t i = -1;
    output.clear();
    token = strtok(input, separator);
    if (token != nullptr) {
        output.push_back(token);
        i++;
    }
    do {
        token = strtok(nullptr, separator);
        if (token != nullptr) {
            output.push_back(token);
            ++i;
        }
    } while (token != nullptr && i < RX_WORD_COUNT);

    return i;
}

int OrderManager::parseInt(const char* s) {
    return strtol(s, nullptr, DEC);
}

float OrderManager::parseFloat(const char* s) {
    return strtof(s, nullptr);
}

bool OrderManager::isHLWaiting() {
    return(HLWaiting);
}

void OrderManager::checkJumper() {
    if(sensorMgr.isReadyToGo() && HLWaiting)
    {
        highLevel.startMatch();
        HLWaiting = false;
    }
}

void OrderManager::checkHooks() {
    hookList.check(motionControlSystem.getX(), motionControlSystem.getY(),motionControlSystem.getAngleRadian());
}

void OrderManager::executeHooks() {
    std::vector<String> orders = hookList.executeHooks();

    for(String &order : orders)
    {
        execute(order.c_str());
    }
}
