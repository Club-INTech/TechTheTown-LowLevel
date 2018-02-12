#include "SensorMgr.h"

SensorMgr::SensorMgr()
{
	Wire.begin();
	distances.reserve(NBR_OF_US_CAPTOR);
	US[0] = new SRF10(0,255,SRF10::GAIN::G100);
	/*US[1] = new SRF10(0,255,SRF10::GAIN::G100);
	US[2] = new SRF10(0,255,SRF10::GAIN::G100);
	US[3] = new SRF10(0,255,SRF10::GAIN::G100);*/
	distances.push_back(0x0000);
	distances.push_back(0x0000);
	distances.push_back(0x0000);
	distances.push_back(0x0000);

}

void SensorMgr::reset_measure(uint8_t new_current_measuring_us)
{
	currentMeasuringUS = new_current_measuring_us;
	firstMeasure = true;
	highLevel.sendUS(distances);
	std::fill(distances.begin(),distances.end(),0x00);
}

void SensorMgr::refresh(MOVING_DIRECTION dir)
{
	if(NBR_OF_US_CAPTOR)
	{
		if(!isMeasuring)
		{
			US[currentMeasuringUS]->request();
			isMeasuring=true;
			if( firstMeasure )
			{
				if( dir == MOVING_DIRECTION::FORWARD || dir == MOVING_DIRECTION::NONE )
					currentMeasuringUS=0;
				else
					currentMeasuringUS=NBR_OF_US_CAPTOR/2;
				measure_direction = dir;
				firstMeasure=false;
			}
		}
		if( isMeasuring && US[currentMeasuringUS]->update())
		{
			distances[currentMeasuringUS] = US[currentMeasuringUS]->getDistance();
			isMeasuring=false;
			if( measure_direction == MOVING_DIRECTION::FORWARD )
			{
				if( currentMeasuringUS < NBR_OF_US_CAPTOR/2-1 )
					++currentMeasuringUS;
				else
					reset_measure(0);

			}
			else if( measure_direction == MOVING_DIRECTION::BACKWARD )
			{
				if( currentMeasuringUS < NBR_OF_US_CAPTOR-1 )
					++currentMeasuringUS;
				else
					reset_measure(NBR_OF_US_CAPTOR/2);
			}
			else
			{
				if( currentMeasuringUS < NBR_OF_US_CAPTOR-1)
					++currentMeasuringUS;
				else
					reset_measure(0);
			}
		}
	}
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
