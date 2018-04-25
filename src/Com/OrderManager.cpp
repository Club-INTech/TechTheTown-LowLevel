#include "OrderManager.h"
#include "./Actuators/ActuatorValues.h"

OrderManager::OrderManager():
        sensorMgr(SensorMgr::Instance()),
        motionControlSystem(MotionControlSystem::Instance()),
        actuatorsMgr(ActuatorsMgr::Instance()),
        hookList(HookList()),
        orderData(OrderData()),
        highLevel(ComMgr::Instance())
{

    memset(readMessage, 0, RX_BUFFER_SIZE);
    isSendingUS = true;
    hooksEnabled = true;
    HLWaiting = false;
    basicDetectionTriggeredSent = false;
    basicDetectionFinishedSent = false;

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
        int8_t basicDetectionCheck = sensorMgr.isBasicBlocked();
        if(basicDetectionCheck) {
            if (!basicDetectionTriggeredSent) {
                highLevel.sendEvent("basicDetectionTriggered");
                basicDetectionTriggeredSent = true;
                basicDetectionFinishedSent = false;
            }
        }
        else if(!basicDetectionCheck) {
            if (!basicDetectionFinishedSent && basicDetectionTriggeredSent) {
                highLevel.sendEvent("basicDetectionFinished");
                basicDetectionTriggeredSent = false;
                basicDetectionFinishedSent = true;
            }

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
    char order[RX_BUFFER_SIZE];
//    #else                          *TANT QU'ON RESTE EN ORDRES
//    int order;                     *SOUS FORME STRINGS
//    #endif                        \*m'voyez
    char orderBuffer[RX_BUFFER_SIZE];
    strcpy(orderBuffer, orderToExecute);
    highLevel.acknowledge(orderBuffer);

    int8_t n_param = split(orderBuffer, orderData, SEPARATOR);		//Sépare l'ordre en plusieurs mots, n_param=nombre de paramètres

    if (n_param >= 0) {
        //#ifdef DEBUG
        strcpy(order, orderData.at(0));
        //#else
        //order = parseInt(orderData.at(0));
        //#endif //DEBUG

        if (!strcmp(order, "?"))			//Ping
        {
            highLevel.printfln(STD_HEADER,"0");
        }
        else if (!strcmp(order, "j"))       //Le HL attend l'activation du Jumper
        {
            HLWaiting = true;
        }
        else if (!strcmp(order, "f"))
        {
            highLevel.printfln(STD_HEADER,"%d",motionControlSystem.isMoving());
            highLevel.printfln(STD_HEADER,"%d",motionControlSystem.isMoveAbnormal());
        }
        else if (!strcmp(order, "?xyo"))		//Renvoie la position du robot (en mm et radians)
        {
            //highLevel.printfln(STD_HEADER,"%f,%f,%f", motionControlSystem.getX(), motionControlSystem.getY(), motionControlSystem.getAngleRadian());
            highLevel.printfln(STD_HEADER,"%f",motionControlSystem.getX());
            highLevel.printfln(STD_HEADER,"%f",motionControlSystem.getY());
            highLevel.printfln(STD_HEADER,"%f",motionControlSystem.getAngleRadian());
        }
        else if (!strcmp(order, "d"))		//Ordre de déplacement rectiligne (en mm)
        {
            if (n_param == 1) {
                int16_t deplacement = strtod(orderData.at(1), nullptr);
                highLevel.printfln(DEBUG_HEADER,"distance : %d",deplacement);
                motionControlSystem.orderTranslation(deplacement);
                sensorMgr.resetBasicBlocked();
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "t"))
        {
            if (n_param >= 1) {
                float angle;
                if(!strcmp(orderData.at(1),"pi"))
                {
                    angle = (float)PI;
                }
                else
                {
                    angle = strtof(orderData.at(1),nullptr);
                }
                highLevel.printfln(DEBUG_HEADER,"angle : %f", angle);
                MotionControlSystem::RotationWay rotationWay = MotionControlSystem::FREE;
                if(n_param == 2)
                {
                    if(!strcmp(orderData.at(2),"trigo"))
                    {
                        rotationWay = MotionControlSystem::TRIGO;
                    }
                    else if(!strcmp(orderData.at(2),"antitrigo"))
                    {
                        rotationWay = MotionControlSystem::ANTITRIGO;
                    }
                }
                motionControlSystem.orderRotation(angle, rotationWay);
                sensorMgr.resetBasicBlocked();
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "stop"))
        {
            motionControlSystem.stop();
            highLevel.printfln(DEBUG_HEADER,"A priori, je m'arrête");
        }
        else if(!strcmp(order, "emergencyStop"))
        {
            motionControlSystem.enableTranslationControl(false);
            motionControlSystem.enableRotationControl(false);

            motionControlSystem.setRawNullSpeed();

            highLevel.printfln(DEBUG_HEADER,"ARRET D'URGENCE EN COURS, DESACTIVATION ASSERV");
        }
        else if(!strcmp(order, "resumeEmergencyStop"))
        {
            motionControlSystem.stop();                             // On évite de revenir en arrière violamment
            motionControlSystem.enableTranslationControl(true);
            motionControlSystem.enableRotationControl(true);

            highLevel.printfln(DEBUG_HEADER,"REACTIVATION DE L'ASSERV APRES ARRET D'URGENCE");
        }

            /*			 __________________
            * 		   *|                  |*
            *		   *|   POS & VITESSE  |*
            *		   *|__________________|*
            */

        else if (!strcmp(order, "cx"))
        {
            if (n_param == 1) {
                float x = parseFloat(orderData.at(1));
                motionControlSystem.setX(x);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }

        }
        else if (!strcmp(order, "cy"))
        {
            if (n_param == 1) {
                float y = parseFloat(orderData.at(1));
                motionControlSystem.setY(y);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "co"))
        {
            if (n_param == 1) {
                float o = parseFloat(orderData.at(1));
                motionControlSystem.setOriginalAngle(o);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "cxyo"))
        {
            if (n_param == 3) {
                float x = parseFloat(orderData.at(1))
                , y = parseFloat(orderData.at(2))
                , o = parseFloat(orderData.at(3));

                motionControlSystem.setX(x);
                motionControlSystem.setY(y);
                motionControlSystem.setOriginalAngle(o);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }

        }
        else if (!strcmp(order, "ctv"))
        {
            if (n_param == 1) {
                float speed = parseFloat(orderData.at(1));
                motionControlSystem.setTranslationSpeed(speed);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }

        }
        else if (!strcmp(order, "crv"))
        {
            if (n_param == 1) {
                float speed = parseFloat(orderData.at(1));
                motionControlSystem.setRotationSpeed(speed);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }

        }
        else if (!strcmp(order, "ctrv"))
        {
            if (n_param == 2) {
                float transpeed = parseFloat(orderData.at(1));
                float rotspeed = parseFloat(orderData.at(2));
                motionControlSystem.setTranslationSpeed(transpeed);
                motionControlSystem.setRotationSpeed(rotspeed);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }

        }

        else if (!strcmp(order, "efm"))
        {
            motionControlSystem.enableForcedMovement(true);
        }
        else if (!strcmp(order, "dfm"))
        {
            motionControlSystem.enableForcedMovement(true);
        }

            /*			 ___________________
            * 		   *|                   |*
            *		   *|  ASSERVISSEMENTS  |*
            *		   *|___________________|*
            */

        else if (!strcmp(order, "ct0"))		//Désactiver l'asservissement en translation
        {
            motionControlSystem.enableTranslationControl(false);
            highLevel.printfln(DEBUG_HEADER,"non asservi en translation");
        }
        else if (!strcmp(order, "ct1"))		//Activer l'asservissement en translation
        {
            motionControlSystem.enableTranslationControl(true);
            highLevel.printfln(DEBUG_HEADER,"asservi en translation");
        }
        else if (!strcmp(order, "cr0"))		//Désactiver l'asservissement en rotation
        {
            motionControlSystem.enableRotationControl(false);
            highLevel.printfln(DEBUG_HEADER,"non asservi en rotation");
        }
        else if (!strcmp(order, "cr1"))		//Activer l'asservissement en rotation
        {
            motionControlSystem.enableRotationControl(true);
            highLevel.printfln(DEBUG_HEADER,"asservi en rotation");
        }
        else if (!strcmp(order, "cv0"))		//Désactiver l'asservissement en vitesse
        {
            motionControlSystem.enableSpeedControl(false);
            highLevel.printfln(DEBUG_HEADER,"non asservi en vitesse");
        }
        else if (!strcmp(order, "cv1"))		//Activer l'asservissement en vitesse
        {
            motionControlSystem.enableSpeedControl(true);
            highLevel.printfln(DEBUG_HEADER,"asservi en vitesse");
        }

            /*			 _________________________________
            * 		   *|                                 |*
            *		   *|			  Capteurs            |*
            *    	   *|_________________________________|*
            */

        else if (!strcmp(order, "seti2c"))
        {

        }

            /*			 ___________________________
            * 		   *|                           |*
            *		   *|					        |*
            *		   *|			DEBUG			|*
            *		   *|						    |*
            *		   *|___________________________|*
            */

        else if (!strcmp(order, "cod")) {
            highLevel.printfln(DEBUG_HEADER,"Gauche:");
            highLevel.printfln(DEBUG_HEADER,"%ld", motionControlSystem.getLeftTick());
            highLevel.printfln(DEBUG_HEADER,"Droite:");
            highLevel.printfln(DEBUG_HEADER,"%ld", motionControlSystem.getRightTick());
        }
        else if (!strcmp(order, "pfdebug"))
        {
            //highLevel.printfln(STD_HEADER,"%d", (int)motionControlSystem.getRightSpeed());
            //highLevel.printfln(STD_HEADER,"%d", (int)motionControlSystem.getRightMotorDir());
            //highLevel.printfln(STD_HEADER,"%d", (int)motionControlSystem.getRightSetPoint());
            //highLevel.printfln(STD_HEADER,"%d", (int)motionControlSystem.getRightMotorPWM());
            //highLevel.printfln(STD_HEADER,"%d", (int)motionControlSystem.getCodD());
        }
        else if (!strcmp(order, "rawpwm"))
        {
            uint8_t rawPWM = 255;
            if(n_param==1)
            {
                rawPWM = (uint8_t)parseInt(orderData.at(1));
            }
            motionControlSystem.orderRawPwm(Side::LEFT,rawPWM);
            motionControlSystem.orderRawPwm(Side::RIGHT,rawPWM);
        }
        else if (!strcmp(order, "getpwm")) {
            int32_t left, right;
            motionControlSystem.getPWMS(left, right);
            highLevel.printfln(DEBUG_HEADER,"PWMS: %ld - %ld", left, right);
        }
        else if (!strcmp(order, "errors")) {
            float leftProp, leftDer, leftInt, rightProp, rightDer, rightInt;
            motionControlSystem.getSpeedErrors(leftProp, leftInt, leftDer, rightProp, rightInt, rightDer);
            highLevel.printfln(DEBUG_HEADER,"Prop: %f - %f", leftProp, rightProp);
            highLevel.printfln(DEBUG_HEADER,"Deriv: %f - %f", leftDer, rightDer);
            highLevel.printfln(DEBUG_HEADER,"Integ: %f - %f", leftInt, rightInt);
        }
        else if (!strcmp(order, "rawspeed")) {
            int32_t leftsetpoint, rightsetpoint;

            //motionControlSystem.rawWheelSpeed(parseInt(orderData.at(1)), leftsetpoint, rightsetpoint);
            highLevel.printfln(DEBUG_HEADER,"Speed set");
            motionControlSystem.getSpeedSetpoints(leftsetpoint, rightsetpoint);
            highLevel.printfln(DEBUG_HEADER,"speed setpoints: %ld - %ld", leftsetpoint, rightsetpoint);
        }
        else if (!strcmp(order, "rawposdata"))
        {
            int32_t leftsetpoint, rightsetpoint;
            motionControlSystem.getSpeedSetpoints(leftsetpoint, rightsetpoint);
            Serial.print(motionControlSystem.getX());
            Serial.print(",");
            Serial.print(motionControlSystem.getY());
            Serial.print(",");
            Serial.print(motionControlSystem.getAngleRadian());
            Serial.print(",");
            Serial.print(motionControlSystem.getLeftSpeed());
            Serial.print(",");
            Serial.print(leftsetpoint);
            Serial.print(",");
            Serial.print(motionControlSystem.getRightSpeed());
            Serial.print(",");
            Serial.println(rightsetpoint);
            //int32_t right, left;
            //motionControlSystem.getPWMS(left,right);
            //Serial.println(right);
            //float rotaProp, rotaDer, rotaInt;
            //motionControlSystem.getRotationErrors(rotaProp, rotaInt, rotaDer);
            //Serial.println(rotaInt);
        }

            /*			 ___________________________
            * 		   *|                           |*
            *		   *|         MONTLHERY         |*
            *		   *|   DEPLACEMENT ET ROTATION |*
            *		   *|    SANS ASSERVISSEMENT    |*
            *		   *|___________________________|*
            */

        else if (!strcmp(order, "montlhery"))
        {
            motionControlSystem.enableTranslationControl(false);
            motionControlSystem.enableRotationControl(false);
            motionControlSystem.enableForcedMovement(true);
        }
        else if (!strcmp(order, "av"))
        {
            motionControlSystem.setRawPositiveTranslationSpeed();  // definit la consigne max de vitesse de translation envoi�e au PID (trap�ze)
            // déplacement vers l'avant avec asservissement
        }

        else if (!strcmp(order, "rc"))
        {
            motionControlSystem.setRawNegativeTranslationSpeed();  // definit la consigne max de vitesse de translation envoi�e au PID (trap�ze)
            // déplacement vers l'arfrière avec asservissement
        }

        else if (!strcmp(order, "td"))
        {
            motionControlSystem.setRawNegativeRotationSpeed();     // definit la consigne max de vitesse de rotation envoi�e au PID (trap�ze)
            // rotation sens antitrigo avec asservissement
        }

        else if (!strcmp(order, "tg"))
        {
            motionControlSystem.setRawPositiveRotationSpeed();     // definit la consigne max de vitesse de rotation envoi�e au PID (trap�ze)
            // rotation sens antitrigo avec asservissement
        }

        else if (!strcmp(order, "sstop"))                            // Stoppe le robot
        {
            motionControlSystem.setRawNullSpeed();
        }

            /*			 _________________________________
            * 		   *|                                 |*
            *		   *|CONSTANTES D'ASSERV (pour le PID)|*
            *    	   *|_________________________________|*
            */

        else if (!strcmp(order, "toggle"))
        {
            motionControlSystem.translation = !motionControlSystem.translation;   //Bascule entre le réglage d'asserv en translation et en rotation
            if (motionControlSystem.translation)
                highLevel.printfln(DEBUG_HEADER,"reglage de la translation");
            else
                highLevel.printfln(DEBUG_HEADER,"reglage de la rotation");
        }
        else if (!strcmp(order, "display")) //affiche les paramètres des PID des différentes asserv (translation, rotation, vitesse à droite, vitesse à gauche)
        {
            float
                    kp_t, ki_t, kd_t,	  // Translation
                    kp_r, ki_r, kd_r,	  // Rotation
                    kp_g, ki_g, kd_g,	  // Vitesse gauche
                    kp_d, ki_d, kd_d;	  // Vitesse droite
            motionControlSystem.getTranslationTunings(kp_t, ki_t, kd_t);
            motionControlSystem.getRotationTunings(kp_r, ki_r, kd_r);
            motionControlSystem.getLeftSpeedTunings(kp_g, ki_g, kd_g);
            motionControlSystem.getRightSpeedTunings(kp_d, ki_d, kd_d);
            highLevel.printfln(DEBUG_HEADER,"trans : kp= %g ; ki= %g ; kd= %g", kp_t, ki_t, kd_t);
            highLevel.printfln(DEBUG_HEADER,"rot   : kp= %g ; ki= %g ; kd= %g", kp_r, ki_r, kd_r);
            highLevel.printfln(DEBUG_HEADER,"gauche: kp= %g ; ki= %g ; kd= %g", kp_g, ki_g, kd_g);
            highLevel.printfln(DEBUG_HEADER,"droite: kp= %g ; ki= %g ; kd= %g", kp_d, ki_d, kd_d);
        }
            // ***********  Paramètres du PID pour l'asserv en position (TRANSLATION)  ***********
        else if (!strcmp(order, "kpt"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"kp_trans ?");
                motionControlSystem.getTranslationTunings(kp, ki, kd);
                kp = parseFloat(orderData.at(1));

                motionControlSystem.setTranslationTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"kp_trans = %g", kp);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kdt"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"kd_trans ?");
                motionControlSystem.getTranslationTunings(kp, ki, kd);
                kd = parseFloat(orderData.at(1));

                motionControlSystem.setTranslationTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"kd_trans = %g", kd);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kit"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"ki_trans ?");
                motionControlSystem.getTranslationTunings(kp, ki, kd);
                ki = parseFloat(orderData.at(1));

                motionControlSystem.setTranslationTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"ki_trans = %g", ki);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }

            // ***********  Paramètres du PID pour l'asserv en ROTATION  ***********
        else if (!strcmp(order, "kpr"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"kp_rot ?");
                motionControlSystem.getRotationTunings(kp, ki, kd);
                kp = parseFloat(orderData.at(1));

                motionControlSystem.setRotationTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"kp_rot = %g", kp);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kir"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"ki_rot ?");
                motionControlSystem.getRotationTunings(kp, ki, kd);
                ki = parseFloat(orderData.at(1));

                motionControlSystem.setRotationTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"ki_rot = %g", ki);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kdr"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"kd_rot ?");
                motionControlSystem.getRotationTunings(kp, ki, kd);
                kd = parseFloat(orderData.at(1));

                motionControlSystem.setRotationTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"kd_rot = %g", kd);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }

            // ***********  Paramètres du PID pour l'asserv en vitesse à gauche  ***********
        else if (!strcmp(order, "kpg"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"kp_gauche ?");
                motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
                kp = parseFloat(orderData.at(1));

                motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"kp_gauche = %g", kp);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kig"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"ki_gauche ?");
                motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
                ki = parseFloat(orderData.at(1));

                motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"ki_gauche = %g", ki);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kdg"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"kd_gauche ?");
                motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
                kd = parseFloat(orderData.at(1));

                motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"kd_gauche = %g", kd);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }

            // ***********  Paramètres du PID pour l'asserv en vitesse à droite ****************
        else if (!strcmp(order, "kpd"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"kp_droite ?");
                motionControlSystem.getRightSpeedTunings(kp, ki, kd);
                kp = parseFloat(orderData.at(1));

                motionControlSystem.setRightSpeedTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"kp_droite = %g", kp);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kid"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"ki_droite ?");
                motionControlSystem.getRightSpeedTunings(kp, ki, kd);
                ki = parseFloat(orderData.at(1));

                motionControlSystem.setRightSpeedTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"ki_droite = %g", ki);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kdd"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln(STD_HEADER,"kd_droite ?");
                motionControlSystem.getRightSpeedTunings(kp, ki, kd);
                kd = parseFloat(orderData.at(1));

                motionControlSystem.setRightSpeedTunings(kp, ki, kd);
                highLevel.printfln(DEBUG_HEADER,"kd_droite = %g", kd);
            }
            else {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }

            /*			 _________________________________
            * 		   *|                                 |*
            *		   *|			 Actionneurs          |*
            *    	   *|_________________________________|*
            */

        else if (!strcmp(order, "AXm"))
        {
            if (n_param == 2)
            {
                actuatorsMgr.movAX12(parseInt(orderData.at(1)),(uint16_t)parseInt(orderData.at(2)));
            }
            else
            {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "AXGm"))
        {
            if(n_param == 2)
            {
                actuatorsMgr.movAX12G((unsigned int)parseInt(orderData.at(1)),(uint16_t)parseInt(orderData.at(2)));
            }
            else
            {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "AXs"))
        {
            if(n_param == 2)
            {
                actuatorsMgr.setAX12Speed(parseInt(orderData.at(1)),(uint16_t)parseInt(orderData.at(2)));
            }
            else
            {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "AXGs"))
        {
            if(n_param == 2)
            {
                actuatorsMgr.setAX12GSpeed((unsigned int)parseInt(orderData.at(1)),(uint16_t)parseInt(orderData.at(2)));
            }
            else
            {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
        }
            /*			 _________________________________
             * 		   *|                                 |*
             *		   *|     Actionneurs spécifiques     |*
             *    	   *|_________________________________|*
             */
        /*
         * BRAS
         */
        else if(!strcmp(order, "blbAvbei"))
        {
            actuatorsMgr.movAX12G(frontArmGroup_ID,frontLowBee);
        }
        else if (!strcmp(order, "blbAv"))
        {
            actuatorsMgr.movAX12G(frontArmGroup_ID,frontLowCubes);
        }
        else if (!strcmp(order, "rlbAv"))
        {
            actuatorsMgr.movAX12G(frontArmGroup_ID,frontFolded);
        }
        else if (!strcmp(order, "blbArbei"))
        {
            actuatorsMgr.movAX12G(backArmGroup_ID,backLowBee);
        }
        else if (!strcmp(order, "blbAr"))
        {
            actuatorsMgr.movAX12G(backArmGroup_ID,backLowCubes);
        }
        else if (!strcmp(order, "rlbAr"))
        {
            actuatorsMgr.movAX12G(backArmGroup_ID,backFolded);
        }
        /*
         * PORTES
         */
        else if (!strcmp(order, "flpAv"))
        {
            actuatorsMgr.movAX12(frontDoor_ID,frontDoorClosed);
        }
        else if (!strcmp(order, "olpAv"))
        {
            actuatorsMgr.movAX12(frontDoor_ID,frontDoorOpen);
        }
        else if (!strcmp(order, "flpAr"))
        {
            actuatorsMgr.movAX12(backDoor_ID,backDoorClosed);
        }
        else if (!strcmp(order, "olpAr"))
        {
            actuatorsMgr.movAX12(backDoor_ID,backDoorOpen);
        }
        else if (!strcmp(order, "olpAvp"))
        {
            actuatorsMgr.movAX12(frontDoor_ID,frontDoorClosed+doorSmallOpeningAngle);
        }
        else if (!strcmp(order, "olpArp"))
        {
            actuatorsMgr.movAX12(backDoor_ID,backDoorClosed-doorSmallOpeningAngle);
        }
        /*
         * POMPE
         */
        else if (!strcmp(order, "alp"))
        {
            actuatorsMgr.togglePumpState(true);
        }
        else if (!strcmp(order, "dlp"))
        {
            actuatorsMgr.togglePumpState(false);
        }
        /*
         * ELECTROVANNES
         */
        else if (!strcmp(order, "aeAv"))
        {
            actuatorsMgr.setElecVanneAV(true);
        }
        else if (!strcmp(order, "deAv"))
        {
            actuatorsMgr.setElecVanneAV(false);
        }
        else if (!strcmp(order, "aeAr"))
        {
            actuatorsMgr.setElecVanneAR(true);
        }
        else if (!strcmp(order, "deAr"))
        {
            actuatorsMgr.setElecVanneAR(false);
        }

            /*			 _________________________________
            * 		   *|                                 |*
            *		   *|			  Capteurs            |*
            *    	   *|_________________________________|*
            */
            
        else if (!strcmp(order, "sus"))		//Switch d'envois périodiques de données des capteurs
        {
            if(isSendingUS)
            {
                highLevel.printfln(DEBUG_HEADER,"Désactivation US");
            }
            else
            {
                highLevel.printfln(DEBUG_HEADER,"Activation US");
            }
            if(n_param == 0)
            {
                isSendingUS = !isSendingUS;
            }
            else if(n_param == 1)
            {
                isSendingUS = !strcmp(orderData.at(1), "on");
            }
            else
            {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
                highLevel.printfln(DEBUG_HEADER,"Pas de changement");
            }
        }
        else if(!strcmp(order, "ccAv"))
        {
            sensorMgr.checkCubeAV();
        }
        else if(!strcmp(order, "ccAr"))
        {
            sensorMgr.checkCubeAR();
        }
        else if(!strcmp(order, "bde"))
        {
            sensorMgr.enableBasicDetection(true);
            sensorMgr.resetBasicBlocked();
        }
        else if(!strcmp(order, "bdd"))
        {
            sensorMgr.enableBasicDetection(false);
            sensorMgr.resetBasicBlocked();
        }
            /*			 _________________________________
            * 		   *|                                 |*
            *		   *|			   HOOKS	          |*
            *    	   *|_________________________________|*
            */

        else if(!strcmp(order, "nh")){
            uint8_t id;
            int32_t x;
            uint32_t y, r;
            float angleHook,angleTolerance;

            if (n_param < 7)
            {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Paramètres incorrects");
            }
            if (n_param >=7)
            {

                id = (uint8_t)parseInt(orderData.at(1));
                x = (int32_t)parseInt(orderData.at(2));
                y = (uint32_t)parseInt(orderData.at(3));
                r = (uint32_t)parseInt(orderData.at(4));
                angleHook = parseFloat(orderData.at(5));
                angleTolerance = parseFloat(orderData.at(6));

                char hookOrder[RX_BUFFER_SIZE] = "";

                for (int i = 7; i < n_param + 1; i++) {
                    strcat(hookOrder, orderData.at(i));
                    strcat(hookOrder, " ");
                }
                hookOrder[RX_BUFFER_SIZE - 1] = '\0';

                hookList.addHook(id, x, y, r, angleHook, angleTolerance, hookOrder);

                Serial.print("Ordre du hook: ");
                Serial.println(hookOrder);

                //TEST:
                Serial.println(hookList.getHook(id).getOrder());
            }

        }
        else if (!strcmp(order, "eh")) {
            int hookId = parseInt(orderData.at(1));

            if(hookList.hookWithId(hookId))
            {
                hookList.enableHook((uint8_t)hookId); //Singe proof ?
            }
            else
            {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Activation d'un hook inexistant");
            }

        }
        else if (!strcmp(order, "dh")) {
            int hookId = parseInt(orderData.at(1));

            if(hookList.hookWithId(hookId))
            {
                hookList.disableHook((uint8_t)hookId); //Singe proof ?
            }
            else
            {
                highLevel.printfln(DEBUG_HEADER,"ERREUR::Activation d'un hook inexistant");
            }
        }

            /*			 _________________________________
             * 		   *|                                 |*
             *		   *|			   RANDOM	          |*
             *    	   *|_________________________________|*
              */

        else if (!strcmp(order, "demo"))
        {
            motionControlSystem.orderTranslation(400);
            delay(3000);
            motionControlSystem.orderRotation(-1.6f, MotionControlSystem::FREE);
            delay(2000);
            motionControlSystem.orderTranslation(200);
            delay(2000);
            actuatorsMgr.movAX12(3,100);
            delay(1000);
            motionControlSystem.orderTranslation(-200);
            delay(2000);
            motionControlSystem.orderRotation(0, MotionControlSystem::FREE);
            actuatorsMgr.movAX12(3,10);
            delay(2000);
            motionControlSystem.orderTranslation(-400);
            actuatorsMgr.movAX12G(0,95);
            delay(2000);
            actuatorsMgr.movAX12G(0,177);
        }

        else
        {
            highLevel.printfln(STD_HEADER,"ordre inconnu");
            highLevel.printfln(DEBUG_HEADER,"T'es un déchêt");
        }
        /*			 __________________
        * 		   *|                  |*
        *		   *|  COMM. DE BASE   |*
        *		   *|__________________|*
        */


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
