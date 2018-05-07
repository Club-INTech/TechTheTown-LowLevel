/**
*Initialisation et boucle principale du programme
*
* @author caillou, sylvain, rémi, melanie, Ug
*
**/

#include "Com/Order/OrderManager.h"


void setup() {

	/* Série */
	Serial.begin(9600);
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


	/*9pinMode(5,INPUT);
	pinMode(PIN_PWM_LEFT,OUTPUT);
	pinMode(PIN_DIR_LEFT,OUTPUT);
	pinMode(PIN_PWM_RIGHT,OUTPUT);
	analogWriteFrequency(PIN_PWM_LEFT,234375);
	digitalWrite(PIN_DIR_LEFT,HIGH);
	analogWriteFrequency(PIN_PWM_RIGHT,234375);*/

    Serial.println("Fin du setup");
}

/* Interruptions d'asservissements */
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

void loop(){
	OrderManager& orderMgr = OrderManager::Instance();

    // AX12 initialisation
    orderMgr.execute("rlbAv");
    orderMgr.execute("rlbAr");
    delay(1000);
    orderMgr.execute("flpAv");
    orderMgr.execute("flpAr");
    delay(1000);


    delay(2000);
    // MotionControlSystem

	IntervalTimer motionControlInterruptTimer;
	motionControlInterruptTimer.priority(253);
    motionControlInterruptTimer.begin(motionControlInterrupt, MC_PERIOD); // Setup de l'interruption d'asservissement


//    orderMgr.execute("ct0");
//    orderMgr.execute("cr0");
//    orderMgr.execute("monthlery");
//    orderMgr.execute("cv0");
//    orderMgr.execute("kpg 0.2165");
//    orderMgr.execute("kig 0.00005");
//    orderMgr.execute("kdg 0.414");
//    orderMgr.execute("kpd 0.225");
//    orderMgr.execute("kid 0.00005");
//    orderMgr.execute("kdd 0.4121");


//    orderMgr.execute("kpt 6.5");
//    orderMgr.execute("kit 0");
//    orderMgr.execute("kdt 1.08");
//
//    orderMgr.execute("kpr 15");
//    orderMgr.execute("kir 0.00001");
//    orderMgr.execute("kdr 0");

    /*
//    orderMgr.execute("t pi");
//    int i = 0;
    for(int i=0;i<250;i++)
//    while(true)
    {
        if(i==20)
        {
//			analogWrite(PIN_PWM_LEFT,100);
//			analogWrite(PIN_PWM_RIGHT,100);
//            orderMgr.execute("d 1000");
            orderMgr.execute("t pi");
//            orderMgr.execute("av");
//            orderMgr.execute("rawpwm 70");
        }
//        if(i%200==100)
//        {
//            orderMgr.execute("t 0.2");
//        }
//        if(i==120)
//        {
//            orderMgr.execute("d 300");
//        }
        orderMgr.execute("rawposdata");
//        orderMgr.execute("rawdistance");
        delay(10);
        i++;
    }
    orderMgr.execute("sstop");
//	analogWrite(PIN_PWM_LEFT,0);
//	analogWrite(PIN_PWM_RIGHT,0);
//    orderMgr.execute("rawpwm 0");
    for(int i = 0;i<200;i++)
    {
        orderMgr.execute("rawposdata");
        delay(10);
    }
//    orderMgr.execute("rawdistance");
//    delay(1000);
//    orderMgr.execute("rc");
//    delay(130*10);
//    orderMgr.execute("sstop");
//    orderMgr.execute("d -2000");
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

//    long long unsigned int i = 0;
//	orderMgr.execute("d 2000");
//	delay(4000);
//	orderMgr.execute("d -2000");
//	orderMgr.execute("t pi");
//	delay(2000);*/
//    orderMgr.execute("demo");


	// Measure Ambient light
	orderMgr.sensorMgr.measureMeanAmbientLight();


	delay(1500);//Laisse le temps aux capteurs de clignotter leur ID

    static Metro USSend = Metro(100);

    while (true) {
        orderMgr.communicate();
//		orderMgr.refreshUS();
        orderMgr.isHLWaiting() ? orderMgr.checkJumper() : void();
//        USSend.check() ? orderMgr.sendUS() : void();
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
