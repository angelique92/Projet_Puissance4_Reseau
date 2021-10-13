#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "joueur.h"

/************************************************************
    set_fd
 Fonction qui associe la socket du client dans la structure joueur.
************************************************************/
void set_fd( struct joueur *j, int f){
    j->fd=f;
}

/************************************************************
    get_fd
 Fonction qui renvoie la socket d'un joueur.
************************************************************/
int get_fd( struct joueur *j){
    return j->fd;
}

/************************************************************
    set_pseudo
 Fonction qui memorise le pseudo dans la structure joueur.
************************************************************/
void set_pseudo(struct joueur *j, char *pseudo){
    char * copy;
    size_t  length;

    length= strlen(pseudo)+1;
    copy = (char *) malloc( length * sizeof(char) );
    assert( copy != NULL );
    memcpy( copy, pseudo, length );
    
    j->pseudo=copy;
}

/************************************************************
    free_joueur
 Fonction qui libère l'espace allouer par malloc.
************************************************************/
void free_joueur(struct joueur *j){
     /* Toujours libérer la mémoire après utilisation */
    free( j->pseudo );
}

/************************************************************
    get_pseudo
 Fonction qui renvoie le pseudo du joueur.
************************************************************/
char * get_pseudo(struct joueur *j){

    return j->pseudo;
    
}
/************************************************************
    set_couleur
 Fonction qui associe la couleur à la structure d'un joueur.
************************************************************/
void set_couleur(struct joueur *j, unsigned char couleur){

    j->couleur=couleur;

}
/************************************************************
    get_couleur
 Fonction qui renvoie la couleur d'un joueur.
************************************************************/
unsigned char get_couleur(struct joueur *j){

    return j->couleur;
    
}
/*
int main (){
   printf(" dans main \n");
    
    struct joueur j;
    
    set_pseudo(&j);
    
    //printf("Mon pseudo est %s\n", get_pseudo(&j));


}*/