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
    Serial.flush();
	Serial.println("Série OK");
	delay(250);

	pinMode(30, OUTPUT);
    digitalWrite(30, HIGH);

    /*Actuators*/
		/*Pompe*/
	pinMode(PIN_PWM_POMPE,OUTPUT);
}

/* Interruptions1 d'asservissements */
void motionControlInterrupt() {
	static MotionControlSystem &motionControlSystem = MotionControlSystem::Instance();
	motionControlSystem.updateTicks();
    motionControlSystem.control();
	motionControlSystem.updatePosition();
	motionControlSystem.manageStop();
}

//Boucle principale, gere entre autres la communication avec le HL
void loop(){
    delay(1000);
    OrderManager& orderMgr = OrderManager::Instance();

    /*AX12 initialisation*/
    orderMgr.execute("rlbAv");
    orderMgr.execute("rlbAr");
    delay(1000);
//    orderMgr.execute("flpAv");
//    orderMgr.execute("flpAr");
//    delay(1000);

    for(int i=0;i<10;i++)
	{
		digitalWrite(30,HIGH);
		delay(200);
		digitalWrite(30,LOW);
		delay(200);
	}
	
    /* MotionControlSystem */
    IntervalTimer motionControlInterruptTimer;
    motionControlInterruptTimer.priority(253);
    motionControlInterruptTimer.begin(motionControlInterrupt, MC_PERIOD); //asservissements

//    orderMgr.execute("kpt 5.5");
//    orderMgr.execute("kit 0");
//    orderMgr.execute("kdt 0");
    orderMgr.execute("kpr 3");
    orderMgr.execute("kir 0");
    orderMgr.execute("kdr 0");
//    orderMgr.execute("ct0");
//    orderMgr.execute("cr0");
//    orderMgr.execute("monthlery");
//    orderMgr.execute("kpg 0.175");
//    orderMgr.execute("kig 0.0003");
//    orderMgr.execute("kdg 0.65"); //0.65
//    orderMgr.execute("kpd 0.18");
//    orderMgr.execute("kid 0.0003");
//    orderMgr.execute("kdd 0.45"); //0.45
//    orderMgr.execute("t pi");
    for(int i=0;i<300;i++)
    {
        if(i==20)
        {
//            orderMgr.execute("d 1000");
            orderMgr.execute("t pi");
//            orderMgr.execute("av");
//            orderMgr.execute("rawpwm 150");
        }
        orderMgr.execute("rawposdata");
        delay(10);
    }
    delay(1000);
//    orderMgr.execute("sstop");
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
    orderMgr.execute("t 0");
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
        //orderMgr.refreshUS();
        orderMgr.communicate();
        //orderMgr.sendUSData();
    }
}






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

