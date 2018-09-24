//
// Created by asphox on 29/04/18.
//


#include "Orders.h"

void ORDER_UNKOWN::impl(Args args)
{
    orderManager.highLevel.printfln(STD_HEADER,"ordre inconnu");
    orderManager.highLevel.printfln(DEBUG_HEADER,"T'es un déchêt");
}

void ORDER_PING::impl(Args args)
{
    orderManager.highLevel.printfln(STD_HEADER,"0");
}

void ORDER_J::impl(Args args)
{
    orderManager.HLWaiting = true;
}

void ORDER_F::impl(Args args)
{
    orderManager.highLevel.printfln(STD_HEADER,"%d",orderManager.motionControlSystem.isMoving());
    orderManager.highLevel.printfln(STD_HEADER,"%d",orderManager.motionControlSystem.isMoveAbnormal());
}

void ORDER_XYO::impl(Args args)
{
    orderManager.highLevel.printfln(STD_HEADER,"%f",orderManager.motionControlSystem.getX());
    orderManager.highLevel.printfln(STD_HEADER,"%f",orderManager.motionControlSystem.getY());
    orderManager.highLevel.printfln(STD_HEADER,"%f",orderManager.motionControlSystem.getAngleRadian());
}

void ORDER_D::impl(Args args)
{
    int16_t deplacement = strtod(args[0], nullptr);
    orderManager.highLevel.printfln(DEBUG_HEADER,"distance : %d",deplacement);
    orderManager.motionControlSystem.disablePointToPoint();
    orderManager.motionControlSystem.orderTranslation(deplacement);
}

void ORDER_T::impl(Args args)
{
    float angle;
    if (!strcmp(args[0], "pi")) {
        angle = (float) PI;
    } else {
        angle = strtof(args[0], nullptr);
    }
    orderManager.highLevel.printfln(DEBUG_HEADER,"angle : %f", angle);

    MotionControlSystem::RotationWay rotationWay = MotionControlSystem::FREE;
    if(args.nbrParams() > 1)
    {
        if(!strcmp(args[1],"trigo"))
        {
            rotationWay = MotionControlSystem::TRIGO;
        }
        else if(!strcmp(args[1],"antitrigo"))
        {
            rotationWay = MotionControlSystem::ANTITRIGO;
        }
        else if(strcmp(args[1],"free") != 0)
        {
            orderManager.highLevel.printfln(DEBUG_HEADER,"ARGUMENT INVALIDE");
        }
    }

    orderManager.motionControlSystem.disablePointToPoint();
    orderManager.motionControlSystem.orderRotation(angle, rotationWay);
}

void ORDER_GOTO::impl(Args args)
{
    float targetX = strtof(args[0],nullptr);
    float targetY = strtof(args[1],nullptr);
    bool isSequential = false;

    if(args.nbrParams() == 3)
    {
        isSequential = !strcmp(args[2],"true") || !strcmp(args[2],"1");
        Serial.print("On séquentialise : ");
        Serial.println(isSequential);
    }
//                if(-1500 <= targetX && targetX <= 1500 && 0 <= targetY && targetY <= 2000)
//                {
        orderManager.motionControlSystem.orderGoto(targetX,targetY, isSequential);
//                }
//                else
//      {
//                highLevel.log("ERREUR::Paramètres incorrects");
//      }
}

void ORDER_FOLLOWTRAJECTORY::impl(Args args)
{
    if(strtof(args[0], nullptr) == 0)
    {
        orderManager.motionControlSystem.orderTrajectory(trajectory_S_path[0],trajectory_S_path[1],trajectory_S_size);
    }
    else
    {
        orderManager.highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
    }
}

void ORDER_STOP::impl(Args args)
{


    orderManager.motionControlSystem.stop();
    orderManager.highLevel.printfln(DEBUG_HEADER,"A priori, je m'arrête");
}

void ORDER_EMERGENCYSTOP::impl(Args args)
{

}

void ORDER_RESUMEEMERGENCYSTOP::impl(Args args)
{

}

void ORDER_CX::impl(Args args)
{
    orderManager.motionControlSystem.setX(orderManager.parseFloat(args[0]));
}

void ORDER_CY::impl(Args args)
{
    orderManager.motionControlSystem.setY(orderManager.parseFloat(args[0]));
}

void ORDER_CO::impl(Args args)
{
    orderManager.motionControlSystem.setOriginalAngle(orderManager.parseFloat(args[0]));
}

