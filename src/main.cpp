#ifndef UNIT_TEST

/**
*Initialisation et boucle principale du programme
*
* @author caillou, sylvain, rémi, melanie, Ug
*
**/

#include "Com/OrderManager.h"

//Initialisation de la Serie
OrderManager& orderMgr = OrderManager::Instance();
void setup() {
	/*serie*/
	Serial.begin(115200);
	Serial.println("Série OK");
	delay(250);

	pinMode(30, OUTPUT);
    digitalWrite(30, HIGH);

    /*Actuators*/
		/*Pompe*/
	pinMode(PIN_PWM_POMPE,OUTPUT);
	orderMgr.execute("dlp");

        /*AX12*/
    orderMgr.execute("rlb");
    delay(1000);
    orderMgr.execute("flp");
    delay(1000);


    /*Wire.begin();
    for (int i = 0; i < US_TOTAL; i++) {
        boolean error = true;
        while (error) {
            Wire.beginTransmission(SLAVE_ADDRESS);
            error = Wire.endTransmission(I2C_STOP, 50000); // if error !=0, connection error(50ms timeout)
            if (error) { // Not connected, try to reset bus and connect again
                Serial.println("\nNo I2C connection\n...trying again\n");
                Wire.resetBus();
                Wire.begin();
                Wire.setClock(400000);
            }
            delay(200);
        }
    }*/


}

/* Interruptions1 d'asservissements */
void motionControlInterrupt() {
	static MotionControlSystem &motionControlSystem = MotionControlSystem::Instance();
	motionControlSystem.control();
	motionControlSystem.updatePosition();
	motionControlSystem.manageStop();
}


//Boucle principale, gere entre autres la communication avec le HL

void loop(){
	for(int i=0;i<10;i++)
	{
		digitalWrite(30,HIGH);
		delay(200);
		digitalWrite(30,LOW);
		delay(200);
	}


//	orderMgr.execute("nh 1 1500 1000 50 0 3.2 6"); //Test de hooks

    /* MotionControlSystem */
    IntervalTimer motionControlInterruptTimer;
    motionControlInterruptTimer.priority(253);
    motionControlInterruptTimer.begin(motionControlInterrupt, MC_PERIOD); //asservissements


//    orderMgr.execute("nh 1 0 0 20 1.6 0.3 olp");
//    orderMgr.execute("t 1.6");

//    orderMgr.execute("ctrv 3000 100");
//    orderMgr.execute("demo");
//    orderMgr.execute("ctv 250");
//    orderMgr.execute("ct0");
//    orderMgr.execute("cr0");
//    delay(3000);
//    digitalWrite(30,HIGH);
//    orderMgr.execute("d 500");
//    orderMgr.execute("av");
//    while(true)
//    {
//        orderMgr.execute("cod");
//        delay(250);
//    }
    orderMgr.execute("kit 0");
    orderMgr.execute("kdt 0");
    orderMgr.execute("kpt 40");
    orderMgr.execute("kir 0");
    orderMgr.execute("kdr 4");
    orderMgr.execute("kpr 9"); //14 semble être autour de la limite du dépassement à 15
    orderMgr.execute("ct0");
    orderMgr.execute("t pi");
//    orderMgr.execute("d 500");
    for(int i=0;i<300/1;i++)
    {
        orderMgr.execute("rawposdata");
        delay(10);
    }
    orderMgr.execute("t 0");
//    orderMgr.execute("d -100");

	//orderMgr.execute("alp");


	/*orderMgr.execute("d 500");
	delay(5000);
	for (int i=0; i<4; i++) {
		orderMgr.execute("blb");
		delay(2000);
		orderMgr.execute("alp");
		delay(2000);
		orderMgr.execute("rlb");
		delay(2000);
		orderMgr.execute("dlp");
		delay(2000);
		orderMgr.execute("d 60");
	}*/
    /* Gestion des ordres recus */

    while (true) {
        //orderMgr.refreshUS();
        orderMgr.communicate();
        //orderMgr.sendUSData();
    }
}






/* Ce bout de code permet de compiler avec std::vector, copi� honteusement de chez INTech-Senpai */
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

#endif //!UNIT_TEST
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

