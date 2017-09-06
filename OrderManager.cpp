#include "OrderManager.h"

OrderManager::OrderManager():	motionControlSystem(MotionControlSystem::Instance()), 
								sensorMgr(SensorMgr::Instance()), 
								actuatorsMgr(ActuatorsMgr::Instance()),
								serialHL(SerialHL::Instance()),
								ethMgr(EthernetMgr::Instance())
{
#if DEBUG
	highLevel = serialHL;
#else
	highLevel = ethMgr;
#endif
	order = "";
	isSendingUS = false;
}

void OrderManager::refreshUS()
{
	sensorMgr.refresh(motionControlSystem.getMovingDirection());
}

void OrderManager::receiveAndExecute() //TODO:traiter selon si c'est la liaison série ou ethernet qui envoie une donnée
{
	if(highLevel.read(order)){
		ack();
		highLevel.log("Ordre recu: %s", order.c_str());
		serialHL.log("Ordre recu: %s", order.c_str());


/*			 __________________
* 		   *|                  |*
*		   *|  COMM. DE BASE   |*
*		   *|__________________|*
*/

		if (order.equals("?"))			//Ping
		{
			highLevel.printfln("0");
		}
		else if (order.equals("sus"))		//Switch d'envois périodiques de données des capteurs
		{
			highLevel.log("Activation US");
			isSendingUS = !isSendingUS;
		}
		else if (order.equals("f")) 
		{
			highLevel.printfln("%d", motionControlSystem.isMoving());
			highLevel.printfln("%d", motionControlSystem.isMoveAbnormal());
		}
		else if (order.equals("?xyo"))		//Renvoie la position du robot (en mm et radians)
		{
			highLevel.printfln("%f", motionControlSystem.getX());
			highLevel.printfln("%f", motionControlSystem.getY());
			highLevel.printfln("%f", motionControlSystem.getAngleRadian());
		}
		else if (order.equals("d"))		//Ordre de d�placement rectiligne (en mm)
		{
			int16_t deplacement = 0;
			highLevel.read(deplacement);
			ack();           //Acquittement
			highLevel.log("distance : %d",deplacement);
			motionControlSystem.orderTranslation(deplacement);
		}
		else if (order.equals("t"))
		{
			float angle = motionControlSystem.getAngleRadian();
			highLevel.read(angle);
			ack();
			highLevel.log("angle : %f", angle);
			motionControlSystem.orderRotation(angle, MotionControlSystem::FREE);
		}
		else if (order.equals("t3"))
		{
			float angle_actuel = motionControlSystem.getAngleRadian();
			float delta_angle;
			highLevel.read(delta_angle);
			ack();
			motionControlSystem.orderRotation(angle_actuel + delta_angle, MotionControlSystem::FREE);
		}
		else if (order.equals("stop"))
		{
			motionControlSystem.stop();
		}
		else if (order.equals("cx"))
		{
			float x;
			highLevel.read(x);
			ack();
			motionControlSystem.setX(x);
		}
		else if (order.equals("cy"))
		{
			float y;
			highLevel.read(y);
			ack();
			motionControlSystem.setY(y);
		}
		else if (order.equals("co"))
		{
			float o;
			highLevel.read(o);
			ack();
			motionControlSystem.setOriginalAngle(o);
		}
		else if (order.equals("cxyz"))
		{
			float x, y, o;
			highLevel.read(x);
			ack();
			highLevel.read(y);
			ack();
			highLevel.read(o);
			ack();

			motionControlSystem.setX(x);
			motionControlSystem.setY(y);
			motionControlSystem.setOriginalAngle(o);
		}
		else if (order.equals("ctv"))
		{
			float speed=0;
			highLevel.read(speed);
			ack();
			motionControlSystem.setTranslationSpeed(speed);
		}
		else if (order.equals("crv"))
		{
			float speed = 0;
			highLevel.read(speed);
			ack();
			motionControlSystem.setRotationSpeed(speed);
		}
		else if (order.equals("efm"))
		{
			motionControlSystem.enableForcedMovement();
		}
		else if (order.equals("dfm"))
		{
			motionControlSystem.disableForcedMovement();
		}

		/*			 ___________________
		* 		   *|                   |*
		*		   *|  ASSERVISSEMENTS  |*
		*		   *|___________________|*
		*/

		else if (order.equals("ct0"))		//D�sactiver l'asservissement en translation
		{
			motionControlSystem.enableTranslationControl(false);
			highLevel.log("non asservi en translation");
		}
		else if (order.equals("ct1"))		//Activer l'asservissement en translation
		{
			motionControlSystem.enableTranslationControl(true);
			highLevel.log("asservi en translation");
		}
		else if (order.equals("cr0"))		//D�sactiver l'asservissement en rotation
		{
			motionControlSystem.enableRotationControl(false);
			highLevel.log("non asservi en rotation");
		}
		else if (order.equals("cr1"))		//Activer l'asservissement en rotation
		{
			motionControlSystem.enableRotationControl(true);
			highLevel.log("asservi en rotation");
		}
		else if (order.equals("cv0"))		//Désactiver l'asservissement en vitesse
		{
			motionControlSystem.enableSpeedControl(false);
			highLevel.log("non asservi en vitesse");
		}
		else if (order.equals("cv1"))		//Activer l'asservissement en vitesse
		{
			motionControlSystem.enableSpeedControl(true);
			highLevel.log("asservi en vitesse");
		}

/*			 ___________________________
* 		   *|                           |*
*		   *|         MONTLHERY         |*
*		   *|   DEPLACEMENT ET ROTATION |*
*		   *|    AVEC ASSERVISSEMENT    |*
*		   *|___________________________|*
*/

		else if (order.equals("monthlery"))
		{
			motionControlSystem.enableTranslationControl(false);
			motionControlSystem.enableRotationControl(false);
		}
		else if (order.equals("av"))
		{
			motionControlSystem.setRawPositiveTranslationSpeed();  // definit la consigne max de vitesse de translation envoi�e au PID (trap�ze)
																	// déplacement vers l'avant avec asservissement
		}

		else if (order.equals("rc"))
		{
			motionControlSystem.setRawNegativeTranslationSpeed();  // definit la consigne max de vitesse de translation envoi�e au PID (trap�ze)
																	// déplacement vers l'arrière avec asservissement
		}

		else if (order.equals("td"))
		{
			motionControlSystem.setRawNegativeRotationSpeed();     // definit la consigne max de vitesse de rotation envoi�e au PID (trap�ze)
																	// rotation sens antitrigo avec asservissement
		}

		else if (order.equals("tg"))
		{
			motionControlSystem.setRawPositiveRotationSpeed();     // definit la consigne max de vitesse de rotation envoi�e au PID (trap�ze)
																	// rotation sens antitrigo avec asservissement
		}

		else if (order.equals("sstop"))                            // Stoppe le robot
		{
			motionControlSystem.setRawNullSpeed();
		}

/*			 _________________________________
* 		   *|                                 |*
*		   *|CONSTANTES D'ASSERV (pour le PID)|*
*    	   *|_________________________________|*
*/
		else if (order.equals("toggle"))
		{
			motionControlSystem.translation = !motionControlSystem.translation;   //Bascule entre le réglage d'asserv en translation et en rotation
			if (motionControlSystem.translation)
				highLevel.log("reglage de la translation");
			else
				highLevel.log("reglage de la rotation");
		}
		else if (order.equals("display")) //affiche les paramètres des PID des différentes asserv (translation, rotation, vitesse à droite, vitesse à gauche)
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
		// ***********  Paramètres du PID pour l'asserv en position (TRANSLATION)  ***********
		else if (order.equals("kpt"))
		{
			float kp, ki, kd;
			highLevel.printfln("kp_trans ?");
			motionControlSystem.getTranslationTunings(kp, ki, kd);
			highLevel.read(kp);
			ack();
			motionControlSystem.setTranslationTunings(kp, ki, kd);
			highLevel.printfln("kp_trans = %g", kp);
		}
		else if (order.equals("kdt"))
		{
			float kp, ki, kd;
			highLevel.printfln("kd_trans ?");
			motionControlSystem.getTranslationTunings(kp, ki, kd);
			highLevel.read(kd);
			ack();
			motionControlSystem.setTranslationTunings(kp, ki, kd);
			highLevel.printfln("kd_trans = %g", kd);
		}
		else if (order.equals("kit"))
		{
			float kp, ki, kd;
			highLevel.printfln("ki_trans ?");
			motionControlSystem.getTranslationTunings(kp, ki, kd);
			highLevel.read(ki);
			ack();
			motionControlSystem.setTranslationTunings(kp, ki, kd);
			highLevel.printfln("ki_trans = %g", ki);
		}

		// ***********  Paramètres du PID pour l'asserv en ROTATION  ***********
		else if (order.equals("kpr"))
		{
			float kp, ki, kd;
			highLevel.printfln("kp_rot ?");
			motionControlSystem.getRotationTunings(kp, ki, kd);
			highLevel.read(kp);
			ack();
			motionControlSystem.setRotationTunings(kp, ki, kd);
			highLevel.printfln("kp_rot = %g", kp);
		}
		else if (order.equals("kir"))
		{
			float kp, ki, kd;
			highLevel.printfln("ki_rot ?");
			motionControlSystem.getRotationTunings(kp, ki, kd);
			highLevel.read(ki);
			ack();
			motionControlSystem.setRotationTunings(kp, ki, kd);
			highLevel.printfln("ki_rot = %g", ki);
		}
		else if (order.equals("kdr"))
		{
			float kp, ki, kd;
			highLevel.printfln("kd_rot ?");
			motionControlSystem.getRotationTunings(kp, ki, kd);
			highLevel.read(kd);
			ack();
			motionControlSystem.setRotationTunings(kp, ki, kd);
			highLevel.printfln("kd_rot = %g", kd);
		}

		// ***********  Paramètres du PID pour l'asserv en vitesse à gauche  ***********
		else if (order.equals("kpg"))
		{
			float kp, ki, kd;
			highLevel.printfln("kp_gauche ?");
			motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
			highLevel.read(kp);
			ack();
			motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
			highLevel.printfln("kp_gauche = %g", kp);
		}
		else if (order.equals("kig"))
		{
			float kp, ki, kd;
			highLevel.printfln("ki_gauche ?");
			motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
			highLevel.read(ki);
			ack();
			motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
			highLevel.printfln("ki_gauche = %g", ki);
		}
		else if (order.equals("kdg"))
		{
			float kp, ki, kd;
			highLevel.printfln("kd_gauche ?");
			motionControlSystem.getLeftSpeedTunings(kp, ki, kd);
			highLevel.read(kd);
			ack();
			motionControlSystem.setLeftSpeedTunings(kp, ki, kd);
			highLevel.printfln("kd_gauche = %g", kd);
		}

		// ***********  Paramètres du PID pour l'asserv en vitesse à droite ****************
		else if (order.equals("kpd"))
		{
			float kp, ki, kd;
			highLevel.printfln("kp_droite ?");
			motionControlSystem.getRightSpeedTunings(kp, ki, kd);
			highLevel.read(kp);
			ack();
			motionControlSystem.setRightSpeedTunings(kp, ki, kd);
			highLevel.printfln("kp_droite = %g", kp);
		}
		else if (order.equals("kid"))
		{
			float kp, ki, kd;
			highLevel.printfln("ki_droite ?");
			motionControlSystem.getRightSpeedTunings(kp, ki, kd);
			highLevel.read(ki);
			ack();
			motionControlSystem.setRightSpeedTunings(kp, ki, kd);
			highLevel.printfln("ki_droite = %g", ki);
		}
		else if (order.equals("kdd"))
		{
			float kp, ki, kd;
			highLevel.printfln("kd_droite ?");
			motionControlSystem.getRightSpeedTunings(kp, ki, kd);
			highLevel.read(kd);
			ack();
			motionControlSystem.setRightSpeedTunings(kp, ki, kd);
			highLevel.printfln("kd_droite = %g", kd);
		}
		else if (order.equals("accelAv"))
		{
			motionControlSystem.setAccelAv();
		}
		else if (order.equals("accelAr"))
		{
			motionControlSystem.setAccelAr();
		}
		else if (order.equals("axTest"))
		{
			int16_t goal = 150;
			highLevel.read(goal);
			ack();
			actuatorsMgr.testGoto(goal);
		}
		else
		{
			highLevel.printfln("ordre inconnu");
		}
	}
	order = "";
}

void OrderManager::sendUSData() {
	static uint32_t lastSent = 0;
	if (isSendingUS && millis() - lastSent > 100) 
	{
		highLevel.sendUS(sensorMgr.getUsTest());
		lastSent = millis();
	}
}

//Parce que c'est chiant d'acquitter à la main
void OrderManager::ack() {
	highLevel.printfln("_");
}