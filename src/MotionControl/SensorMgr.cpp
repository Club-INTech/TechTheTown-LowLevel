#include "SensorMgr.h"

SensorMgr::SensorMgr()
{

	Wire.begin(I2C_MASTER,0x00,16,17);
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

void SensorMgr::refresh(MOVING_DIRECTION dir)
{
	if(NBR_OF_US_CAPTOR)
	{
		if(!isMeasuring)
		{
			if( firstMeasure )
			{
				highLevel.sendUS(distances);
				std::fill(distances.begin(),distances.end(),0x00);
				if( dir == MOVING_DIRECTION::FORWARD || dir == MOVING_DIRECTION::NONE )
					currentMeasuringUS=0;
				else
					currentMeasuringUS=NBR_OF_US_CAPTOR/2;
				measure_direction = dir;
				firstMeasure=false;
			}
			US[currentMeasuringUS]->request();
			isMeasuring=true;
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
					firstMeasure = true;

			}
			else
			{
				if( currentMeasuringUS < NBR_OF_US_CAPTOR-1 )
					++currentMeasuringUS;
				else
					firstMeasure = true;
			}
		}
	}
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
