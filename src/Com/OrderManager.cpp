#include "OrderManager.h"

OrderManager::OrderManager():	motionControlSystem(MotionControlSystem::Instance()),
								sensorMgr(SensorMgr::Instance()),
								actuatorsMgr(ActuatorsMgr::Instance()),
								hookList(HookList()),
								orderData(OrderData()),
								#if DEBUG
									highLevel(SerialMgr::Instance())
								#else
									highLevel(EthernetMgr::Instance())
								#endif
{
	memset(readMessage, 0, RX_BUFFER_SIZE);
	isSendingUS = false;
	hooksEnabled = true;
	highLevel.log("Communications ready");
}

void OrderManager::communicate() {

	if (highLevel.read(readMessage)) {
		execute(readMessage);
	}

	memset(readMessage, 0, RX_BUFFER_SIZE);


    // Active des timer qui s'activent à intervalle régulier pour le mouvement et les hooks
	static Metro checkMovement = Metro(10);
    static Metro checkHooksTimer = Metro(20);


	if (checkMovement.check())
	{
		if (!motionControlSystem.sentMoveAbnormal() && motionControlSystem.isMoveAbnormal()) {//Si on est bloqué et qu'on n'a pas encore prévenu
			motionControlSystem.setMoveAbnormalSent(true);
            //TODO prévoir le cas quand on ne peut pas bouger car on a détecté un obstable (envoyer "unableToMove o")
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


	// Code de compilé seulement si on utilise l'ethernet
    // Gère l'envoie de la position tout les F_ENV_POS
	#if !DEBUG
    static Metro sendPos = Metro(F_ENV_POS);
	    if (sendPos.check()) {
			if (motionControlSystem.isMoving()) {
                float posToSend[3]={motionControlSystem.getX(), motionControlSystem.getY(), motionControlSystem.getAngleRadian()};
				highLevel.sendPosition(posToSend);
                motionControlSystem.setPreviousIsMoving(true);
            } else {
                if (motionControlSystem.previousIsMoving()){
                    highLevel.sendEvent("stoppedMoving");
                }
                motionControlSystem.setPreviousIsMoving(false);
            }
		}

    #endif

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
	highLevel.log("Message recu: %s", orderBuffer);

	int8_t n_param = split(orderBuffer, orderData, SEPARATOR);		// Sépare l'ordre en plusieurs mots, n_param=nombre de paramètres

	if (n_param >= 0) {
		//#ifdef DEBUG
		strcpy(order, orderData.at(0));
        //#else
        //order = parseInt(orderData.at(0));
        //#endif //DEBUG

        if (!strcmp(order, "?"))			        // Ping
        {
            highLevel.printfln("0");
        }
        else if (!strcmp(order, "sus"))		        // Switch d'envois périodiques de données des capteurs
        {
            highLevel.log("Activation US");
            isSendingUS = !isSendingUS;
        }
        else if (!strcmp(order, "f"))
        {
            highLevel.println(motionControlSystem.isMoving());
            highLevel.println(motionControlSystem.isMoveAbnormal());
        }
        else if (!strcmp(order, "?xyo"))	        // Renvoie la position du robot (en mm et radians)
        {
            //highLevel.printfln("%f,%f,%f", motionControlSystem.getX(), motionControlSystem.getY(), motionControlSystem.getAngleRadian());
            highLevel.println(motionControlSystem.getX());
            highLevel.println(motionControlSystem.getY());
            highLevel.println(motionControlSystem.getAngleRadian());
        }
        else if (!strcmp(order, "d"))		        // Ordre de déplacement rectiligne (en mm)
        {
            if (n_param == 1) {
                int16_t deplacement = strtod(orderData.at(1), nullptr);
                highLevel.log("distance : %d", deplacement);
                motionControlSystem.orderTranslation(deplacement);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "t"))               // Ordre de rotation (en rad)
        {
            if (n_param == 1) {
                float angle;
                if(!strcmp(orderData.at(1),"pi"))
                {
                    angle = (float)PI;
                }
                else
                {
                    angle = strtof(orderData.at(1),nullptr);
                }
                highLevel.log("angle : %f", angle);
                motionControlSystem.orderRotation(angle, MotionControlSystem::FREE);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "stop"))
        {
            motionControlSystem.stop();
            highLevel.log("A priori, je m'arrête");
        }

            /*			 __________________
            * 		   *|                  |*
            *		   *|   POS & VITESSE  |*
            *		   *|__________________|*
            */

        else if (!strcmp(order, "cx"))              // Change la position en X du robot
        {
            if (n_param == 1) {
                float x = parseFloat(orderData.at(1));
                motionControlSystem.setX(x);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }

        }
        else if (!strcmp(order, "cy"))              // Change la position en Y du robot
        {
            if (n_param == 1) {
                float y = parseFloat(orderData.at(1));
                motionControlSystem.setY(y);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "co"))              // Change la position en θ du robot
        {
            if (n_param == 1) {
                float o = parseFloat(orderData.at(1));
                motionControlSystem.setOriginalAngle(o);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "cxyo"))            // Change la position en X,Y,θ
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
                highLevel.log("ERREUR::Paramètres incorrects");
            }

        }
        else if (!strcmp(order, "ctv"))             // Change la vitesse de translation
        {
            if (n_param == 1) {
                float speed = parseFloat(orderData.at(1));
                motionControlSystem.setTranslationSpeed(speed);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }

        }
        else if (!strcmp(order, "crv"))             // Change la vitesse de rotation
        {
            if (n_param == 1) {
                float speed = parseFloat(orderData.at(1));
                motionControlSystem.setRotationSpeed(speed);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }

        }
        else if (!strcmp(order, "ctrv"))            // Change les vitesses de rotation et de translation
        {
            if (n_param == 2) {
                float transpeed = parseFloat(orderData.at(1));
                float rotspeed = parseFloat(orderData.at(2));
                motionControlSystem.setTranslationSpeed(transpeed);
                motionControlSystem.setRotationSpeed(rotspeed);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }

        }

        else if (!strcmp(order, "efm"))             // Active le mouvement forcé
        {
            motionControlSystem.enableForcedMovement();
        }
        else if (!strcmp(order, "dfm"))             // Désactive le mouvement forcé
        {
            motionControlSystem.disableForcedMovement();
        }

            /*			 ___________________
            * 		   *|                   |*
            *		   *|  ASSERVISSEMENTS  |*
            *		   *|___________________|*
            */

        else if (!strcmp(order, "ct0"))		        // Désactiver l'asservissement en translation
        {
            motionControlSystem.enableTranslationControl(false);
            highLevel.log("non asservi en translation");
        }
        else if (!strcmp(order, "ct1"))		        // Activer l'asservissement en translation
        {
            motionControlSystem.enableTranslationControl(true);
            highLevel.log("asservi en translation");
        }
        else if (!strcmp(order, "cr0"))		        // Désactiver l'asservissement en rotation
        {
            motionControlSystem.enableRotationControl(false);
            highLevel.log("non asservi en rotation");
        }
        else if (!strcmp(order, "cr1"))		        // Activer l'asservissement en rotation
        {
            motionControlSystem.enableRotationControl(true);
            highLevel.log("asservi en rotation");
        }
        else if (!strcmp(order, "cv0"))		        // Désactiver l'asservissement en vitesse
        {
            motionControlSystem.enableSpeedControl(false);
            highLevel.log("non asservi en vitesse");
        }
        else if (!strcmp(order, "cv1"))		        // Activer l'asservissement en vitesse
        {
            motionControlSystem.enableSpeedControl(true);
            highLevel.log("asservi en vitesse");
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

        else if (!strcmp(order, "cod"))             // Renvoie les ticks de codeuse
        {
            highLevel.log("Gauche:");
            highLevel.log("%ld", motionControlSystem.getLeftTick());
            highLevel.log("Droite:");
            highLevel.log("%ld", motionControlSystem.getRightTick());
        }
        else if (!strcmp(order, "pfdebug"))         // Fonction de debug de PF
        {
            //highLevel.printfln("%d", (int)motionControlSystem.getRightSpeed());
            //highLevel.printfln("%d", (int)motionControlSystem.getRightMotorDir());
            //highLevel.printfln("%d", (int)motionControlSystem.getRightSetPoint());
            //highLevel.printfln("%d", (int)motionControlSystem.getRightMotorPWM());
            //highLevel.printfln("%d", (int)motionControlSystem.getCodD());
        }
        else if (!strcmp(order, "rawpwm"))          // Envoie un PWM brut aux deux moteurs
        {
            uint8_t rawPWM = 255;
            if(n_param==1)
            {
                rawPWM = (uint8_t)parseInt(orderData.at(1));
            }
            motionControlSystem.orderRawPwm(Side::LEFT,rawPWM);
            motionControlSystem.orderRawPwm(Side::RIGHT,rawPWM);
        }
        else if (!strcmp(order, "getpwm"))          // Retourne les PWM actuels des moteurs
        {
            int32_t left, right;
            motionControlSystem.getPWMS(left, right);
            highLevel.log("PWMS: %d - %d", left, right);
        }
        else if (!strcmp(order, "errors"))          // Retourne les erreurs des PIDs en vitesse
        {
            float leftProp, leftDer, leftInt, rightProp, rightDer, rightInt;
            motionControlSystem.getSpeedErrors(leftProp, leftInt, leftDer, rightProp, rightInt, rightDer);
            highLevel.log("Prop: %f - %f", leftProp, rightProp);
            highLevel.log("Deriv: %f - %f", leftDer, rightDer);
            highLevel.log("Integ: %f - %f", leftInt, rightInt);
        }
        else if (!strcmp(order, "rawspeed"))        // Retourne la vitesse des codeuses
        {
            int32_t leftsetpoint, rightsetpoint;

            motionControlSystem.rawWheelSpeed((uint16_t)parseInt(orderData.at(1)),leftsetpoint,rightsetpoint);
            highLevel.log("Speed set");
            motionControlSystem.getSpeedSetpoints(leftsetpoint, rightsetpoint);
            highLevel.log("speed setpoints: %ld - %ld", leftsetpoint, rightsetpoint);
        }
        else if (!strcmp(order, "rawposdata"))      // Retourne des infos de position (X,Y,θ)
        {                                           // Ainsi que les vitesses des codeuses et les vitesses cibles
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

        else if (!strcmp(order, "monthlery"))       // Prépare le robot pour un contrôle direct (Avec joystick par ex)
        {
            motionControlSystem.enableTranslationControl(false);
            motionControlSystem.enableRotationControl(false);
            motionControlSystem.enableForcedMovement();
        }
        else if (!strcmp(order, "av"))              // Avance
        {
            motionControlSystem.setRawPositiveTranslationSpeed();  // Définit la consigne max de vitesse de translation envoyée au PID (trapèze)
        }                                                          // Déplacement vers l'avant avec asservissement

        else if (!strcmp(order, "rc"))              // Recule
        {
            motionControlSystem.setRawNegativeTranslationSpeed();  // Définit la consigne max de vitesse de translation envoyée au PID (trapèze)
        }                                                          // Déplacement vers l'arrière avec asservissement

        else if (!strcmp(order, "td"))              // Tourne à droite
        {
            motionControlSystem.setRawNegativeRotationSpeed();     // Définit la consigne max de vitesse de rotation envoyée au PID (trapèze)
                                                                   // Rotation sens antitrigo avec asservissement
        }

        else if (!strcmp(order, "tg"))              // Tourne à gauche
        {
            motionControlSystem.setRawPositiveRotationSpeed();     // Définit la consigne max de vitesse de rotation envoyée au PID (trapèze)
                                                                   // Rotation sens antitrigo avec asservissement
        }

        else if (!strcmp(order, "sstop"))
        {
            motionControlSystem.setRawNullSpeed();
        }

            /*			 _________________________________
            * 		   *|                                 |*
            *		   *|CONSTANTES D'ASSERV (pour le PID)|*
            *    	   *|_________________________________|*
            */

        else if (!strcmp(order, "toggle"))          // Change le réglage d'asserv' entre translation et rotation
        {
            motionControlSystem.translation = !motionControlSystem.translation;
            if (motionControlSystem.translation)
                highLevel.log("reglage de la translation");
            else
                highLevel.log("reglage de la rotation");
        }
        else if (!strcmp(order, "display"))         // Affiche les paramètres des PID des différentes asserv (translation, rotation, vitesse à droite, vitesse à gauche)
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

            highLevel.log("trans : kp= %g ; ki= %g ; kd= %g", kp_t, ki_t, kd_t);
            highLevel.log("rot   : kp= %g ; ki= %g ; kd= %g", kp_r, ki_r, kd_r);
            highLevel.log("gauche: kp= %g ; ki= %g ; kd= %g", kp_g, ki_g, kd_g);
            highLevel.log("droite: kp= %g ; ki= %g ; kd= %g", kp_d, ki_d, kd_d);
        }

            /* Paramètres du PID pour l'asserv en position (TRANSLATION) */
        else if (!strcmp(order, "kpt"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("kp_trans ?");
                motionControlSystem.getTranslationTunings(kp, ki, kd);
                kp = parseFloat(orderData.at(1));

                motionControlSystem.setTranslationTunings(kp, ki, kd);
                highLevel.log("kp_trans = %g", kp);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kdt"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("kd_trans ?");
                motionControlSystem.getTranslationTunings(kp, ki, kd);
                kd = parseFloat(orderData.at(1));

                motionControlSystem.setTranslationTunings(kp, ki, kd);
                highLevel.log("kd_trans = %g", kd);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kit"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("ki_trans ?");
                motionControlSystem.getTranslationTunings(kp, ki, kd);
                ki = parseFloat(orderData.at(1));

                motionControlSystem.setTranslationTunings(kp, ki, kd);
                highLevel.log("ki_trans = %g", ki);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }

            /* Paramètres du PID pour l'asserv en ROTATION */
        else if (!strcmp(order, "kpr"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("kp_rot ?");
                motionControlSystem.getRotationTunings(kp, ki, kd);
                kp = parseFloat(orderData.at(1));

                motionControlSystem.setRotationTunings(kp, ki, kd);
                highLevel.log("kp_rot = %g", kp);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kir"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("ki_rot ?");
                motionControlSystem.getRotationTunings(kp, ki, kd);
                ki = parseFloat(orderData.at(1));

                motionControlSystem.setRotationTunings(kp, ki, kd);
                highLevel.log("ki_rot = %g", ki);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kdr"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("kd_rot ?");
                motionControlSystem.getRotationTunings(kp, ki, kd);
                kd = parseFloat(orderData.at(1));

                motionControlSystem.setRotationTunings(kp, ki, kd);
                highLevel.log("kd_rot = %g", kd);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }

            /* Paramètres du PID pour l'asserv en vitesse à gauche */
        else if (!strcmp(order, "kpg"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("kp_gauche ?");
                motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
                kp = parseFloat(orderData.at(1));

                motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
                highLevel.log("kp_gauche = %g", kp);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kig"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("ki_gauche ?");
                motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
                ki = parseFloat(orderData.at(1));

                motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
                highLevel.log("ki_gauche = %g", ki);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kdg"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("kd_gauche ?");
                motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
                kd = parseFloat(orderData.at(1));

                motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
                highLevel.log("kd_gauche = %g", kd);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }

            /*  Paramètres du PID pour l'asserv en vitesse à droite */
        else if (!strcmp(order, "kpd"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("kp_droite ?");
                motionControlSystem.getRightSpeedTunings(kp, ki, kd);
                kp = parseFloat(orderData.at(1));

                motionControlSystem.setRightSpeedTunings(kp, ki, kd);
                highLevel.log("kp_droite = %g", kp);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kid"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("ki_droite ?");
                motionControlSystem.getRightSpeedTunings(kp, ki, kd);
                ki = parseFloat(orderData.at(1));

                motionControlSystem.setRightSpeedTunings(kp, ki, kd);
                highLevel.log("ki_droite = %g", ki);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "kdd"))
        {
            if (n_param == 1) {
                float kp, ki, kd;
                highLevel.printfln("kd_droite ?");
                motionControlSystem.getRightSpeedTunings(kp, ki, kd);
                kd = parseFloat(orderData.at(1));

                motionControlSystem.setRightSpeedTunings(kp, ki, kd);
                highLevel.log("kd_droite = %g", kd);
            }
            else {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }

            /*			 _________________________________
            * 		   *|                                 |*
            *		   *|			 Actionneurs          |*
            *    	   *|_________________________________|*
            */

        else if (!strcmp(order, "AXm"))             // Change l'angle cible d'un AX12
        {
            if (n_param == 2)
            {
                actuatorsMgr.movAX12(parseInt(orderData.at(1)),(uint16_t)parseInt(orderData.at(2)));
            }
            else
            {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "AXGm"))            // Change l'angle cible d'un groupe d'AX12
        {
            if(n_param == 2)
            {
                actuatorsMgr.movAX12G((unsigned int)parseInt(orderData.at(1)),(uint16_t)parseInt(orderData.at(2)));
            }
            else
            {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "AXs"))             // Change la vitesse d'un AX12
        {
            if(n_param == 2)
            {
                actuatorsMgr.setAX12Speed(parseInt(orderData.at(1)),(uint16_t)parseInt(orderData.at(2)));
            }
            else
            {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
        }
        else if (!strcmp(order, "AXGs"))            // Change la vitesse d'un groupe d'AX12
        {
            if(n_param == 2)
            {
                actuatorsMgr.setAX12GSpeed((unsigned int)parseInt(orderData.at(1)),(uint16_t)parseInt(orderData.at(2)));
            }
            else
            {
                highLevel.log("ERREUR::Paramètres incorrects");
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
        else if (!strcmp(order, "blbAv"))           // Baisse le bras avant
        {
            actuatorsMgr.movAX12G(0,95);
        }
        else if (!strcmp(order, "rlbAv"))           // Relève le bras avant
        {
            actuatorsMgr.movAX12G(0,185);
        }
        else if (!strcmp(order, "blbAr"))           // Baisse le bras arrière
        {
            actuatorsMgr.movAX12G(1,202);
        }
        else if (!strcmp(order, "rlbAr"))           // Relève le bras arrière
        {
            actuatorsMgr.movAX12G(1,120);
        }
        else if (!strcmp(order, "albl"))            // Active les bras latéraux
        {
            actuatorsMgr.movAX12G(1,60);
            delay(1000);
            actuatorsMgr.movAX12G(1,240);
        }
        /*
         * PORTES
         */
        else if (!strcmp(order, "flpAv"))           // Ferme la porte avant
        {
            actuatorsMgr.movAX12(3,150);
        }
        else if (!strcmp(order, "olpAv"))           // Ouvre la porte avant
        {
            actuatorsMgr.movAX12(3,240);
        }
        else if (!strcmp(order, "flpAr"))           // Ferme la porte arrière
        {
            actuatorsMgr.movAX12(6,240);
        }
        else if (!strcmp(order, "olpAr"))           // Ouvre la porte arrière
        {
            actuatorsMgr.movAX12(6,150);
        }
        else if (!strcmp(order, "tlpAv"))           // Gigotte la porte avant
        {
            actuatorsMgr.movAX12(3,235);
            delay(500);
            actuatorsMgr.movAX12(3,240);
        }
        else if (!strcmp(order, "tlpAr"))           // Gigotte la porte arrière
        {
            actuatorsMgr.movAX12(6,235);
            delay(500);
            actuatorsMgr.movAX12(6,240);
        }
        /*
         * POMPE
         */
        else if (!strcmp(order, "alp"))             // Active la pompe
        {
            actuatorsMgr.togglePumpState(true);
        }
        else if (!strcmp(order, "dlp"))             // Désactive la pompe
        {
            actuatorsMgr.togglePumpState(false);
        }
        /*
         * ELECTROVANNES
         */
        else if (!strcmp(order, "aeAv"))            // Active l'électrovanne avant
        {
            actuatorsMgr.setElecVanneAV(true);
        }
        else if (!strcmp(order, "deAv"))            // Désactive l'électrovanne avant
        {
            actuatorsMgr.setElecVanneAV(false);
        }
        else if (!strcmp(order, "aeAr"))            // Active l'électrovanne arrière
        {
            actuatorsMgr.setElecVanneAR(true);
        }
        else if (!strcmp(order, "deAr"))            // Désactive l'électrovanne arrière
        {
            actuatorsMgr.setElecVanneAR(false);
        }

            /*			 _________________________________
            * 		   *|                                 |*
            *		   *|			   HOOKS	          |*
            *    	   *|_________________________________|*
            */

        else if(!strcmp(order, "nh"))               // Créé un nouveau hook
        {
            uint8_t id;
            uint32_t x, y, r;
            float angleHook,angleTolerance;

            if (n_param < 7)
            {
                highLevel.log("ERREUR::Paramètres incorrects");
            }
            if (n_param >=7)
            {
                id = (uint8_t)parseInt(orderData.at(1));
                x = (uint32_t)parseInt(orderData.at(2));
                y = (uint32_t)parseInt(orderData.at(3));
                r = (uint32_t)parseInt(orderData.at(4));
                angleHook = parseFloat(orderData.at(5));
                angleTolerance = parseFloat(orderData.at(6));

                char hookOrder[RX_BUFFER_SIZE] = "";

                for (uint8_t i = 7; i < n_param + 1; i++) {
                    strcat(hookOrder, orderData.at(i));
                    strcat(hookOrder, " ");
                }
                hookOrder[RX_BUFFER_SIZE - 1] = '\0';

                hookList.addHook(id, x, y, r, angleHook, angleTolerance, hookOrder);

                //TEST:
                Serial.println(hookList.getHook(id).getOrder());
            }

        }
        else if (!strcmp(order, "eh"))              // Active un hook
        {
            int hookId = parseInt(orderData.at(1));

            if(hookList.hookWithId(hookId))
            {
                hookList.enableHook((uint8_t)hookId); //Singe proof ?
            }
            else
            {
                highLevel.log("ERREUR::Activation d'un hook inexistant");
            }

        }
        else if (!strcmp(order, "dh"))              // Désactive un hook
        {
            int hookId = parseInt(orderData.at(1));

            if(hookList.hookWithId(hookId))
            {
                hookList.disableHook((uint8_t)hookId); //Singe proof ?
            }
            else
            {
                highLevel.log("ERREUR::Activation d'un hook inexistant");
            }
        }

            /*			 _________________________________
             * 		   *|                                 |*
             *		   *|			   RANDOM	          |*
             *    	   *|_________________________________|*
              */

        else if (!strcmp(order, "demo"))            // Ordre de démo des différents actionneurs/du mouvement
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

        else                                        // Sérieux Jamy?
        {
            highLevel.printfln("ordre inconnu");
            highLevel.log("T'es un déchêt");
        }
        /*			 __________________
        * 		   *|                  |*
        *		   *|  COMM. DE BASE   |*
        *		   *|__________________|*
        */


	}

    checkHooks();
}

void OrderManager::refreshUS()
{
	sensorMgr.refresh();
}


/**
*	Sépare une courte chaîne de caractères(RX_BUFFER_SIZE) selon un séparateur, dans un tableau output (au plus 4 mots)
*/

uint8_t OrderManager::split(char* input, OrderData& output, const char* separator) {
    char *token;
    uint8_t i = 0;
    output.clear();
    token = strtok(input, separator);
    if (token != nullptr) {
        output.push_back(token);
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

void OrderManager::checkHooks() {
		hookList.check(motionControlSystem.getX(), motionControlSystem.getY(),motionControlSystem.getAngleRadian());
}

void OrderManager::executeHooks() {
	int l = hookList.getReadySize();
	for (uint8_t i = 0; i < l; ++i) {
		execute(hookList.getReadyHookOrder(i));
	}
}
