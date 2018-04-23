#include "SensorMgr.h"

SensorMgr::SensorMgr()
{
	Wire.begin();

	pinMode(PIN_JMPR,INPUT_PULLUP);

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
	sensorCubeAV.setAmbiantGain(VL6180X::ALS_GAIN::G40);
	delay(50);
	//wake up sensorcubeAR
	digitalWrite(PIN_CUBE_AR_DETECTION_SLEEP,HIGH);
	delay(50);
	//init sensorCubeAR at defaultAddress
	sensorCubeAR.init();
	sensorCubeAR.configureDefault();
    sensorCubeAR.setAmbiantGain(VL6180X::ALS_GAIN::G40);

	jumperPlugged = isJumperEngaged();
	basicBlocked = false;
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

			/*
			 * Gestion des fronts pour la basic detection
			 * On check les fronts montants (Ennemi qui arrive dans la zone) uniquement si on se déplace,
			 * si on est en basicDetection (Obviously) et qu'on a rien détecté précédemment.
			 *
			 * On test le front descendant à partir du moment où on a eu un front montant (Obviously).
			 * Il n'est validé que si toutes les valeur mesurées sont passées au dessus du seuil
			 */

			if(currentDistance <= BASIC_DETECTION_DISTANCE && currentDistance != 0
			   && measure_direction != MOVING_DIRECTION::NONE && isBasicDetectionOn && !basicBlocked)
            {
                basicBlocked=true;
            }
			else
			{
				for(int i=0;i<NBR_OF_US_SENSOR;i++)
				{
					if(US[i]->getDistance()<BASIC_DETECTION_DISTANCE && US[i]->getDistance() != 0)
					{
						break;
					}
					if(i==3)
					{
						basicBlocked = false;
					}
				}
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
	if( sensorCubeAV.readRangeSingle() < CUBE_AV_DETECTION_RANGE_MM || sensorCubeAV.readAmbientSingleLux() < CUBE_AV_DETECTION_AMBIANT_LUX )
	{
		highLevel.sendEvent("cubeDetectedAV");
	}
	else
	{
		highLevel.sendEvent("noCubeDetectedAV");
	}
}

void SensorMgr::checkCubeAR()
{
	if( sensorCubeAR.readRangeSingle() < CUBE_AR_DETECTION_RANGE_MM || sensorCubeAR.readAmbientSingleLux() < CUBE_AR_DETECTION_AMBIANT_LUX )
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
/**
 * Check l'état du jumper
 * @return : Vrai si le jumper est présent
 */
{
	return !digitalRead(PIN_JMPR);				// Inversé car le switch descend à faux quand il est inséré
}

bool SensorMgr::isReadyToGo()
/**
 * Vérifie si on doit lancer le match
 * @return : Vrai si on lance le match
 */
{
	if(jumperPlugged)							// Si le jumper était présent au test précédent
	{
		jumperPlugged = isJumperEngaged();
		return(!jumperPlugged);					// Alors on part si il ne l'est plus à ce test
	}
	jumperPlugged = isJumperEngaged();
	return(false);								// Sinon on ne part pas de toutes façons
}

bool SensorMgr::isCont1Engaged()
{
	return digitalRead(PIN_CONT1);
}

void SensorMgr::enableBasicDetection(bool newStatus)
{
    isBasicDetectionOn = newStatus;
}

int8_t SensorMgr::isBasicBlocked()
{
	return basicBlocked;
}

void SensorMgr::resetBasicBlocked()
{
	basicBlocked = false;
}
