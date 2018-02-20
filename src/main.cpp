/**
*Initialisation et boucle principale du programme
*
* @author caillou, sylvain, rémi, melanie, Ug
*
**/

#include "Com/OrderManager.h"


void setup() {
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
}

/* Interruptions d'asservissements */
void motionControlInterrupt() {
	static MotionControlSystem &motionControlSystem = MotionControlSystem::Instance();
	motionControlSystem.updateTicks();
    motionControlSystem.control();
	motionControlSystem.updatePosition();
	motionControlSystem.manageStop();
}


void blink(){
	static int32_t t=0;
	if(millis()-t>500){
		t=millis();
		digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
	}
}
/**
 * Boucle principale, y est géré:
 * La communication HL
 * Les capteurs
 * Divers initialisations et instanciations
 */
void loop(){
	OrderManager& orderMgr = OrderManager::Instance();

    /* AX12 initialisation */
    orderMgr.execute("rlbAv");
    orderMgr.execute("rlbAr");
    delay(1000);
    orderMgr.execute("flpAv");
    orderMgr.execute("flpAr");
    delay(1000);

    /* MotionControlSystem */
    IntervalTimer motionControlInterruptTimer;
    motionControlInterruptTimer.priority(253);
    motionControlInterruptTimer.begin(motionControlInterrupt, MC_PERIOD); // Setup de l'interruption d'asservissement


    orderMgr.execute("ct0");
    orderMgr.execute("cr0");
    orderMgr.execute("monthlery");
    orderMgr.execute("kpg 0.2");
    orderMgr.execute("kig 0.0001");
    orderMgr.execute("kdg 0.2");
    orderMgr.execute("kpd 0.2");
    orderMgr.execute("kid 0.0001");
    orderMgr.execute("kdd 0.2");

//    orderMgr.execute("kpt 2.05");
//    orderMgr.execute("kit 0.0");
//    orderMgr.execute("kdt 0.0");
//
//    orderMgr.execute("kpr 3.0");
//    orderMgr.execute("kir 0.0");
//    orderMgr.execute("kdr 0.0");
//    orderMgr.execute("t pi");
    for(int i=0;i<500;i++)
    {
        if(i==20)
        {
//            orderMgr.execute("d 1000");
//            orderMgr.execute("t pi");
            orderMgr.execute("av");
//            orderMgr.execute("rawpwm 150");
        }
        orderMgr.execute("rawposdata");
        delay(10);
    }
    orderMgr.execute("sstop");
    delay(1000);
//    for(int i = 0;i<20;i++)
//    {
//        orderMgr.execute("rawpostdata");
//        delay(10);
//    }
//    delay(1000);
//    orderMgr.execute("rc");
//    delay(130*10);
//    orderMgr.execute("sstop");
//    orderMgr.execute("d -1000");
//    orderMgr.execute("t 0");
//    orderMgr.execute("rawpwm 0");
//    for(int i=0;i<600/1;i++)
//    {
//        if(i==20)
//        {
//            orderMgr.execute("d -1000");
//        }
//        orderMgr.execute("rawposdata");
//        delay(10);
//    }
    Serial.println("DATAEND");


    while (true) {
		 orderMgr.communicate();
//		 orderMgr.refreshUS();
    }
}

/*
extern "C"
{
    void _kill(){}
    void _getpid(){}
}*/



/* Ce bout de code permet de compiler avec std::vector, copie honteusement de chez INTech-Senpai */
namespace std {
	void __throw_bad_alloc()
	{
		while (true)
		{
			Serial.println("ERROR\tUnable to allocate memory");
			delay(500);
		}
	}

	void __throw_length_error(char const*e)
	{
		while (true)
		{
			Serial.print("Length Error\t");
			Serial.println(e);
			delay(500);
		}
	}

	void __throw_out_of_range(char const*e)
	{
		while (true)
		{
			Serial.print("Out of range error\t");
			Serial.println(e);
			delay(500);
		}
	}

	void __throw_out_of_range_fmt(char const*e, ...)
	{
		while (true)
		{
			Serial.print("Out of range fmt\t");
			Serial.println(e);
			delay(500);
		}
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
