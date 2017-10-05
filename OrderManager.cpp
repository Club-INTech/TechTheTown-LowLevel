#include "OrderManager.h"

OrderManager::OrderManager():	motionControlSystem(MotionControlSystem::Instance()), 
								sensorMgr(SensorMgr::Instance()), 
								actuatorsMgr(ActuatorsMgr::Instance()),
								serialHL(SerialMgr::Instance()),
								hookList(std::vector<Hook>()),
#if DEBUG
								highLevel(SerialMgr::Instance())
#else
								highLevel(EthernetMgr::Instance())
#endif
{
	memset(readMessage, 0, RX_BUFFER_SIZE);
	isSendingUS = false;
	hooksEnabled = true;
}

void OrderManager::communicate() {
	if (highLevel.read(readMessage)) {
		execute(readMessage);
	}
	memset(readMessage, 0, RX_BUFFER_SIZE);
}

void OrderManager::execute(const char* orderToExecute)
{
		char order[RX_BUFFER_SIZE];
		strcpy(order, orderToExecute);
		highLevel.log("Message recu: %s", order);

		int8_t n_param = split(order, orderData, SEPARATOR);		//Sépare l'ordre en plusieurs mots, n_param=nombre de paramètres

	if (n_param >= 0) {
		strcpy(order, orderData.at(0));

		/*			 __________________
		* 		   *|                  |*
		*		   *|  COMM. DE BASE   |*
		*		   *|__________________|*
		*/

		if (!strcmp(order, "?"))			//Ping
		{
			highLevel.printfln("0");
		}
		else if (!strcmp(order, "sus"))		//Switch d'envois périodiques de données des capteurs
		{
			highLevel.log("Activation US");
			isSendingUS = !isSendingUS;
		}
		else if (!strcmp(order, "f"))
		{
			highLevel.println(motionControlSystem.isMoving());
			highLevel.println(motionControlSystem.isMoveAbnormal());
		}
		else if (!strcmp(order, "?xyo"))		//Renvoie la position du robot (en mm et radians)
		{
			//highLevel.printfln("%f,%f,%f", motionControlSystem.getX(), motionControlSystem.getY(), motionControlSystem.getAngleRadian());
			highLevel.println(motionControlSystem.getX());
			highLevel.println(motionControlSystem.getY());
			highLevel.println(motionControlSystem.getAngleRadian());
		}
		else if (!strcmp(order, "d"))		//Ordre de déplacement rectiligne (en mm)
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
		else if (!strcmp(order, "t"))
		{
			if (n_param == 1) {
				float angle = motionControlSystem.getAngleRadian();
				angle = parseFloat(orderData.at(1));
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

		else if (!strcmp(order, "cx"))
		{
			if (n_param == 1) {
				float x = parseFloat(orderData.at(1));
				motionControlSystem.setX(x);
			}
			else {
				highLevel.log("ERREUR::Paramètres incorrects");
			}

		}
		else if (!strcmp(order, "cy"))
		{
			if (n_param == 1) {
				float y = parseFloat(orderData.at(1));
				motionControlSystem.setY(y);
			}
			else {
				highLevel.log("ERREUR::Paramètres incorrects");
			}
		}
		else if (!strcmp(order, "co"))
		{
			if (n_param == 1) {
				float o = parseFloat(orderData.at(1));
				motionControlSystem.setOriginalAngle(o);
			}
			else {
				highLevel.log("ERREUR::Paramètres incorrects");
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
				highLevel.log("ERREUR::Paramètres incorrects");
			}

		}
		else if (!strcmp(order, "ctv"))
		{
			if (n_param == 1) {
				float speed = parseFloat(orderData.at(1));
				motionControlSystem.setTranslationSpeed(speed);
			}
			else {
				highLevel.log("ERREUR::Paramètres incorrects");
			}

		}
		else if (!strcmp(order, "crv"))
		{
			if (n_param == 1) {
				float speed = parseFloat(orderData.at(1));
				motionControlSystem.setRotationSpeed(speed);
			}
			else {
				highLevel.log("ERREUR::Paramètres incorrects");
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
				highLevel.log("ERREUR::Paramètres incorrects");
			}

		}

		else if (!strcmp(order, "efm"))
		{
			motionControlSystem.enableForcedMovement();
		}
		else if (!strcmp(order, "dfm"))
		{
			motionControlSystem.disableForcedMovement();
		}

		/*			 ___________________
		* 		   *|                   |*
		*		   *|  ASSERVISSEMENTS  |*
		*		   *|___________________|*
		*/

		else if (!strcmp(order, "ct0"))		//Désactiver l'asservissement en translation
		{
			motionControlSystem.enableTranslationControl(false);
			highLevel.log("non asservi en translation");
		}
		else if (!strcmp(order, "ct1"))		//Activer l'asservissement en translation
		{
			motionControlSystem.enableTranslationControl(true);
			highLevel.log("asservi en translation");
		}
		else if (!strcmp(order, "cr0"))		//Désactiver l'asservissement en rotation
		{
			motionControlSystem.enableRotationControl(false);
			highLevel.log("non asservi en rotation");
		}
		else if (!strcmp(order, "cr1"))		//Activer l'asservissement en rotation
		{
			motionControlSystem.enableRotationControl(true);
			highLevel.log("asservi en rotation");
		}
		else if (!strcmp(order, "cv0"))		//Désactiver l'asservissement en vitesse
		{
			motionControlSystem.enableSpeedControl(false);
			highLevel.log("non asservi en vitesse");
		}
		else if (!strcmp(order, "cv1"))		//Activer l'asservissement en vitesse
		{
			motionControlSystem.enableSpeedControl(true);
			highLevel.log("asservi en vitesse");
		}

		/*			 ___________________________
		* 		   *|                           |*
		*		   *|					        |*
		*		   *|			DEBUG			|*
		*		   *|						    |*
		*		   *|___________________________|*
		*/

		else if (!strcmp(order, "cod")) {
			highLevel.log("Gauche:");
			highLevel.log("%ld", motionControlSystem.getCodG());
			highLevel.log("Droite:");
			highLevel.log("%ld", motionControlSystem.getCodD());
		}
		else if (!strcmp("pfdebug", order))
		{
			//highLevel.printfln("%d", (int)motionControlSystem.getRightSpeed());
			//highLevel.printfln("%d", (int)motionControlSystem.getRightMotorDir());
			//highLevel.printfln("%d", (int)motionControlSystem.getRightSetPoint());
			//highLevel.printfln("%d", (int)motionControlSystem.getRightMotorPWM());
			//highLevel.printfln("%d", (int)motionControlSystem.getCodD());
		}

		/*			 ___________________________
		* 		   *|                           |*
		*		   *|         MONTLHERY         |*
		*		   *|   DEPLACEMENT ET ROTATION |*
		*		   *|    SANS ASSERVISSEMENT    |*
		*		   *|___________________________|*
		*/

		else if (!strcmp(order, "monthlery"))
		{
			motionControlSystem.enableTranslationControl(false);
			motionControlSystem.enableRotationControl(false);
		}
		else if (!strcmp(order, "av"))
		{
			motionControlSystem.setRawPositiveTranslationSpeed();  // definit la consigne max de vitesse de translation envoi�e au PID (trap�ze)
																	// déplacement vers l'avant avec asservissement
		}

		else if (!strcmp(order, "rc"))
		{
			motionControlSystem.setRawNegativeTranslationSpeed();  // definit la consigne max de vitesse de translation envoi�e au PID (trap�ze)
																	// déplacement vers l'arrière avec asservissement
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
				highLevel.log("reglage de la translation");
			else
				highLevel.log("reglage de la rotation");
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
			highLevel.log("trans : kp= %g ; ki= %g ; kd= %g", kp_t, ki_t, kd_t);
			highLevel.log("rot   : kp= %g ; ki= %g ; kd= %g", kp_r, ki_r, kd_r);
			highLevel.log("gauche: kp= %g ; ki= %g ; kd= %g", kp_g, ki_g, kd_g);
			highLevel.log("droite: kp= %g ; ki= %g ; kd= %g", kp_d, ki_d, kd_d);
		}
		// ***********  Paramètres du PID pour l'asserv en position (TRANSLATION)  ***********
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

		// ***********  Paramètres du PID pour l'asserv en ROTATION  ***********
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

		// ***********  Paramètres du PID pour l'asserv en vitesse à gauche  ***********
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

		// ***********  Paramètres du PID pour l'asserv en vitesse à droite ****************
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

		else if (!strcmp(order, "axTest"))
		{
			if (n_param == 1) {
				int16_t goal = 150;
				goal = parseFloat(orderData.at(1));
				actuatorsMgr.testGoto(goal);
			}
			else {
				highLevel.log("ERREUR::Paramètres incorrects");
			}
		}

		/*			 _________________________________
		* 		   *|                                 |*
		*		   *|			   HOOKS	              |*
		*    	   *|_________________________________|*
		*/
		else if(!strcmp(order, "nh")){
			int16_t id, x, y, r;
			if (n_param >=5) {
				id = parseInt(orderData.at(1));
				x = parseInt(orderData.at(2));
				y = parseInt(orderData.at(3));
				r = parseInt(orderData.at(4));

				char hookOrder[RX_BUFFER_SIZE] = "";

				for (int i = 5; i < n_param + 1; i++) {
					strcat(hookOrder, orderData.at(i));
					strcat(hookOrder, " ");

				}
				hookList.push_back(Hook(id, x, y, r, hookOrder));
				for (uint i = 0; i < hookList.size(); i++) {
					Serial.println(hookList.at(i).getOrder());
				}
			}
			else {
				highLevel.log("ERREUR::Paramètres incorrects");
			}

		}
		else if (!strcmp(order, "eh")) {
			//TODO: active un hook
		}
		else if (!strcmp(order, "dh")) {
			//TODO: désactive un hook
		}
		else
		{
			highLevel.printfln("ordre inconnu");
			highLevel.log("T'es un déchêt");
		}
	}

	executeHooks();

}

void OrderManager::refreshUS()
{
	sensorMgr.refresh(motionControlSystem.getMovingDirection());
}

void OrderManager::sendUSData() {
	static uint32_t lastSent = 0;
	if (isSendingUS && millis() - lastSent > 100) 
	{
		//highLevel.sendUS(sensorMgr.getUsTest());
		lastSent = millis();
	}
}

/**
*	Sépare une courte chaîne de caractères(RX_BUFFER_SIZE) selon un séparateur, dans un tableau output (au plus 4 mots)
*/

int8_t OrderManager::split(char* input, OrderData& output, const char* separator) {
	//uint32_t start = micros();
	char* token;
	int i = 0;
	output.clear();
	token = strtok(input, separator);

	if (token != NULL) {
		output.push_back(token);
	}

	//Le premier caractère est le nombre de mots (max rx_word_count ici)

	do {
		token = strtok(NULL, separator);
		if (token != NULL) {
			output.push_back(token);
			++i;
		}
	} while (token != NULL && i < RX_WORD_COUNT);

	//Serial.println(micros() - start);
	//Serial.println("C'était la durée du split");
	return i;
}

int OrderManager::parseInt(const char* s) {
	char currentChar = s[0];
	int i = 0;
	while (currentChar != '\0') {
		if (!isDigit(currentChar)) {
			highLevel.log("Not a digit at pos %d of:", i);
			highLevel.log(s);

			return -1;
		}
		currentChar = s[++i];
	}
	return strtol(s, nullptr, DEC);
}

float OrderManager::parseFloat(const char* s) {
	char currentChar = s[0];
	int i = 0;
	uint8_t point = 0;
	while (currentChar != '\0') {
		if (!isDigit(currentChar) && currentChar != '.') {
			highLevel.log("Not a digit at pos %d of:", i);
			highLevel.log(s);


			return -1;
		}
		if (currentChar == '.') {
			++point;
		}
		if (point > 1) {
			return -1;
		}
		currentChar = s[++i];
	}
	return strtod(s, nullptr);
}


void OrderManager::hookInterrupt() {
	if (hooksEnabled) {
		uint8_t l = hookList.size();
		for (int i = 0; i < l; ++i) {
			if (!hookList.at(i).isReady() && hookList.at(i).check(motionControlSystem.getX(), motionControlSystem.getY())) {
				hookList.at(i).setReady();
			}
		}
	}
}

void OrderManager::executeHooks() {

	if (hooksEnabled) {
		uint8_t l = hookList.size();

		for (int i = 0; i < l; ++i) {
			if (hookList.at(i).isReady()) {
				execute(hookList.at(i).getOrder());
			}
		}
	}
}