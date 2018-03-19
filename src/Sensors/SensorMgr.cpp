#include "SensorMgr.h"


void PCCubeAVInterrupt(){
	static SensorMgr& sensorMgr = SensorMgr::Instance();
	sensorMgr.refreshPCAV();
}

void PCCubeARInterrupt(){
	static SensorMgr& sensorMgr = SensorMgr::Instance();
	sensorMgr.refreshPCAR();
}

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
		US[i] = new SRF10(i,40,SRF10::GAIN::G120);

	distances.push_back(0x0000);
	distances.push_back(0x0000);
	distances.push_back(0x0000);
	distances.push_back(0x0000);

	//PC_cube_av = new PassageCounter(40,10,-10);
	//PC_cube_ar = new PassageCounter(40,10;-10);

	m_timer_update_PC_cube_AV.priority(250);
	m_timer_update_PC_cube_AR.priority(250);
}

void SensorMgr::refreshUS(MOVING_DIRECTION dir)
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

void SensorMgr::enableCheckPCAV()
{
	//m_timer_update_PC_cube.begin(PCCubeAVInterrupt,50000);
}

void SensorMgr::enableCheckPCAR()
{
	//m_timer_update_PC_cube_AR.begin(PCCubeARInterrupt,50000);
}

void SensorMgr::disableCheckPC()
{
	m_timer_update_PC_cube_AV.end();
	m_timer_update_PC_cube_AR.end();
}

void SensorMgr::refreshPCAV()
{
	/*
	if(PC_cube_av->update()){
		disableCheckPC();
		highLevel.sendEvent("cubeDetectedAV");
	}
	*/
}

void SensorMgr::refreshPCAR()
{
	/*
	if(PC_cube_ar->update()){
		disableCheckPC();
		highLevel.sendEvent("cubeDetectedAR");
	}
	*/
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
