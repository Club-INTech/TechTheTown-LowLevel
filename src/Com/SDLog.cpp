//
// Created by Teo-CD on 22/04/18.
//

#include "SDLog.h"


SDLog::SDLog()
/**
 * Initialise la carte SD:
 * Ouvre la communication avec la carte SD
 * Supprime les fichiers précédents et en crééant de nouveaux, vides
 * Si l'initialisation échoue, on bloque les écritures
 **/
{
    if(!SD.begin(BUILTIN_SDCARD))
    {
        Serial.println("ERROR::SDCARD::Could not initialize SD card");
        sdStatus = false;
    }
    else
    {
        sdStatus = true;
        for(const char* fileName: fileList)
        {
            SD.remove(fileName);
            File tempCreation = SD.open(fileName,FILE_WRITE);
            if(!tempCreation)
            {
                Serial.printf("ERROR:SDCARD::Could not create file %s\n", fileName);
            }
            tempCreation.close();
        }
    }
    Serial.println("SD card ready");

    startingTime = 0;
}

bool SDLog::logWrite(const String message)
/**
 * Écrit le message dans le fichier correspondant à son canal et dans
 * le fichier général, ou seulement le fichier général si il n'a pas de header
 * @param message: message à logger
 * @return Vrai si on a bien réussi à écrire, faux sinon ou si la carte SD n'est pas init
 **/
{
    if(!sdStatus)
        return(false);

    if(message.length() >= 6)
    {
        if(message.substring(0,5).equals("Reçu: "))
        {
            if(!logToFile(message,fileList[1]))
            {
                return(false);
            }
        }
    }
    if(message.length() >= 2)
    {
        for(int i = 0;i<6;i++)
        {
            if(message[0] == headerList[i][0] && message[1] == headerList[i][1])
            {
                if(!logToFile(message.substring(2),fileList[i+2]))      // Permet de garder le fichier général
                {                                                       // en indice 0 et la lecture en indice 1,
                    return(false);                                      // pour éventuellement rajouter
                }                                                       // d'autres canaux en changeant le minimum
                return(logToFile(message.substring(2),fileList[0]));
            }
        }
    }
    return(logToFile(message,fileList[0]));
}

bool SDLog::logWrite(const char* message)
{
    return(logWrite(String(message)));
}

bool SDLog::logWriteReception(const char* message)
/**
 * Permet d'écrire dans le fichier "input.txt" et le fichier général les messages
 * reçus, en ajoutant un indicateur de la réception
 * @param message : Message reçu a logger
 * @return Vrai si l'écriture a réussi
 **/
{
    if(!sdStatus)
        return(false);

    String receptionMessage = String("Reçu: ").append(message);
    if(!logToFile(receptionMessage,fileList[1]))
    {
        return(false);
    }
    return logToFile(receptionMessage,fileList[0]);
}

bool SDLog::logToFile(const String &message,const char* logFile)
/**
 * Gère l'écriture dans le fichier, permet de savoir si elle a été réussie et
 * ajoute le timestamp à la ligne de log
 * @param message : message à logger
 * @param logFile : fichier de log correspondant
 * @return Vrai si on a réussi à écrire dans le fichier
 **/
{
    File outputFile = SD.open(logFile,FILE_WRITE);
    if(!outputFile)
    {
        Serial.printf("ERROR::SDCARD::Could not open file %s\n",logFile);
        return(false);
    }
    outputFile.printf("[Time: %lu] ",millis()-startingTime);
    outputFile.println(message);
    outputFile.close();
    return(true);
}

void SDLog::setStartingTime()
{
    startingTime = millis();
}