void ORDER_CXYO::impl(Args args)
{
    orderManager.motionControlSystem.setX(orderManager.parseFloat(args[0]));
    orderManager.motionControlSystem.setY(orderManager.parseFloat(args[1]));
    orderManager.motionControlSystem.setOriginalAngle(orderManager.parseFloat(args[2]));
}

void ORDER_CTV::impl(Args args)
{
    orderManager.motionControlSystem.setTranslationSpeed(orderManager.parseFloat(args[0]));
}

void ORDER_CRV::impl(Args args)
{
    orderManager.motionControlSystem.setRotationSpeed(orderManager.parseFloat(args[0]));
}

void ORDER_CTRV::impl(Args args)
{
    orderManager.motionControlSystem.setTranslationSpeed(orderManager.parseFloat(args[0]));
    orderManager.motionControlSystem.setRotationSpeed(orderManager.parseFloat(args[1]));
}

void ORDER_EFM::impl(Args args)
{
    orderManager.motionControlSystem.enableForcedMovement(true);
}

void ORDER_DFM::impl(Args args)
{
    orderManager.motionControlSystem.enableForcedMovement(true);
}

void ORDER_CT0::impl(Args args)
{
    orderManager.motionControlSystem.enableTranslationControl(false);
    orderManager.highLevel.printfln(DEBUG_HEADER,"non asservi en translation");
}

void ORDER_CT1::impl(Args args)
{
    orderManager.motionControlSystem.enableTranslationControl(true);
    orderManager.highLevel.printfln(DEBUG_HEADER,"asservi en translation");
}

void ORDER_CR0::impl(Args args)
{
    orderManager.motionControlSystem.enableRotationControl(false);
    orderManager.highLevel.printfln(DEBUG_HEADER,"non asservi en rotation");
}

void ORDER_CR1::impl(Args args)
{
    orderManager.motionControlSystem.enableRotationControl(true);
    orderManager.highLevel.printfln(DEBUG_HEADER,"asservi en rotation");
}

void ORDER_CV0::impl(Args args)
{
    orderManager.motionControlSystem.enableSpeedControl(false);
    orderManager.highLevel.printfln(DEBUG_HEADER,"non asservi en vitesse");
}

void ORDER_CV1::impl(Args args)
{
    orderManager.motionControlSystem.enableSpeedControl(true);
    orderManager.highLevel.printfln(DEBUG_HEADER,"asservi en vitesse");
}

void ORDER_COD::impl(Args args)
{
    orderManager.highLevel.printfln(DEBUG_HEADER,"Gauche:");
    orderManager.highLevel.printfln(DEBUG_HEADER,"%ld", orderManager.motionControlSystem.getLeftTick());
    orderManager.highLevel.printfln(DEBUG_HEADER,"Droite:");
    orderManager.highLevel.printfln(DEBUG_HEADER,"%ld", orderManager.motionControlSystem.getRightTick());
}

void ORDER_PFDEBUG::impl(Args args)
{
    //highLevel.printfln(STD_HEADER,"%d", (int)motionControlSystem.getRightSpeed());
    //highLevel.printfln(STD_HEADER,"%d", (int)motionControlSystem.getRightMotorDir());
    //highLevel.printfln(STD_HEADER,"%d", (int)motionControlSystem.getRightSetPoint());
    //highLevel.printfln(STD_HEADER,"%d", (int)motionControlSystem.getRightMotorPWM());
    //highLevel.printfln(STD_HEADER,"%d", (int)motionControlSystem.getCodD());
}

void ORDER_RAWPWM::impl(Args args)
{
    uint8_t rawpwm = orderManager.parseInt(args[0]);
    orderManager.motionControlSystem.orderRawPwm(Side::LEFT, rawpwm);
    orderManager.motionControlSystem.orderRawPwm(Side::RIGHT, rawpwm);
}

void ORDER_GETPWM::impl(Args args)
{
    int32_t left, right;
    orderManager.motionControlSystem.getPWMS(left, right);
    orderManager.highLevel.printfln(DEBUG_HEADER,"PWMS: %ld - %ld", left, right);
}

void ORDER_ERRORS::impl(Args args)
{
    float leftProp, leftDer, leftInt, rightProp, rightDer, rightInt;
    orderManager.motionControlSystem.getSpeedErrors(leftProp, leftInt, leftDer, rightProp, rightInt, rightDer);
    orderManager.highLevel.printfln(DEBUG_HEADER,"Prop: %f - %f", leftProp, rightProp);
    orderManager.highLevel.printfln(DEBUG_HEADER,"Deriv: %f - %f", leftDer, rightDer);
    orderManager.highLevel.printfln(DEBUG_HEADER,"Integ: %f - %f", leftInt, rightInt);
}

