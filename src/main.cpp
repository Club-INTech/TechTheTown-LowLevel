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

void test()
{
	Serial.println("coucou");
	delay(200);
}
/**
 * Boucle principale, y est géré:
 * La communication HL
 * Les capteurs
 * Divers initialisations et instanciations
 */

void inc()
{
	Serial.println("caca");
}

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
	delay(1500);//Laisse le temps aux capteurs de clignotter leur ID
#if DEBUG
	IntervalTimer blinkTim;
	blinkTim.priority(255);
	blinkTim.begin(blink,500000);
#endif


	//PassageCounter cnter(100,10,inc);
    while (true) {
        orderMgr.communicate();
        orderMgr.refreshUS();
		//cnter.update();
		//Serial.println("test");
        if(orderMgr.isHLWaiting())
        {
            orderMgr.checkJumper();
        }

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
