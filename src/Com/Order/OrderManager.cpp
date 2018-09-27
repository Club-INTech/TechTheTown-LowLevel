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

    init();

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
    char str_order[RX_BUFFER_SIZE];
    char orderBuffer[RX_BUFFER_SIZE];
    strcpy(orderBuffer, orderToExecute);

    int8_t n_param = split(orderBuffer, orderData,
                           SEPARATOR);        //Sépare l'ordre en plusieurs mots, n_param=nombre de paramètres

    if (n_param >= 0) {
        strcpy(str_order, orderData.at(0));

        Serial.println(orderToExecute);

        auto it = orders.find(str_order);
        if(it != orders.end())
            it->second->exec(orderData);
        else
        {
            highLevel.printfln(STD_HEADER,"ordre inconnu");
            highLevel.printfln(DEBUG_HEADER,"T'es un déchêt");
        }

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

void OrderManager::init() {

    orders.clear();

    orders.insert( {"?", new ORDER_PING} );
    orders.insert( {"j", new ORDER_J} );
    orders.insert( {"f", new ORDER_F} );
    orders.insert( {"?xyo", new ORDER_XYO} );
    orders.insert( {"d", new ORDER_D} );
    orders.insert( {"t", new ORDER_T} );
    orders.insert( {"goto", new ORDER_GOTO} );
    orders.insert( {"followtrajectory", new ORDER_FOLLOWTRAJECTORY} );
    orders.insert( {"stop", new ORDER_STOP} );
    orders.insert( {"emergencyStop", new ORDER_EMERGENCYSTOP} );
    orders.insert( {"resumeEmergencyStop", new ORDER_RESUMEEMERGENCYSTOP} );

    orders.insert( {"cx", new ORDER_CX} );
    orders.insert( {"cy", new ORDER_CY} );
    orders.insert( {"co", new ORDER_CO} );
    orders.insert( {"cxyo", new ORDER_CXYO} );
    orders.insert( {"ctv", new ORDER_CTV} );
    orders.insert( {"crv", new ORDER_CRV} );
    orders.insert( {"ctrv", new ORDER_CTRV} );
    orders.insert( {"efm", new ORDER_EFM} );
    orders.insert( {"dfm", new ORDER_DFM} );

    orders.insert( {"ct0", new ORDER_CT0} );
    orders.insert( {"ct1", new ORDER_CT1} );
    orders.insert( {"cr0", new ORDER_CR0} );
    orders.insert( {"cr1", new ORDER_CR1} );
    orders.insert( {"cv0", new ORDER_CV0} );
    orders.insert( {"cv1", new ORDER_CV1} );

    orders.insert( {"cod", new ORDER_COD} );
    orders.insert( {"pfdebug", new ORDER_PFDEBUG} );
    orders.insert( {"rawpwm", new ORDER_RAWPWM} );
    orders.insert( {"getpwm", new ORDER_GETPWM} );
    orders.insert( {"errors", new ORDER_ERRORS} );
    orders.insert( {"rawposdata", new ORDER_RAWPOSDATA} );
    orders.insert( {"rawspeed", new ORDER_RAWSPEED} );

    orders.insert( {"montlhery", new ORDER_MONTLHERY} );
    orders.insert( {"av", new ORDER_AV} );
    orders.insert( {"rc", new ORDER_RC} );
    orders.insert( {"td", new ORDER_TD} );
    orders.insert( {"tg", new ORDER_TG} );
    orders.insert( {"sstop", new ORDER_SSTOP} );

    orders.insert( {"toggle", new ORDER_TOGGLE} );
    orders.insert( {"display", new ORDER_DISPLAY} );
    orders.insert( {"kpt", new ORDER_KPT} );
    orders.insert( {"kdt", new ORDER_KDT} );
    orders.insert( {"kit", new ORDER_KIT} );
    orders.insert( {"kpr", new ORDER_KPR} );
    orders.insert( {"kir", new ORDER_KIR} );
    orders.insert( {"kdr", new ORDER_KDR} );
    orders.insert( {"kpg", new ORDER_KPG} );
    orders.insert( {"kpd", new ORDER_KPD} );
    orders.insert( {"kid", new ORDER_KID} );
    orders.insert( {"kdd", new ORDER_KDD} );

    orders.insert( {"axm", new ORDER_AXM} );
    orders.insert( {"axgm", new ORDER_AXGM} );
    orders.insert( {"axs", new ORDER_AXS} );
    orders.insert( {"axgs", new ORDER_AXGS} );

    orders.insert( {"blbavbei", new ORDER_BLBAVBEI} );
    orders.insert( {"blbav", new ORDER_BLBAV} );
    orders.insert( {"rlbav", new ORDER_RLBAV} );
    orders.insert( {"blbarbei", new ORDER_BLBARBEI} );
    orders.insert( {"blbar", new ORDER_BLBAR} );
    orders.insert( {"rlbar", new ORDER_RLBAR} );

    orders.insert( {"flpav", new ORDER_FLPAV} );
    orders.insert( {"olpav", new ORDER_OLPAV} );
    orders.insert( {"flpar", new ORDER_FLPAR} );
    orders.insert( {"olpar", new ORDER_OLPAR} );
    orders.insert( {"olparp", new ORDER_OLPARP} );
    orders.insert( {"olpavp", new ORDER_OLPAVP} );

    orders.insert( {"alp", new ORDER_ALP} );
    orders.insert( {"dlp", new ORDER_DLP} );

    orders.insert( {"aeAv", new ORDER_AEAV} );
    orders.insert( {"deAv", new ORDER_DEAV} );
    orders.insert( {"aeAr", new ORDER_AEAR} );
    orders.insert( {"deAr", new ORDER_DEAR} );

    orders.insert( {"sus", new ORDER_SUS} );
    orders.insert( {"ccAv", new ORDER_CCAV} );
    orders.insert( {"ccAr", new ORDER_CCAR} );
    orders.insert( {"bde", new ORDER_BDE} );
    orders.insert( {"bdd", new ORDER_BDD} );

    orders.insert( {"nh", new ORDER_NH} );
    orders.insert( {"eh", new ORDER_EH} );
    orders.insert( {"dh", new ORDER_DH} );

    orders.insert( {"ptpdemo", new ORDER_PTPDEMO} );
    orders.insert( {"ptpdemoseq",new ORDER_PTPDEMOSEQ} );
}