void ORDER_RAWSPEED::impl(Args args)
{
    int32_t leftsetpoint, rightsetpoint;

    //motionControlSystem.rawWheelSpeed(parseInt(orderData.at(1)), leftsetpoint, rightsetpoint);
    orderManager.highLevel.printfln(DEBUG_HEADER,"Speed set");
    orderManager.motionControlSystem.getSpeedSetpoints(leftsetpoint, rightsetpoint);
    orderManager.highLevel.printfln(DEBUG_HEADER,"speed setpoints: %ld - %ld", leftsetpoint, rightsetpoint);
}

void ORDER_RAWPOSDATA::impl(Args args)
{
    int32_t leftsetpoint, rightsetpoint;
    orderManager.motionControlSystem.getSpeedSetpoints(leftsetpoint, rightsetpoint);
    Serial.print(orderManager.motionControlSystem.getX());
    Serial.print(",");
    Serial.print(orderManager.motionControlSystem.getY());
    Serial.print(",");
    Serial.print(orderManager.motionControlSystem.getAngleRadian());
    Serial.print(",");
    Serial.print(orderManager.motionControlSystem.getLeftSpeed());
    Serial.print(",");
    Serial.print(leftsetpoint);
    Serial.print(",");
    Serial.print(orderManager.motionControlSystem.getRightSpeed());
    Serial.print(",");
    Serial.println(rightsetpoint);
    //int32_t right, left;
    //motionControlSystem.getPWMS(left,right);
    //Serial.println(right);
    //float rotaProp, rotaDer, rotaInt;
    //motionControlSystem.getRotationErrors(rotaProp, rotaInt, rotaDer);
    //Serial.println(rotaInt);
}

void ORDER_MONTLHERY::impl(Args args)
{
    orderManager.motionControlSystem.enableTranslationControl(false);
    orderManager.motionControlSystem.enableRotationControl(false);
    orderManager.motionControlSystem.enableForcedMovement(true);
}

void ORDER_AV::impl(Args args)
{
    orderManager.motionControlSystem.setRawPositiveTranslationSpeed();
}

void ORDER_RC::impl(Args args)
{
    orderManager.motionControlSystem.setRawNegativeTranslationSpeed();
}

void ORDER_TD::impl(Args args)
{
    orderManager.motionControlSystem.setRawNegativeRotationSpeed();
}

void ORDER_TG::impl(Args args)
{
    orderManager.motionControlSystem.setRawPositiveRotationSpeed();
}

void ORDER_SSTOP::impl(Args args)
{
    orderManager.motionControlSystem.setRawNullSpeed();
}

void ORDER_TOGGLE::impl(Args args)
{
    orderManager.motionControlSystem.translation = !orderManager.motionControlSystem.translation;   //Bascule entre le réglage d'asserv en translation et en rotation
    if (orderManager.motionControlSystem.translation) {
        orderManager.highLevel.printfln(DEBUG_HEADER, "reglage de la translation");
    } else
        orderManager.highLevel.printfln(DEBUG_HEADER, "reglage de la rotation");
}

void ORDER_DISPLAY::impl(Args args)
{
    float
            kp_t, ki_t, kd_t,      // Translation
            kp_r, ki_r, kd_r,      // Rotation
            kp_g, ki_g, kd_g,      // Vitesse gauche
            kp_d, ki_d, kd_d;      // Vitesse droite
    orderManager.motionControlSystem.getTranslationTunings(kp_t, ki_t, kd_t);
    orderManager.motionControlSystem.getRotationTunings(kp_r, ki_r, kd_r);
    orderManager.motionControlSystem.getLeftSpeedTunings(kp_g, ki_g, kd_g);
    orderManager.motionControlSystem.getRightSpeedTunings(kp_d, ki_d, kd_d);
    orderManager.highLevel.printfln(DEBUG_HEADER,"trans : kp= %g ; ki= %g ; kd= %g", kp_t, ki_t, kd_t);
    orderManager.highLevel.printfln(DEBUG_HEADER,"rot   : kp= %g ; ki= %g ; kd= %g", kp_r, ki_r, kd_r);
    orderManager.highLevel.printfln(DEBUG_HEADER,"gauche: kp= %g ; ki= %g ; kd= %g", kp_g, ki_g, kd_g);
    orderManager.highLevel.printfln(DEBUG_HEADER,"droite: kp= %g ; ki= %g ; kd= %g", kp_d, ki_d, kd_d);
}

