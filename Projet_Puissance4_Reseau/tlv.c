#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "joueur.h"
#include "tlv.h"
#include "grille.h"
#include "utils.h"


/************************************************
    write_tlv
 Fonction qui écrit un TLV sur une socket . 
************************************************/
ssize_t write_tlv(int fd,  void *buf){
    size_t count;
    int rc;
    unsigned char *bufptr=(unsigned char*)buf;
    count=bufptr[1]+2;

    rc=write_all( fd,buf,  count);
    return rc;
}
/************************************************
    read_tlv
 Fonction qui lis un TLV sur une socket . 
************************************************/
ssize_t read_tlv(int fd, void *buf){
    int rc;
    unsigned char *bufptr=(unsigned char*)buf;
    init_buf(buf, TAILLE_BUF);

    rc=read_all( fd, bufptr,  2);

    if( is_discon(buf)){
        //printf("LE CLIENT A RECU UN TLV DISCON\n");
        printf("L’adversaire a été déconnecté du serveur.\nPartie abandonnée\n");
        close(fd);
        printf("Salut à la prochaine.\n");
        exit(0);
    }
    
    size_t taille=bufptr[1];

    rc= read_all( fd, bufptr+2 ,  taille);
    return rc;
}

/************************************************
    init_buf
 Fonction qui initialise un buffer a 0. 
************************************************/
void init_buf(unsigned char *buf,int taille){
    for (int i=0; i< taille; i++){
        buf[i]=0;
    } 

}
/************************************************
    send_discon
 Fonction qui envoie un TLV DISCON a une socket. 
************************************************/
int send_discon( int fd){
 unsigned char buf[TAILLE_BUF];
    int rc;

    init_buf(buf, TAILLE_BUF);

    code_discon(buf);

    decode_discon(buf);

    rc = write_tlv(fd,buf);
    return rc;
}
/*********************************************************
    code_discon
 Fonction qui code un message TLV DISCON dans un buffer. 
**********************************************************/
void code_discon( unsigned char *buf){
    buf[0]=7; //TYPE

    buf[1]=0; //LENGTH
}
/*********************************************************
    decode_discon
 Fonction qui affiche TLV DISCON. 
**********************************************************/
void decode_discon(unsigned char *buf){

    printf("\n << TLV DISCON >>\n");
    printf(" Type : %d \n", buf[0]); 
    printf(" Length : %d \n", buf[1] );

}
/*********************************************************
    send_concede
 Fonction qui envoie TLV CONCEDE a une socket. 
**********************************************************/
int send_concede( int fd){
 unsigned char buf[TAILLE_BUF];
    int rc;

    init_buf(buf, TAILLE_BUF);

    code_concede(buf);

    decode_concede(buf);

    rc = write_tlv(fd,buf);
    return rc;
}
/*********************************************************
    code_concede
 Fonction qui code TLV CONCEDE dans un buffer. 
**********************************************************/
void code_concede( unsigned char *buf){
    buf[0]=6; //TYPE

    buf[1]=0; //LENGTH
}
/*********************************************************
    decode_concede
 Fonction qui affiche un TLV CONCEDE. 
**********************************************************/
void decode_concede(unsigned char *buf){
/*
    printf("\n << TLV CONCEDE >>\n");
    printf(" Type : %d \n", buf[0]); 
    printf(" Length : %d \n", buf[1] );
*/
}
/*********************************************************
    send_moveack
 Fonction qui envoie un TLV MOVEACK a une socket. 
**********************************************************/
int send_moveack(int fd, unsigned char colonne, int accept_or_not){
unsigned char buf[TAILLE_BUF];
    int rc;
    init_buf(buf, TAILLE_BUF);

    code_moveack(buf, colonne,accept_or_not);

    decode_moveack(buf);

    rc = write_tlv(fd,buf);
    return rc;

}
/*********************************************************
    code_moveack
 Fonction qui code un TLV MOVEACK dans un buffer. 
**********************************************************/
void code_moveack( unsigned char *buf, unsigned char colonne, unsigned char ok){
    buf[0]=5; //TYPE

    buf[1]=2; //LENGTH

    buf[2]=colonne; // COLONNE

    buf[3]=ok; // OK

}

