#include "SensorMgr.h"

SensorMgr::SensorMgr()
{
	Wire.begin();
	distances.reserve(NBR_OF_US_CAPTOR);
	US[0] = new SRF10(0,255,SRF10::GAIN::G100);
	distances.push_back(0x0000);
	distances.push_back(0x0000);
	distances.push_back(0x0000);
	distances.push_back(0x0000);

}

void SensorMgr::refresh()
{
	if(NBR_OF_US_CAPTOR)
	{
		if(!isMeasuring)
		{
			US[currentMeasuringUS]->request();
			isMeasuring=true;
		}
		if(US[currentMeasuringUS]->update())
		{
			distances[currentMeasuringUS] = US[currentMeasuringUS]->getDistance();
			isMeasuring=false;
			if(currentMeasuringUS<NBR_OF_US_CAPTOR-1)
				++currentMeasuringUS;
			else
			{
				highLevel.sendUS(distances);
				currentMeasuringUS=0;
			}
		}
	}
	return;
}

uint16_t SensorMgr::getUsTest()
{
	//return usTEST.value();
	return 0;
}

//Contacteurs et Jumper

bool SensorMgr::isJumpEngaged()
{
	return digitalRead(PIN_JMPR);
}

bool SensorMgr::isCont1Engaged()
{
	return digitalRead(PIN_CONT1);
}
