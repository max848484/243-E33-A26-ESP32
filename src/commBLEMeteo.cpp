/*
  Fichier commBLEMeteo.cpp

  Fichier source utilisé pour le décodage des messages qui sont nécessaires au contrôle du service 
  météo via le BLE sur l'ESP32-C3. 
*/
#include "commBLEMeteo.h"

typeMesure monTypeService = NON_DEFINI;


/*
  La fonction analyseTrame() analyse le message rxMessage reçu.
  Elle retourne 0 en cas de succès, autrement un nuéro d'erreur est retourné
*/
uint8_t analyseTrame(char *rxMessage){
  erreurTrame monErreur = SUCCES;
  uint16_t index = 0;
  etatTrame monEtat = ETAT_INIT;
  uint8_t longueurChamp = 0;
  char nomEquipe[RX_BUFFER_SIZE] = {'\0'};
  bool analyseComplete = false;
  char strValue[RX_BUFFER_SIZE] = {'\0'};
  int value = 0;
  typeMesure typeData = NON_DEFINI;

  while(rxMessage[index] != '\0'){


    switch(monEtat){
      case ETAT_INIT:
        if(rxMessage[index] == '$'){
          monEtat = ETAT_DEBUT_TROUVE;
          printf("Début de trame trouvé\n");
        }
        else{
          // on reste dans l'état ETAT_INIT, on va continuer à chercher le début de trame
        }
        break;

      case ETAT_DEBUT_TROUVE:
        monErreur = supprimeSeparateur(rxMessage, index, &longueurChamp);
        if (monErreur != SUCCES){
          break; // on a rencontré une erreur, on sort.
        }

        if (strcmp(&rxMessage[index], "CONFIG") == 0){
          printf("Trame CONFIG reçue\n");
          index = index + longueurChamp;
          monEtat = ETAT_CONFIG;
        }
        else if(strcmp(&rxMessage[index], "DATA") == 0){
          index = index + longueurChamp;
          monEtat = ETAT_DATA;
        }
        else{
          printf("Erreur de nom de trame");
          monErreur = ERR_NOM_COMMANDE;
        }
        break;

      case ETAT_CONFIG:
        monErreur = supprimeSeparateur(rxMessage, index, &longueurChamp);
        if (monErreur != SUCCES){
          break; // on a rencontré une erreur, on sort.
        }

        if (strcmp(&rxMessage[index], "START") == 0){
          printf("Trame START reçue\n");
          index = index + longueurChamp;
          monEtat = ETAT_START;
        }
        else if(strcmp(&rxMessage[index], "STOP") == 0){
          index = index + longueurChamp;
          monEtat = ETAT_STOP;
        }
        else{
          printf("Erreur de nom de trame");
          monErreur = ERR_NOM_COMMANDE;
        }
        break;

      case ETAT_START:
        monErreur = supprimeSeparateur(rxMessage, index, &longueurChamp);
        if (monErreur != SUCCES){
          break; // on a rencontré une erreur, on sort.
        }

        strncpy(nomEquipe, &rxMessage[index], longueurChamp);
        index = index + longueurChamp;
        printf("strncpy complété !\n");
        monEtat = ETAT_CONFIG_TYPE;
        break;

      case ETAT_CONFIG_TYPE:
        monErreur = supprimeSeparateur(rxMessage, index, &longueurChamp);
        if (monErreur != SUCCES){
          break; // on a rencontré une erreur, on sort.
        }

        if (strcmp(&rxMessage[index], "TEMP") == 0){
          monTypeService = TEMP;
          index = index + longueurChamp;
          monEtat = ETAT_CONFIG_CHECKSUM;
        }
        else if (strcmp(&rxMessage[index], "VENT_VIT") == 0){
          monTypeService = VENT_VIT;
          index = index + longueurChamp;
          monEtat = ETAT_CONFIG_CHECKSUM;
        }
        else{
          monErreur = ERR_TYPE_MESURE;
        }

        break;

      case ETAT_CONFIG_CHECKSUM:

        if (false == verifyChecksum(rxMessage, index)){
          monErreur = ERR_CHECKSUM;
        }
        else{
          monEtat = ETAT_START_BLE;
        }
        break;
      case ETAT_START_BLE:

        initBLE(nomEquipe);


        if (TEMP == monTypeService){
          startBleTempera();
        }
        else if (VENT_VIT == monTypeService){
          startBleVentVit();
        }
        else if (VENT_DIR == monTypeService){
          startBleVentDir();
        }
        else if (PLUIE == monTypeService){
          startBlePluie();
        }
        else if (LUM == monTypeService){
          startBleLumino();
        }    
        analyseComplete = true;
        break;
        
      case ETAT_DATA:

        if (NON_DEFINI == monTypeService){
          monErreur = ERR_BLE_NOT_STARTED;
          break;
        }

        monErreur = supprimeSeparateur(rxMessage, index, &longueurChamp);
        if (monErreur != SUCCES){
          break; // on a rencontré une erreur, on sort.
        }        

        if (strcmp(&rxMessage[index], "TEMP") == 0){
          if (TEMP != monTypeService){
            monErreur = ERR_SERVICE_NOT_STARTED;
            break;
          }
          typeData = TEMP;
          index = index + longueurChamp;
          monEtat = ETAT_VALEUR;
        }
        else if(strcmp(&rxMessage[index], "HUM") == 0){
          if (TEMP != monTypeService){
            monErreur = ERR_SERVICE_NOT_STARTED;
            break;
          }          
          typeData = HUM;
          index = index + longueurChamp;
          monEtat = ETAT_VALEUR;
        }
        else if(strcmp(&rxMessage[index], "VENT_VIT") == 0){
          if (VENT_VIT != monTypeService){
            monErreur = ERR_SERVICE_NOT_STARTED;
            break;
          }          
          index = index + longueurChamp;
          monEtat = ETAT_VALEUR;
        }
        else if(strcmp(&rxMessage[index], "VENT_DIR") == 0){
          if (VENT_DIR != monTypeService){
            monErreur = ERR_SERVICE_NOT_STARTED;
            break;
          }              
          index = index + longueurChamp;
          monEtat = ETAT_VALEUR;
        }
        else if(strcmp(&rxMessage[index], "PLUIE") == 0){
          if (PLUIE != monTypeService){
            monErreur = ERR_SERVICE_NOT_STARTED;
            break;
          }           
          index = index + longueurChamp;
          monEtat = ETAT_VALEUR;
        }
        else if(strcmp(&rxMessage[index], "LUM") == 0){
          if (LUM != monTypeService){
            monErreur = ERR_SERVICE_NOT_STARTED;
            break;
          }             
          index = index + longueurChamp;
          monEtat = ETAT_VALEUR;
        }    
        else{
          monErreur = ERR_TYPE_MESURE;
        }    
        
        break;
      case ETAT_VALEUR:
        monErreur = supprimeSeparateur(rxMessage, index, &longueurChamp);
        if (monErreur != SUCCES){
          break; // on a rencontré une erreur, on sort.
        }

        strncpy(strValue, &rxMessage[index], longueurChamp);
        index = index + longueurChamp;
        monEtat = ETAT_DATA_CHECKSUM;
        break;      

      case ETAT_DATA_CHECKSUM:

        if (false == verifyChecksum(rxMessage, index)){
          monErreur = ERR_CHECKSUM;
        }
        else{
          monEtat = ETAT_SET_VALUE;
        }
        break;

      case ETAT_SET_VALUE:
        value = (int) strtol(strValue, NULL, 16);
        if (TEMP == monTypeService){
          if (TEMP == typeData){
            setBleTempera(value);
          }
          else if (HUM == typeData){
            //setBleHum(value);
          }
        }
        else if (VENT_VIT == monTypeService){
          setBleVentVit(value);
        }
        else if (VENT_DIR == monTypeService){
          setBleVentDir(value);
        }        
        else if (PLUIE == monTypeService){
          setBleVentDir(value);
        }    
        else if (LUM == monTypeService){
          setBleVentDir(value);
        }    
        analyseComplete = true;
        break;          
        
      default:
        break;
    }

    index++;

    if (index >= RX_BUFFER_SIZE) {
      monErreur = ERR_FIN_TRAME1; // erreur de fin de trame
    }

    if ((monErreur != SUCCES) || (true == analyseComplete)) {
      // Dans un cas ou l'autre, on sort de la boucle while
      break;
    }
  }

  printf("sortie de la fonction analyseTrame, monErreur == %d\n", monErreur);
  return(monErreur);
}

