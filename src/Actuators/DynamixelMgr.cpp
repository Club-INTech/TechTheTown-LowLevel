//
// Created by trotfunky on 18/12/17.
//

#include "DynamixelMgr.h"
#include "ActuatorValues.h"

DynamixelMgr::DynamixelMgr() : serialAX(DynamixelInterface(Serial4)) // Utilise la série intialisée dans la bibliothèque Dynamixel
{                                                                    // SerialY correspond aux pins RXY/TXY
    serialAX.begin(9600);               // Démarre la série

    // Ajouter les actionneurs ici

    // FRONT
    addAX12Group();                                                             // Groupe du bras
    populateAX12Group(frontArmGroup_ID,frontLeftAX12_ID,armBaseSpeed,BASE);
    populateAX12Group(frontArmGroup_ID,frontRightAX12_ID,armBaseSpeed,MIRROR);
    addAX12(frontDoor_ID,doorBaseSpeed);                                        // Porte

    // BACK
    addAX12Group();                                                             // Groupe du bras
    populateAX12Group(backArmGroup_ID,backLeftAX12_ID,armBaseSpeed,BASE);
    populateAX12Group(backArmGroup_ID,backRightAX12_ID,armBaseSpeed,MIRROR);
    addAX12(backDoor_ID,doorBaseSpeed);                                         // Porte
}

DynamixelMgr::~DynamixelMgr()
{
}

bool DynamixelMgr::addAX12(int id)
/**
 * Ajoute un AX12
 * @param id : ID du nouvel AX12
 * @return : Faux si l'AX12 existe déjà
 */
{
    if(checkIfAX(id))                       // Vérifie si il n'existe pas déjà
    {
        return(false);
    }
    axList.insert({id,new DynamixelMotor(serialAX,id)});
    axList.at(id)->init();                  // Initialisation des AX12 pour les utiliser correctement
    axList.at(id)->enableTorque();
    axList.at(id)->jointMode();
    return(true);
}
bool DynamixelMgr::addAX12(int id, uint16_t speed)
/**
 * Initialise l'AX avec une vitesse définie
 * @param id : ID de l'AX12 à rajouter
 * @param speed : Nouvelle vitesse de l'AX12
 * @return : Faux si l'AX12 existe déjà
 */
{
    if(!this->addAX12(id))
    {
        return(false);
    }
    axList.at(id)->speed(speed);
    return(true);
}

void DynamixelMgr::addAX12Group()
/**
 * Créé un nouveau groupe _VIDE_
 */
{
    DynamixelGroup tempo;
    axGroupsList.push_back(tempo);
}

bool DynamixelMgr::populateAX12Group(unsigned int groupId, int motorId, DynSym symmetry)
/**
 * Rajoute un AX12 dans le groupe avec une symétrie particulière
 * @param groupId : ID du groupe à peupler
 * @param motorId : ID de l'AX12 a rajouter
 * @param symmetry : Mode de symétrie du moteur à rajouter
 * @return : Faux si l'AX12 n'a pas put être ajouté
 */
{
    if(!addAX12(motorId))                   // Si l'AX12 existe déjà, on ne le rajoute pas au groupe
    {
        return(false);
    }
    axGroupsList.at(groupId).add(axList.at(motorId),symmetry);
    return(true);
}

bool DynamixelMgr::populateAX12Group(unsigned int groupId, int motorId, uint16_t speed, DynSym symmetry)
/**
 * Rajoute un AX12 au groupe avec une vitesse précise
 * @param groupId : ID du groupe à peupler
 * @param motorId : ID de l'AX12 à rajouter
 * @param speed : Vitesse de l'AX12
 * @param symmetry : Mode de symétrie du moteur à ajouter
 * @return : Faux si l'AX12 n'a pas put être ajouté
 */
{
    if(!this->populateAX12Group(groupId, motorId, symmetry))
    {
        return(false);
    }
    axGroupsList.at(groupId).speed(speed);
    return(true);

}

bool DynamixelMgr::movAX12(int id, uint16_t angleDegree)
/**
 * Déplace un AX12 vers une position
 * @param id : ID de l'AX12
 * @param angleDegree : Angle ciblé
 * @return : False si l'AX12 n'existe pas
 */
{
    if(!checkIfAX(id))
    {
        return(false);
    }
    axList.at(id)->goalPositionDegree(angleDegree);
    return(true);
}
bool DynamixelMgr::setAX12Speed(int id, uint16_t speed)
/**
 * Change la vitesse d'un AX12
 * @param id : ID de l'AX12
 * @param speed : Nouvelle vitesse de l'AX12
 * @return : False si l'AX12 n'existe pas
 */
{
    if(!checkIfAX(id))
    {
        return(false);
    }
    axList.at(id)->speed(speed);
    return(true);
}

bool DynamixelMgr::movAX12G(unsigned int groupId, uint16_t angleDegree)
/**
 * Déplace tout un groupe d'AX12 vers un angle
 * @param groupId : ID du groupe
 * @param angleDegree : Angle ciblé
 * @return : Faux si le groupe n'existe pas
 */
{
    if(groupId>=axGroupsList.size())
    {
        return(false);
    }
    axGroupsList.at(groupId).goalPositionDegree(angleDegree);
    return(true);
}

bool DynamixelMgr::setAX12GSpeed(unsigned int groupId, uint16_t speed)
/**
 * Change la vitesse de tout les AX12 d'un groupe
 * @param groupId : ID du groupe
 * @param speed : Nouvelle vitesse
 * @return : Faux si le groupe n'existe pas
 */
{
    if(groupId>=axGroupsList.size())
    {
        return(false);
    }
    axGroupsList.at(groupId).speed(speed);
    return(true);
}


bool DynamixelMgr::checkIfAX(int id)
/**
 * Vérifie si l'AX12 est dans la liste
 * @param id : ID de l'AX12 à vérifier
 * @return : Faux si l'AX12 n'est pas présent
 */
{
    return(axList.count(id)!=0);
}
