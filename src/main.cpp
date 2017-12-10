#ifndef UNIT_TEST

/**
*Initialisation et boucle principale du programme
*
* @author caillou, sylvain, rémi, melanie, Ug
*
**/

#include "Com/OrderManager.h"

//Initialisation de la Serie
void setup() {
	/*serie*/
	Serial.begin(115200);
	Serial.println("Série OK");
	delay(250);

	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);


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

/* Interruptions d'asservissements */
void motionControlInterrupt() {
	static MotionControlSystem &motionControlSystem = MotionControlSystem::Instance();
	motionControlSystem.control();
	motionControlSystem.manageStop();
}


//Boucle principale, gere entre autres la communication avec le HL

void loop(){

	for(int i=0;i<10;i++)
	{
		digitalWrite(13,HIGH);
		delay(200);
		digitalWrite(13,LOW);
		delay(200);
	}

    /*ActuatorsMgr& actuatorMgr = ActuatorsMgr::Instance();
    actuatorMgr.addAX12Group();
    actuatorMgr.populateAX12Group(0,1,100,BASE);
    actuatorMgr.populateAX12Group(0,8,100,MIRROR);
    actuatorMgr.addAX12(3,100);*/

    pinMode(24,OUTPUT);

    OrderManager& orderMgr = OrderManager::Instance();

//	orderMgr.execute("nh 1 1500 1000 50 0 3.2 6");

    digitalWrite(24, HIGH);

//    orderMgr.execute("AXm 3 10");
//    orderMgr.execute("AXGm 0 89");

    delay(2000);

//    orderMgr.execute("AXGm 0 130");

    delay(4000);

    digitalWrite(24,LOW);


    /* MotionControlSystem */
    IntervalTimer motionControlInterruptTimer;
    motionControlInterruptTimer.priority(253);
    motionControlInterruptTimer.begin(motionControlInterrupt, MC_PERIOD); //asservissements

	orderMgr.execute("cod");


	/*delay(2000);
	orderMgr.execute("d 500");
	delay(2000);
	orderMgr.execute("d -500");
    delay(2000);
    orderMgr.execute("d 1000");
    delay(4000);
    orderMgr.execute("d -1000");
    delay(4000);
    orderMgr.execute("t 3.14");
    delay(5000);
    orderMgr.execute("t -3.14");
    delay(5000);
    orderMgr.execute("t 0");
    delay(5000);
    orderMgr.execute("t 1.6");
    delay(5000);
    orderMgr.execute("t -1.6");
    delay(5000);
    orderMgr.execute("t 0");*/

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