void ORDER_KPT::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"kp_trans ?");
    orderManager.motionControlSystem.getTranslationTunings(kp, ki, kd);
    kp = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setTranslationTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"kp_trans = %g", kp);
}

void ORDER_KDT::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"kd_trans ?");
    orderManager.motionControlSystem.getTranslationTunings(kp, ki, kd);
    kd = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setTranslationTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"kd_trans = %g", kd);
}

void ORDER_KIT::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"ki_trans ?");
    orderManager.motionControlSystem.getTranslationTunings(kp, ki, kd);
    ki = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setTranslationTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"ki_trans = %g", ki);
}

void ORDER_KPR::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"kp_rot ?");
    orderManager.motionControlSystem.getRotationTunings(kp, ki, kd);
    kp = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setRotationTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"kp_rot = %g", kp);
}

void ORDER_KIR::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"ki_rot ?");
    orderManager.motionControlSystem.getRotationTunings(kp, ki, kd);
    ki = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setRotationTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"ki_rot = %g", ki);
}

void ORDER_KDR::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"kd_rot ?");
    orderManager.motionControlSystem.getRotationTunings(kp, ki, kd);
    kd = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setRotationTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"kd_rot = %g", kd);
}

void ORDER_KPG::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"kp_gauche ?");
    orderManager.motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
    kp = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"kp_gauche = %g", kp);
}

void ORDER_KIG::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"ki_gauche ?");
    orderManager.motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
    ki = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"ki_gauche = %g", ki);
}

void ORDER_KDG::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"kd_gauche ?");
    orderManager.motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
    kd = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"kd_gauche = %g", kd);
}

void ORDER_KPD::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"kp_droite ?");
    orderManager.motionControlSystem.getRightSpeedTunings(kp, ki, kd);
    kp = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setRightSpeedTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"kp_droite = %g", kp);
}

void ORDER_KID::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"ki_droite ?");
    orderManager.motionControlSystem.getRightSpeedTunings(kp, ki, kd);
    ki = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setRightSpeedTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"ki_droite = %g", ki);
}

void ORDER_KDD::impl(Args args)
{
    float kp, ki, kd;
    orderManager.highLevel.printfln(STD_HEADER,"kd_droite ?");
    orderManager.motionControlSystem.getRightSpeedTunings(kp, ki, kd);
    kd = orderManager.parseFloat(args[0]);
    orderManager.motionControlSystem.setRightSpeedTunings(kp, ki, kd);
    orderManager.highLevel.printfln(DEBUG_HEADER,"kd_droite = %g", kd);
}

void ORDER_AXM::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12(orderManager.parseInt(args[0]),(uint16_t)orderManager.parseInt(args[1]));
}

void ORDER_AXGM::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12G((unsigned int)orderManager.parseInt(args[0]),(uint16_t)orderManager.parseInt(args[1]));
}

void ORDER_AXS::impl(Args args)
{
    orderManager.actuatorsMgr.setAX12Speed(orderManager.parseInt(args[0]),(uint16_t)orderManager.parseInt(args[1]));
}

void ORDER_AXGS::impl(Args args)
{
    orderManager.actuatorsMgr.setAX12GSpeed((unsigned int)orderManager.parseInt(args[0]),(uint16_t)orderManager.parseInt(args[1]));
}

void ORDER_BLBAVBEI::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12G(frontArmGroup_ID, frontLowBee);
}

void ORDER_BLBAV::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12G(frontArmGroup_ID, frontLowCubes);
}

void ORDER_RLBAV::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12G(frontArmGroup_ID, frontFolded);
}

void ORDER_BLBARBEI::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12G(backArmGroup_ID, backLowBee);
}

void ORDER_BLBAR::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12G(backArmGroup_ID, backLowCubes);
}

void ORDER_RLBAR::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12G(backArmGroup_ID, backFolded);
}

void ORDER_FLPAV::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12(frontDoor_ID, frontDoorClosed);
}

void ORDER_OLPAV::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12(frontDoor_ID, frontDoorOpen);
}

void ORDER_FLPAR::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12(backDoor_ID, backDoorClosed);
}

void ORDER_OLPAR::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12(backDoor_ID, backDoorOpen);
}

void ORDER_OLPAVP::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12(frontDoor_ID, frontDoorClosed + doorSmallOpeningAngle);
}

