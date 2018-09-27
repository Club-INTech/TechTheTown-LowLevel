/**
*Initialisation et boucle principale du programme
*
* @author caillou, sylvain, rémi, melanie, Ug
*
**/

#include "Com/Order/OrderManager.h"

/* Interruption d'asservissement */
void motionControlInterrupt() {
	static int compteurDeLavage = 0;
	static MotionControlSystem &motionControlSystem = MotionControlSystem::Instance();
	motionControlSystem.updateTicks();
	motionControlSystem.control();
	motionControlSystem.updatePosition();
	if(compteurDeLavage == 5)
	{
		motionControlSystem.manageStop();
		compteurDeLavage = 0;
	}
	compteurDeLavage++;
}

int main(){
	/*************************
	 * Initialisation du LL, gère:
	 * La série
	 * Les actionneurs
	 * L'asservissement
	 *************************/
	/* Série */
	Serial.begin(115200);

	Serial.flush();
	Serial.println("Série OK");
	delay(250);

	/* Actuators */
	// Par sécurité on met tout les actuators à LOW quand on les initialise
	/* Pompe */
	pinMode(PIN_PWM_POMPE,OUTPUT);
	digitalWrite(PIN_PWM_POMPE,LOW);

	/* Electrovanne */
	pinMode(PIN_ELECTROVANNE_AV,OUTPUT);
	digitalWrite(PIN_ELECTROVANNE_AV,LOW);
	pinMode(PIN_ELECTROVANNE_AR,OUTPUT);
	digitalWrite(PIN_ELECTROVANNE_AR,LOW);

	Serial.println("Fin du setup");
	OrderManager& orderMgr = OrderManager::Instance();

    // AX12 initialisation
    orderMgr.execute("rlbAv");
    orderMgr.execute("rlbAr");
    delay(1000);
    orderMgr.execute("flpAv");
    orderMgr.execute("flpAr");
    delay(1000);
	orderMgr.execute("ctv 300");
	orderMgr.execute("crv 1.75");

    delay(2000);
    // MotionControlSystem interrupt on timer
	IntervalTimer motionControlInterruptTimer;
	motionControlInterruptTimer.priority(253);
    motionControlInterruptTimer.begin(motionControlInterrupt, MC_PERIOD); // Setup de l'interruption d'asservissement

	// Measure Ambient light
	orderMgr.sensorMgr.measureMeanAmbientLight();

	delay(1500);//Laisse le temps aux capteurs de clignotter leur ID

	static Metro USSend = Metro(80);
	/**
	 * Boucle principale, y est géré:
	 * La communication HL
	 * L'execution des ordres de ce dernier
	 * Les capteurs
	 */
	while (true) {
		orderMgr.communicate();
		orderMgr.refreshUS();
		orderMgr.isHLWaiting() ? orderMgr.checkJumper() : void();
		USSend.check() ? orderMgr.sendUS() : void();
    }
}

                   /*``.           `-:--.`
                  `.-::::/.        .:::::::y
                 `::::::::o``````.-::::::/h-
                 `::::::::::::::::::::::/h.
                  -::::::://++++//::::::::.
               `-::::/oso+/:-....--://:::::-.``.---:`
      ````   .-:::/so/-      ``````  `.:::::::::::::+ ``````````````````````````````````````````````````````````````````````
    `-:::::--:::os+.       `/sssssss`   ./syyyyyys+/y+sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssy- `-:::://
   `-:::::::::+y/`        .shhhhhdd:   `/hhhhhhhhhdo:/dddddddddddddddddhhhhhhhdddddddddddddddddddddddddddddddddddddddddddddo`.:////sy.
   ::::::::::so`         :yhhhhhmy`   .ohhhhhhhhhhd  `//oooooo+//////+hhhhhhdd+///////+++++++/////////////////+++++++/////-`-////+h+` `
   `://:::::y/         `+hhhhhdm+    -yhhhhhdhhhhhm`  `oyyyyyhs     .shhhhhdh-  ``.-:::////:::-.       ``.-::::////::://  `://///o+:::::::-.
      `::::y/         .shhhhhdh-   `+hhhhhdd+shhhhd- -yhhhhhmo`    /yhhhhhms` .-://+ssso+++/////o   `.:://+ossoo+++o+oy- -/////+ssoo+//////h
      .:::/y         :yhhhhhms`   .shhhhhdy::hhhhhho+hhhhhdd:    `ohhhhhdd/ .:///oyo::-----////oy `-:///oyo:.`      `-``:////oy+-` `:////+h:
      -:::o:       `+hhhhhdm/    :yhhhhddo:::+hhhhhhhhhhhms.    -yhhhhhdh. -////yo.`ooooooooooss``:////yo.            -/////yo`   .:////ys.
   ``.::::+-      .shhhhhdh-   `+hhhhhdd/::::/hhhhhhhhhdd/     /hhhhhhmo` `/////s```..----:+:..  -/////o``````..:.  `:////oh:   `-////+h/
`.-::::::::+     :yhhhhhms`   .shhhhhmy:::::::hhhhhhhhdh.    `ohhhhhdd:    :++////:::///+oys`    `/++/////://++yo` .:////ys`   .:////ys.
-::::::::::/-    /oooooo/     -sssyyyo:+o/++//hyssssss+`     .sssssss.      `-/++oooo++//:.        .:/+oooo++/:-   /ooooo:     :ooooo/
.:::::/o:::::-`               `.--:::os+```.---
 :+ooo/.`::::::-..`````````.--::::+ss:`
  ``     `-/::::::::::::::::::::::s.
          `:::::::::::://+::::::::o-
         `:::::::/h////::.-:::::::y-
         :::::::ss`        -:/+sso:
         .:/++sy:          `//*/


/*
 *   Dead Pingu in the Main !
 *      	      . --- .
		    /        \
		   |  X  _  X |
		   |  ./   \. |
		   /  `-._.-'  \
		.' /         \ `.
	.-~.-~/    o   o  \~-.~-.
.-~ ~    |    o  o     |    ~ ~-.
`- .     |      o  o   |     . -'
	 ~ - |      o      | - ~
	 	 \             /
	 	__\           /___
	 	~;_  >- . . -<  _i~
	 	  `'         `'
*/