/*********************************************************
    decode_moveack
 Fonction qui affiche un TLV MOVEACK. 
**********************************************************/
void decode_moveack(unsigned char *buf){

    printf("\n << TLV MOVEACK >>\n");
    printf(" Type : %d \n", buf[0]); 
    printf(" Length : %d \n", buf[1] );
    printf(" COL : %d \n", buf[2] );
    printf(" OK : %d \n", buf[3] );

    unsigned coup=buf[3];

    if( coup == 1 ){
        printf("Le coup est accepté\n");
    }
    if( coup == 0 ){
        printf("Le coup n'est pas accepté, la colonne est déjà remplie\n");
    }
    printf("\n");
}

/*********************************************************
    send_move
 Fonction qui envoie un TLV MOVE a une socket. 
**********************************************************/
int send_move ( int fd ,unsigned char colonne){
    unsigned char buf[TAILLE_BUF];
    int rc;
    init_buf(buf, TAILLE_BUF);

    code_move(buf, colonne);

    //decode_move(buf);

    rc = write_tlv(fd,buf);
    return rc;

}
/*********************************************************
    code_move
 Fonction qui code un TLV MOVE dans un buffer. 
**********************************************************/
void code_move( unsigned char *buf, unsigned char colonne){
    buf[0]=4; //TYPE

    buf[1]=1; //LENGTH

    buf[2]=colonne; // COLONNE

}
/*********************************************************
    decode_move
 Fonction qui affiche un TLV MOVE. 
**********************************************************/
void decode_move(unsigned char *buf){
/*
    printf("\n << TLV MOVE >>\n");
    printf(" Type : %d \n", buf[0]); 
    printf(" Length : %d \n", buf[1] );
    printf(" COL : %d \n", buf[2] );
    */
    
}
/*********************************************************
    send_grid
 Fonction qui envoie un TLV GRID a une socket. 
**********************************************************/
int send_grid(int fd, unsigned char state1, unsigned char state2, unsigned char *gril){
    unsigned char buf[TAILLE_BUF];
    int rc;
    init_buf(buf, TAILLE_BUF);
    code_grid(buf , state1 ,state2,  gril) ;
            
    rc=write_tlv( fd,buf );
       
//decode_grille(buf);
        return rc;

} 
/*********************************************************
    code_grid
 Fonction qui code un TLV GRID. 
**********************************************************/
void code_grid( unsigned char *buf, unsigned char state1,unsigned char state2, unsigned char *grille){
    buf[0]=3; //TYPE

    buf[1]=44; //LENGTH

    buf[2]=state1; // PREMIERE PARTIE DE STATE

    buf[3]=state2; // DEUXIEME PARTIE DE STATE

    if( grille){
        for(int i=4, j=0; j< TAILLE_GRILLE ; i++,j++){
            buf[i]=grille[j];
    
        }
    }
     printf("\n");
}
/*************************************************************************
    code_grille_remplie
 Fonction qui code un TLV GRID 
 en assignant le state 1 égale 2 qui signifie que la grille est remplie. 
**************************************************************************/
void code_grille_remplie( unsigned char *buf ){
    code_grid(buf, 2,0, NULL);
}
/*************************************************************************
    decode_grille
 Fonction qui affiche un TLV GRID 
**************************************************************************/
void decode_grille(unsigned char *buf){
    /*
    printf("\n << TLV GRID >>\n");
    printf(" Type : %d \n", buf[0]); 
    printf(" Length : %d \n", buf[1] );
    */
    unsigned char state1=buf[2];
    //printf(" State1 : %d \n", buf[2] );
    unsigned char state2=buf[3];
    //printf(" State2 : %d \n", buf[3] );


    if( state1 == 0){
       /// printf("Joue ton prochain coup\n");
    }
    else if ( state1 == 1){
        printf("La partie a été gagnée par %s\n", (state2 == 0) ? "Rouge": "Jaune");
    }
    else{
        printf("La grille est totalement remplie sans vainqueur, partie nulle \n");
    }
    

    if( state1 != 2){
        affiche_grille(&buf[4]);
    }
    printf("\n");

}
/*************************************************************************
    code_pseudo
 Fonction qui code un TLV PSEUDO 
**************************************************************************/
void code_pseudo( unsigned char *buf, char *pseudo){
    buf[0]=1; //TYPE

    buf[1]=65; //LENGTH

    unsigned longue=strlen(pseudo);
    buf[2]=longue; // PLEN

    unsigned i,j;
    for( i=3, j=0; j< longue ; i++,j++){
        buf[i]=pseudo[j];
 
    }
    buf[i]='\0';
}
/*************************************************************************
    decode_pseudo
 Fonction qui affiche un TLV PSEUDO 
**************************************************************************/
void decode_pseudo( unsigned char *buf){

    printf("\n << TLV PSEUDO >>\n");
    printf(" Type : %d \n", buf[0]); 
    printf(" Length : %d \n", buf[1] );
    printf(" PLen : %d \n", buf[2] );

    printf(" PVal : ");
    int longue=buf[2];
    for(int i=3, j=0; j< longue ; i++,j++){
        printf("%c",buf[i] );
 
    }
    printf("\n");

}
/*************************************************************************
    send_pseudo
 Fonction qui envoie un TLV PSEUDO a une socket 
**************************************************************************/
int send_pseudo( int fd_sock, char *pseudo){
    
    unsigned char buf[TAILLE_BUF];
    int rc;

    init_buf(buf, TAILLE_BUF);

    code_pseudo(buf,pseudo);
//decode_pseudo(buf);

    rc = write_tlv(fd_sock,buf);

    return rc;
}
/*************************************************************************
    send_start
 Fonction qui envoie un TLV START a une socket 
**************************************************************************/
int send_start( int fd_sock, unsigned char couleur, unsigned char* buf1 , unsigned char *buf2){
    
    unsigned char buf[TAILLE_BUF];
    int rc;

    init_buf(buf, TAILLE_BUF);

    code_start(buf,couleur,buf1,buf2);

//decode_start(buf);

    rc = write_tlv(fd_sock,buf);
    return rc;
}
/*************************************************************************
    decode_tlv
 Fonction qui affiche un TLV reçu. 
 Le TLV est dans le buffer, selon le type du message on décode le TLV
**************************************************************************/
void decode_tlv (unsigned char *buf){
    unsigned char typ=buf[0];

    switch ( typ){
        case 1:
            decode_pseudo(buf);
            break;
        case 2:
            decode_start(buf);
            break;
        case 3:
            decode_grille(buf);
            break;
        case 4:
            decode_move(buf);
            break;
        case 5:
            decode_moveack(buf);
            break;
        case 6:
            decode_concede(buf);
            break;
        case 7:
            decode_discon(buf);
            break;
        default:
            printf("Pas encore traiter \n");
            break;
        
    }
}
/*************************************************************************
    code_start
 Fonction qui code un TLV START dans un buffer.
**************************************************************************/
void code_start( unsigned char *buf, unsigned char couleur, unsigned char *bufj1, unsigned char *bufj2){
    int i,j;
    buf[0]=2; // TYPE
    buf[1]=131; // LENGTH
    buf[2]=couleur; // PCOL
    unsigned char taille1=bufj1[0]; // PPLEN
    unsigned char taille2=bufj2[0]; // OPLEN

    buf[3]=taille1;

    for( i=4, j=1; j<= taille1 ; i++,j++){
        
        buf[i]=bufj1[j];
 
    }
    buf[i]='\0';
    i++;
    buf[68]=taille2;
    for(i=69, j=1; j<= taille2 ; i++,j++){
        buf[i]=bufj2[j];
 
    }
}
/*************************************************************************
    decode_start
 Fonction qui affiche un TLV START.
**************************************************************************/
void decode_start( unsigned char *buf){
   
    printf("\n << TLV START >>\n");
    printf(" Type : %d \n", buf[0]); 
    printf(" Length : %d \n", buf[1] );
    printf(" PCol : %d \n", buf[2] );
    printf(" PPLen : %d \n", buf[3] );

    printf(" PVal : ");
    int i,j;
    int longue=buf[3];
    for( i=4, j=0; j< longue ; i++,j++){
        printf("%c",buf[i] );
 
    }
    printf("\n OPLen : %d \n", buf[68] );
    longue=buf[68];
    printf(" OPVal : ");
    for( i=69, j=0; j< longue ; i++,j++){
        printf("%c",buf[i] );
 
    }
    printf("\n");

}
/****************************************************************************
    is_concede
 Fonction booléenne qui renvoie vraie si le buffer est du type TLV CONCEDE. 
*****************************************************************************/
_Bool is_concede(unsigned char *buf){
    return ((buf[0] == 6) ? true : false);
}
/****************************************************************************
    is_discon
 Fonction booléenne qui renvoie vraie si le buffer est du type TLV DISCON. 
*****************************************************************************/
_Bool is_discon(unsigned char *buf){
    return ((buf[0] == 7) ? true : false);
}
/************************************************************************************
    is_colonne_remplie
 Fonction booléenne qui renvoie vraie si la colonne donnée en paramètre est remplie. 
*************************************************************************************/
_Bool is_colonne_remplie(unsigned char colonne, unsigned char* grille){
    int casee= (colonne*TAILLE_LIGNE)+5;
    if (grille[casee] != JOUEUR_VIDE)
        return true;
    else    
        return false;
}
/************************************************************************************
    is_coup_invalide
 Fonction booléenne qui renvoie vraie si la colonne donnée en paramètre est remplie ou
 que la colonne n'appartient pas à la grille. 
*************************************************************************************/
_Bool is_coup_invalide(unsigned char colonne, unsigned char *grille){
    if(colonne > TAILLE_COLONNE || is_colonne_remplie(colonne,grille) )
        return true;
    else
        return false;
}
/************************************************************************************
    is_moveack_ok
 Fonction booléenne qui renvoie vraie si dans le TLV MOVEACK le coup joué a été accepté. 
*************************************************************************************/
_Bool is_moveack_ok(unsigned char *buf){
    return ((buf[3] == 1 ) ? true : false);
}
/************************************************************************************
    is_gamewin
 Fonction booléenne qui renvoie vraie si dans le TLV GRID le state 1 égale 1
 Cela signifie qu'une partie est gagnée. 
*************************************************************************************/
_Bool is_gamewin(unsigned char *buf){
    return ((buf[2] == 1 && buf[0]== 3) ? true : false);
}
/************************************************************************************
    is_gameover
 Fonction booléenne qui renvoie vraie si dans le TLV GRID le state 1 égale 2
 Cela signifie qu'une grille est complètement remplie
  sans qu’aucun joueur ait gagné et la partie est nulle.
*************************************************************************************/
_Bool is_gameover(unsigned char *buf){
    return ((buf[2] == 2 && buf[0]== 3) ? true : false);
}
/************************************************************************************
    get_winner
 Fonction qui renvoie la couleur du joueur qui a gagnée la partie en lisant le TLV GRID.
*************************************************************************************/
unsigned char get_winner(unsigned char *buf){
    return buf[3];
}
/************************************************************************************
    get_next_joueur
 Fonction qui renvoie la couleur du joueur qui est le prochain a joué en lisant le TLV GRID.
*************************************************************************************/
unsigned char get_next_joueur( unsigned char *buf){
    return buf[3];
}
/************************************************************************************
    get_colonne
 Fonction qui renvoie la colonne jouée en lisant le TLV GRID.
*************************************************************************************/
unsigned char get_colonne( unsigned char *buf){
    return buf[2];
}
/*
int main(){

    unsigned char buf[TAILLE_BUF];
    init_buf(buf,TAILLE_BUF);
    code_pseudo(buf,"toto");
    decode_tlv(buf);

    unsigned char buf2[TAILLE_BUF];
    init_buf(buf2,TAILLE_BUF);
    code_pseudo(buf2,"titii");
    decode_tlv(buf2);

    unsigned char bufn[TAILLE_BUF];
    //code_start( bufn, 0, &buf[2] , &buf2[2] );
    //decode_start(bufn);

    unsigned char gril[TAILLE_GRILLE];
    init_grille(gril);
    ajout_coup(0,1,gril);
    ajout_coup(3,0,gril);

printf("\nNotre test \n");
    code_grid(bufn, 0,1, gril);
    //decode_grille(bufn);
    decode_tlv(bufn);

}*/