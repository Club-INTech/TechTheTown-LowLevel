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
	{
		US[i] = new SRF10(i,40,SRF10::GAIN::G120);
		distances.push_back(Average<uint16_t,AVERAGE_US_SIZE>());
	}

	 //puts sensorcubeAR in sleep mode
	pinMode(PIN_CUBE_AR_DETECTION_SLEEP,OUTPUT);
	digitalWrite(PIN_CUBE_AR_DETECTION_SLEEP,LOW);

	//init sensorCubeAV at addr=0x10
	sensorCubeAV.init();
	sensorCubeAV.setAddress(CUBE_AV_DETECTION_ADDR);
	sensorCubeAV.configureDefault();
	delay(50);
	//wake up sensorcubeAR
	digitalWrite(PIN_CUBE_AR_DETECTION_SLEEP,HIGH);
	delay(50);
	//init sensorCubeAR at defaultAddress
	sensorCubeAR.init();
	sensorCubeAR.configureDefault();
}

void SensorMgr::sendUS()
{
	sendRequest=true;
}

void SensorMgr::refreshUS(MOVING_DIRECTION dir)
{
	if(NBR_OF_US_SENSOR)
	{
		if(!isMeasuring)
		{
			if( firstMeasure && sendRequest  )
			{
				sendRequest=false;
				highLevel.sendUS(distances);
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
            uint16_t currentDistance = US[currentMeasuringUS]->getDistance();
			distances[currentMeasuringUS].add(currentDistance);
            if(currentDistance <= BASIC_DETECTION_DISTANCE && currentDistance != 0 && measure_direction != MOVING_DIRECTION::NONE && isBasicDetectionOn )
            {
                MotionControlSystem::Instance().emergencyStop();
                Serial.println("HALP");
            }
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
				{
					firstMeasure = true;
				}
			}
		}
	}
}

void SensorMgr::checkCubeAV()
{
	if( sensorCubeAV.readRangeSingle() < CUBE_AV_DETECTION_RANGE_MM )
		highLevel.sendEvent("cubeDetectedAV");
	else
		highLevel.sendEvent("noCubeDetectedAV");
}

void SensorMgr::checkCubeAR()
{
	if( sensorCubeAR.readRangeSingle() < CUBE_AR_DETECTION_RANGE_MM )
	{
		highLevel.sendEvent("cubeDetectedAR");
	}
	else
	{
		highLevel.sendEvent("noCubeDetectedAR");
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

void SensorMgr::enableBasicDetection(bool newStatus)
{
    isBasicDetectionOn = newStatus;
}