/*
  Cette fonction trouve le prochain séprateur (point-virgule ou *) dans le message rxMessage et le 
  remplace par '\0' (fin de chaîne).
  Paramètres:
    - rxMessage: le message à analyser
    - index: l'index de départ pour la recherche du séparateur
    - longueurChamp: un pointeur vers une variable qui contiendra la longueur du champ trouvé
  Retourne erreurTrame, SUCCES si un séparateur a été trouvé, sinon ERR_FIN_TRAME1
*/
erreurTrame supprimeSeparateur(char *rxMessage, uint16_t index, uint8_t *longueurChamp){
  erreurTrame monErreur;

  index = index + 1;
  *longueurChamp = 1;

  while((rxMessage[index] != ';') && (rxMessage[index] != '*') && (index < RX_BUFFER_SIZE)){
    index = index + 1;
    *longueurChamp = *longueurChamp + 1;
  }

  if ((rxMessage[index] == ';') || (rxMessage[index] == '*')){
    rxMessage[index] = '\0';
    monErreur = SUCCES;
  }
  else{
    // on a atteint la fin du buffer sans trouver de séparateur
    monErreur = ERR_FIN_TRAME1;
  }

return monErreur;
}


/*
  Cette fonction récupère le checksum reçu, calcul le checksum attendu et détermine si la trame est 
  valide.
  Paramètres:
    - rxMessage: le message à analyser
    - index: l'index de départ pour la recherche du séparateur  
  retourne true si le checksum est valide, false dans le cas contraire.
  
*/
bool verifyChecksum(char *rxMessage, uint16_t indexFin){

  if ((indexFin + 1) >= RX_BUFFER_SIZE){
    //Return erreur fin de trame
    return false;
  }

  char ccheckSumReceived[3] = {rxMessage[indexFin], rxMessage[indexFin+1], '\0'};
  int icheckSumReceived = (int) strtol(ccheckSumReceived, NULL, 16);
  int icheckSumComputed = 0;
  uint16_t i = 0;

  while('$' != rxMessage[i]){
    i = i +1;
  }

  //Une fois le '$' trouvé, on incrémente l'index de 1 puisqu'on l'exclue du calcul
  i = i +1; 


  while(i <= (indexFin-2)){
    icheckSumComputed ^= rxMessage[i];
    i = i +1;
  }

  if (icheckSumComputed == icheckSumReceived){
    return true;
  }
  else{
    //printf("\nchecksum false !  icheckSumComputed =%i -  icheckSumReceived = %i", icheckSumComputed, icheckSumReceived);
    return false;
  }

}


