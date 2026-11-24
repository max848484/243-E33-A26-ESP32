/*
  Fichier commBLEMeteo.cpp

  Fichier source utilisé pour le décodage des messages qui sont nécessaires au contrôle du service 
  météo via le BLE sur l'ESP32-C3. 
*/
#include "commBLEMeteo.h"




/*
  La fonction analyseTrame() analyse le message rxMessage reçu.
  Elle retourne 0 en cas de succès, autrement un nuéro d'erreur est retourné
*/
uint8_t analyseTrame(char *rxMessage){
  erreurTrame monErreur = SUCCES;
  uint16_t index = 0;
  etatTrame monEtat = ETAT_INIT;
  uint8_t longueurChamp = 0;

  while((rxMessage[index] != '\0') && (index < RX_BUFFER_SIZE)){


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



      default:
        break;
    }

    index++;

    if (monErreur != SUCCES) {
      // on a rencontré une erreur, on sort de la boucle while
      break;
    }
  }

  if (index >= RX_BUFFER_SIZE) {
    monErreur = ERR_FIN_TRAME1; // erreur de fin de trame
  }
  
  printf("sortie de la fonction analyseTrame\n");
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

