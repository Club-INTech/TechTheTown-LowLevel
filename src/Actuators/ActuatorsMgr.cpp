// 
// 
// 

#include "ActuatorsMgr.h"

ActuatorsMgr::ActuatorsMgr() : dynamixelMgr(DynamixelMgr::Instance())
{
    isPumpOn = false;
    isElecVanneAVOpen = false;
    isElecVanneAROpen = false;
}

ActuatorsMgr::~ActuatorsMgr()
{
}


void ActuatorsMgr::movAX12(int id, uint16_t angleDegree)
{
	dynamixelMgr.movAX12(id, angleDegree);
}
void ActuatorsMgr::setAX12Speed(int id, uint16_t speed)
{
	dynamixelMgr.setAX12Speed(id, speed);
}

void ActuatorsMgr::movAX12G(unsigned int groupId, uint16_t angleDegree)
{
	dynamixelMgr.movAX12G(groupId, angleDegree);
}

void ActuatorsMgr::setAX12GSpeed(unsigned int groupId, uint16_t speed)
{
	dynamixelMgr.setAX12GSpeed(groupId, speed);
}

bool ActuatorsMgr::togglePumpState(bool newState)
/**
 * Change le mode de la pompe, sous condition
 * @param newState : État désiré de la pompe
 * @return : Faux si la pompe ne peut pas être allumée
 */
{
    if (newState && (isElecVanneAVOpen+isElecVanneAROpen))  // Si on veut allumer la pompe avec des électrovannes ouvertes
    {
        digitalWrite(PIN_PWM_POMPE,HIGH);
        isPumpOn = true;
        return(true);                                       // On allume la pompe
    }
    else
    {
        digitalWrite(PIN_PWM_POMPE,LOW);
        isPumpOn = false;                                   // Sinon on l'éteint
        return(isElecVanneAVOpen+isElecVanneAROpen);        // Et on retourne vrai si les électrovannes étaient ouvetes
    }
}

void ActuatorsMgr::setElecVanneAV(bool newState)
/**
 * On change l'état de  l'électrovanne avant, en coupant la pompe si besoin
 * @param newState : Nouvel état
 */
{
    if (newState)                                   // Si on veut l'allumer, pas de vérifications
    {
        digitalWrite(PIN_ELECTROVANNE_AV,HIGH);
        isElecVanneAVOpen = true;
    }
    else
    {
        if(isPumpOn && !isElecVanneAROpen)          // Si on veut l'éteindre, on vérifie que ce n'est pas la dernière
        {
            digitalWrite(PIN_PWM_POMPE,LOW);        // Si oui, on éteint la pompe
            isPumpOn = false;
        }
        digitalWrite(PIN_ELECTROVANNE_AV,LOW);
        isElecVanneAVOpen = false;
    }
}
void ActuatorsMgr::setElecVanneAR(bool newState)
/**
 * On change l'état de l'électrovanne arrière, en coupant la pompe si besoin
 * @param newState : Nouvel état
 */
{
    if (newState)                                   // Si on veut l'allumer, pas de vérifications
    {
        digitalWrite(PIN_ELECTROVANNE_AR,HIGH);
        isElecVanneAROpen = true;
    }
    else
    {
        if(isPumpOn && !isElecVanneAVOpen)          // Si on veut l'éteindre, on vérifie que ce n'est pas la dernière
        {
            digitalWrite(PIN_PWM_POMPE,LOW);        // Si oui, on éteint la pompe
            isPumpOn = false;
        }
        digitalWrite(PIN_ELECTROVANNE_AR,LOW);
        isElecVanneAROpen = false;
    }
}