void ORDER_OLPARP::impl(Args args)
{
    orderManager.actuatorsMgr.movAX12(backDoor_ID, backDoorClosed - doorSmallOpeningAngle);
}

void ORDER_ALP::impl(Args args)
{
    orderManager.actuatorsMgr.togglePumpState(true);
}

void ORDER_DLP::impl(Args args)
{
    orderManager.actuatorsMgr.togglePumpState(false);
}

void ORDER_AEAV::impl(Args args)
{
    orderManager.actuatorsMgr.setElecVanneAV(true);
}

void ORDER_DEAV::impl(Args args)
{
    orderManager.actuatorsMgr.setElecVanneAV(false);
}

void ORDER_AEAR::impl(Args args)
{
    orderManager.actuatorsMgr.setElecVanneAR(true);
}

void ORDER_DEAR::impl(Args args)
{
    orderManager.actuatorsMgr.setElecVanneAR(false);
}

void ORDER_SUS::impl(Args args)
{
    if(!strcmp(args[0],"on"))
    {
        orderManager.isSendingUS = true;
        orderManager.highLevel.printfln(DEBUG_HEADER,"Activation US");
    }
    else if(!strcmp(args[0],"off"))
    {
        orderManager.isSendingUS = false;
        orderManager.highLevel.printfln(DEBUG_HEADER,"Désactivation US");
    }
}

void ORDER_CCAV::impl(Args args)
{
    orderManager.sensorMgr.checkCubeAV();
}

void ORDER_CCAR::impl(Args args)
{
    orderManager.sensorMgr.checkCubeAR();
}

void ORDER_BDE::impl(Args args)
{
    //orderManager.sensorMgr.enableBasicDetection(true);
    //orderManager.sensorMgr.resetBasicBlocked();
}

void ORDER_BDD::impl(Args args)
{
    //orderManager.sensorMgr.enableBasicDetection(false);
    //orderManager.sensorMgr.resetBasicBlocked();
}

void ORDER_NH::impl(Args args)
{
    uint8_t id;
    int32_t x;
    uint32_t y, r;
    float angleHook, angleTolerance;
        id = (uint8_t) orderManager.parseInt(args[0]);
        x = (int32_t) orderManager.parseInt(args[1]);
        y = (uint32_t) orderManager.parseInt(args[2]);
        r = (uint32_t) orderManager.parseInt(args[3]);
        angleHook = orderManager.parseFloat(args[4]);
        angleTolerance = orderManager.parseFloat(args[5]);

        char hookOrder[RX_BUFFER_SIZE] = "";

        for (int i = 6; i < nbr_args; i++) {
            strcat(hookOrder, args[i]);
            strcat(hookOrder, " ");
        }
        hookOrder[RX_BUFFER_SIZE - 1] = '\0';

        orderManager.hookList.addHook(id, x, y, r, angleHook, angleTolerance, hookOrder);

        Serial.print("Ordre du hook: ");
        Serial.println(hookOrder);

        //TEST:
        Serial.println(orderManager.hookList.getHook(id).getOrder());
}

void ORDER_EH::impl(Args args)
{
    int hookId = orderManager.parseInt(args[0]);
    if(orderManager.hookList.hookWithId(hookId))
    {
        orderManager.hookList.enableHook((uint8_t)hookId); //Singe proof ?
    }
    else
    {
        orderManager.highLevel.printfln(DEBUG_HEADER,"ERREUR::Activation d'un hook inexistant");
    }
}

void ORDER_DH::impl(Args args) {
    int hookId = orderManager.parseInt(args[0]);

    if (orderManager.hookList.hookWithId(hookId)) {
        orderManager.hookList.disableHook((uint8_t) hookId); //Singe proof ?
    } else {
        orderManager.highLevel.printfln(DEBUG_HEADER, "ERREUR::Activation d'un hook inexistant");
    }
}

void ORDER_PTPDEMO::impl(Args args)
{
    orderManager.execute("goto 500 -700");
    delay(5000);
    orderManager.execute("goto 1000 -400");
    delay(5000);
    orderManager.execute("goto 750 100");
    delay(5000);
    orderManager.execute("goto 0 0");
}


void ORDER_PTPDEMOSEQ::impl(Args args)
{
    orderManager.execute("goto 500 -700 true");
    delay(5000);
    orderManager.execute("goto 1000 -400 true");
    delay(5000);
    orderManager.execute("goto 750 100 true");
    delay(5000);
    orderManager.execute("goto 0 0 true");
}