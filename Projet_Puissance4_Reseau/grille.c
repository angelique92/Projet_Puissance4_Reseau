#include "grille.h"
#include <stdio.h>
#include <stdbool.h>

/**************************************************************************************
    init_grille
 Fonction qui permet d'initialiser toutes les cases de la grille a 2. ( Case Vide )
***************************************************************************************/
void init_grille (unsigned char *grille){

    for (int k=0 ; k< TAILLE_GRILLE; k++){
       grille[k]=2;
    }
}

/************************************************
    affiche_grille
 Fonction qui permet d'afficher la grille
************************************************/
void affiche_grille( unsigned char *grille){

    for (int i= TAILLE_LIGNE-1; i>=0; i--){
        printf("%d\t",i);
        for (int j=0; j< TAILLE_COLONNE; j++){
            int casee = j*TAILLE_LIGNE+i;
            switch (grille[casee]){
            case JOUEUR_ROUGE:
                printf("| R\t");
                break;
            case JOUEUR_JAUNE:
                printf("| J\t");
                break;
            default :
                printf("| \t");
                break;
            }
        }
        printf("| \n");
    }
    printf("\n\t");
    for(int k=0; k< TAILLE_COLONNE;k++){
        printf("  %d\t",k);
    }
    printf("\n\n");
}

/********************************************************************************************
    ajout_coup
 Fonction qui permet d'ajouter le coup d'un joueur dans la colonne sélectionnée dans la grille
*********************************************************************************************/
void ajout_coup (unsigned char colonne,unsigned char joueur, unsigned char *grille){
    
    for (int ligne=0; ligne <TAILLE_LIGNE; ligne++){
        int casee = (colonne*TAILLE_LIGNE)+ligne;
        if( grille[casee] == JOUEUR_VIDE){
            grille[casee]=joueur;
            
            return;
        }
    }
   
}
/********************************************************************************************
    coup_gagnee
 Fonction booléenne, qui renvoie vrai si le joueur donner en parametre a gagner le jeu c'est a dire :
 Le joueur a gagner en ligne, en colonne ou en diagonale.
 Et la fonction renvoie faux si le joueur n'a pas gagné.
*********************************************************************************************/
_Bool coup_gagnee(unsigned char * grille,unsigned char colonne, unsigned char couleur){

    return (is_win_colonne(grille,colonne,couleur) || is_win_ligne(grille,colonne,couleur) || is_win_diagonal(grille,colonne,couleur));
}

/********************************************************************************************
    is_win_ligne
 Fonction booléenne, qui renvoie vrai si le joueur a 4 pions de sa couleur alignés en ligne.
*********************************************************************************************/
_Bool is_win_ligne( unsigned char * grille,unsigned char colonne, unsigned char couleur){
    int trouve=0;
    int ligne;

    for (int i=TAILLE_LIGNE-1; i>= 0;i-- ){
        int casee = colonne*TAILLE_LIGNE+i;
            if ( grille[casee] != 2){
                ligne=i;
                break;
            }
    }
     for (int i=0; i< TAILLE_COLONNE;i++){
         int casee = i*TAILLE_LIGNE+ligne;
         if( grille[casee] == couleur){
                trouve++;
                if( trouve==4){
                    return true;
                    
                }
        }else{
            trouve=0;
        }
        
    }
    return false;

}

/********************************************************************************************
    is_win_colonne
 Fonction booléenne, qui renvoie vrai si le joueur a 4 pions de sa couleur alignés en colonne.
*********************************************************************************************/
_Bool is_win_colonne( unsigned char * grille,int colonne, unsigned char couleur){
    int trouve=0;

    for (int i= TAILLE_LIGNE-1; i>=0; i--){
            int casee = colonne*TAILLE_LIGNE+i;

            if( grille[casee] == couleur){
                trouve++;
                if( trouve==4){
                    return true;
                    
                }
            }else{
                trouve=0;
                
            }
        }
    return false;
}

/********************************************************************************************
    is_win_diagonal
 Fonction booléenne, qui renvoie vrai si le joueur a 4 pions de sa couleur alignés en diagonale.
*********************************************************************************************/
_Bool is_win_diagonal( unsigned char * grille,int colonne, unsigned char couleur){
    int trouve=0;
    int ligne, i;
    int casee ;

    for ( i=TAILLE_LIGNE-1; i>= 0;i-- ){
        casee = colonne*TAILLE_LIGNE+i;
       
            if ( grille[casee] != 2){
                ligne=i;
                break;
            }
    }
    casee= colonne*TAILLE_LIGNE+ligne;
    
    // DIAGONAL DE GAUCHE VERS DROITE
    for (i= casee%7 ; i < TAILLE_GRILLE; i=i+7){
        if( grille[i] == couleur){
                trouve++;
                if( trouve==4){
                    return true;        
                }
        }else{
            trouve=0;
                
        }
    }
//printf(" ON A PAS TROUVER DE DIAGONAL DE GAUCHE VERS DROITE\n");

    //DIAGONAL DE DROITE  VERS GAUCHE
    while ( casee % 6 != 0 ){
        casee=casee+5;
    }
    for ( i= casee; i>= 0; i=i-5){
         if( grille[i] == couleur){
                trouve++;
                if( trouve==4){
                    return true;        
                }
        }else{
            trouve=0;
                
        }
    }
    
    return false;
}
/********************************************************************************************
    grille_remplie
 Fonction booléenne, qui renvoie vrai si la grille est remplie. 
*********************************************************************************************/
_Bool grille_remplie(unsigned char *gril){
    for ( int i= 0; i < TAILLE_GRILLE; i++){
        if( gril[i] == JOUEUR_VIDE){
            return false;
        }
    }
    return true;
}



//PROGRAMME DE TEST 

/*
int main (){

unsigned char gril[TAILLE_GRILLE];
init_grille(gril);
affiche_grille(gril);
ajout_coup(0,1,gril);
ajout_coup(0,1,gril);
ajout_coup(0,1,gril);
ajout_coup(0,0,gril);

ajout_coup(1,0,gril);
ajout_coup(1,1,gril);


ajout_coup(2,0,gril);
ajout_coup(2,0,gril);
ajout_coup(2,1,gril);

ajout_coup(3,0,gril);
ajout_coup(3,0,gril);
ajout_coup(3,0,gril);
ajout_coup(3,1,gril);
ajout_coup(3,1,gril);


ajout_coup(0,1,gril);
ajout_coup(0,1,gril);
ajout_coup(1,1,gril);
ajout_coup(2,1,gril);
ajout_coup(3,1,gril);
ajout_coup(0,1,gril);
 //is_win_colonne( gril,0,1);
 //is_win_ligne(gril,1,1); 
 

affiche_grille(gril);
//is_win_ligne(gril,3,1);
//is_win_colonne(gril,3,1);
//is_win_diagonal(gril,1,0);

    if( coup_gagnee(gril,1,1))
        printf(" vainqueur\n");
    else    printf("partie continue\n");
//is_win_diagonal( gril ,1,1);
//is_win_diagonal( gril ,1,0);
}*/
