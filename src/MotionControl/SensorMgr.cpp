#include "SensorMgr.h"

SensorMgr::SensorMgr()
{
	Wire.begin();
	distances.reserve(NbrOfUSCaptor);
	for(uint8_t i=0;i<NbrOfUSCaptor;i++)
	{
		US[i] = new SRF10(i,255,SRF10::GAIN::G100);
		distances.push_back(0xFFFF);
	}

}

void SensorMgr::refresh()
{
	if(NbrOfUSCaptor)
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
			if(currentMeasuringUS<NbrOfUSCaptor-1)
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
