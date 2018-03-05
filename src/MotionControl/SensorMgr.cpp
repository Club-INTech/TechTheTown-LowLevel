#include "SensorMgr.h"

SensorMgr::SensorMgr()
{
	Wire.begin();

	/* CHANGEMENT PIN I2C */
	CORE_PIN18_CONFIG = 0;  // turn off primary pins before enable alternates
	CORE_PIN19_CONFIG = 0;
	CORE_PIN16_CONFIG = PORT_PCR_MUX(2)|PORT_PCR_ODE|PORT_PCR_SRE|PORT_PCR_DSE;
	CORE_PIN17_CONFIG = PORT_PCR_MUX(2)|PORT_PCR_ODE|PORT_PCR_SRE|PORT_PCR_DSE;

	distances.reserve(NBR_OF_US_SENSOR);

	for( uint8_t i = 0 ; i < NBR_OF_US_SENSOR ; i++ )
		US[i] = new SRF10(i,128,SRF10::GAIN::G100);

	distances.push_back(0x0000);
	distances.push_back(0x0000);
	distances.push_back(0x0000);
	distances.push_back(0x0000);
}

void SensorMgr::refresh(MOVING_DIRECTION dir)
{
	if(NBR_OF_US_SENSOR)
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
					currentMeasuringUS=NBR_OF_US_SENSOR/2;
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
				if( currentMeasuringUS < NBR_OF_US_SENSOR/2-1 )
					++currentMeasuringUS;
				else
					firstMeasure = true;

			}
			else
			{
				if( currentMeasuringUS < NBR_OF_US_SENSOR-1 )
					++currentMeasuringUS;
				else
					firstMeasure = true;
			}
		}
	}
}

//Contacteurs et Jumper

bool SensorMgr::isJumperEngaged()
{
	return digitalRead(PIN_JMPR);
}

bool SensorMgr::isCont1Engaged()
{
	return digitalRead(PIN_CONT1);
}
