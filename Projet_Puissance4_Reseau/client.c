#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include "utils.h"
#include "grille.h"
#include "joueur.h"
#include "tlv.h"
/********************************************************************************************
    saisie_pseudo
 Fonction qui permet au client de saisir son pseudo 
*********************************************************************************************/
void saisie_pseudo (char *pseudo, int taille ){
    
    char *positionEntree;

    printf("Donnes ton pseudo : \n");
    fgets(pseudo, taille, stdin); // read from stdin
    positionEntree = strchr(pseudo, '\n'); // On recherche l'"Entrée"
    *positionEntree = '\0'; // On remplace ce caractère par \0
    printf("\nBonjour %s,\nBienvenue au jeu Puissance 4 en Réseau \n", pseudo);
    
}
/********************************************************************************************
    saisie_coup_a_jouer
 Fonction qui permet au client de saisir la colonne ou il souhaite joué
 Si le client fais -1 alors il souhaite abandonner la partie 
*********************************************************************************************/
int saisie_coup_a_jouer( ){

    int colonne;
    printf("Donnes la colonne où tu veux jouer : \n");
    printf("Si tu veux abandonner tape -1 \n");
    scanf("%d", &colonne);
    // POUR DEBUG PLUS RAPIDEMENT DU MOVEACK
    if(colonne != -1){
        printf("Ta colonne jouée est %d \n", colonne);
    }else{
        printf("Partie abandonnée\n");
    }
    
    return colonne;
}
/********************************************************************************************
    stop_client_sur_alert
 Fonction qui arrete le client des qu'il reçoit un signal bloquant.
*********************************************************************************************/
void stop_client_sur_alert( int code ) {
    printf( ">>>>>>><<<< stop_client_sur_alert received [%d]\n", code );
    //send_discon(fd_sock);

    exit(0);
}


unsigned char buf[TAILLE_BUF];

int
main(int argc, char **argv)
{
    char pseudo[TAILLE_PSEUDO];
    
    struct joueur j;
    int colonne;
    int rc;

    if (argc < 1 + 2) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);


    int fd_sock = socket(AF_INET6, SOCK_STREAM, 0);
    HANDLE_ERROR(fd_sock, "création socket");


    signal( SIGKILL, &stop_client_sur_alert);
    signal( SIGTERM, &stop_client_sur_alert);
    signal( SIGSTOP, &stop_client_sur_alert);
    signal( SIGQUIT, &stop_client_sur_alert);
    signal( SIGINT, &stop_client_sur_alert);

    struct sockaddr_in6 sin6 = {
        .sin6_family = AF_INET6,
        .sin6_port = htons(port),
    };
    rc = inet_pton(AF_INET6, ip, &sin6.sin6_addr);

//printf("Je suis le client %d \n", getpid());
    
    // SAISIE DU PSEUDO 
    saisie_pseudo(pseudo, TAILLE_PSEUDO);
    set_pseudo(&j, pseudo);
//printf("\t le pseudo du joueur est %s\n", j.pseudo);

    rc = connect(fd_sock,(struct sockaddr *) &sin6, sizeof(sin6));
    HANDLE_ERROR(rc, "connect");


//printf("Connecté au serveur.\n");
    fflush(stdout);

//step0:
    // ENVOIE LE PSEUDO 
    rc=send_pseudo(fd_sock,pseudo);
    HANDLE_ERROR(rc, "write_all");
   //fflush(stdout);
//printf("LE CLIENT A ENVOYER LE TLV PSEUDO \n");

//step1 :
    // RECOIT LE START
   read_tlv(fd_sock,buf);
   j.couleur=buf[2];
   printf("La couleur qui t'a été assigné est : %s\n", (j.couleur == 0) ? "Rouge" : "Jaune" );

//printf("LE CLIENT A RECU LE TLV DE START \n");
//decode_start(buf);

    // RECOIT LA GRILLE
       // unsigned char gril[TAILLE_GRILLE];
//printf("LE CLIENT ROUGE A RECU LE TLV DE GRID \n");
//printf("LE CLIENT 1 ROUGE COMMENCE \n");
    read_tlv(fd_sock,buf);
    if( is_concede(buf)){
        printf("LE CLIENT A RECU DU SERVEUR: QUE L ADVERSAIRE A ABANDONNER LA PARTIE \n");
        goto step7;
    }
        
    decode_tlv(buf);
      
    if( j.couleur == buf[3]){
        goto step2;
    }else{
        goto step5;
    }

step2:
    // SAISIE DU COUP A JOUER ( COLONNE)
    colonne=saisie_coup_a_jouer();
    
    if( colonne != -1 ){
        //printf("LE CLIENT ENVOIE LE TLV MOVE  AU SERVEUR \n");
        send_move(fd_sock,( unsigned char )colonne);
        //printf("LE CLIENT RECOIT LE TLV MOVEACK  DU SERVEUR \n");
        read_tlv(fd_sock,buf);
        
//step3:
        if(!is_moveack_ok(buf)){
            printf("Le coup joué n'a pas été accepté.\nVeuillez recommencer.\n");
            //printf("LE CLIENT A RECU LE TLV DE GRID \n");
            read_tlv(fd_sock,buf);
            decode_tlv(buf);
            goto step2;
        }else{
//step4:
            printf("Le coup joué a été accepté,\nattends le coup de l'adversaire\n");
            //printf("LE CLIENT A RECU LE TLV DE GRID \n");
            read_tlv(fd_sock,buf);
            decode_tlv(buf);
//step4bis: 
            if( is_gameover(buf) ){
                printf("JEU FINIS SANS VAINQUEUR\n");
                goto step7;
            }else if( is_gamewin(buf)){
                printf("JEU FINIS AVEC VAINQUEUR\n");
                printf("FELICITATION TU AS GAGNE ! \n");
                printf("Vainqueur : %s\n", (get_winner(buf) == 0) ? "Rouge": "Jaune");
                goto step7;
            }
            else{
                printf("Le jeu n'est pas terminé en attente du coup de l'adversaire.... \n");
                //printf("\t ------step 4 ------- le client doit jouer son coup on attend une grid\n");
                rc=read_tlv(fd_sock,buf); //move ou concede
                HANDLE_DISCON(rc,fd_sock);
                //decode_tlv(buf);
                if( is_discon(buf)){
                    printf("LA CONNEXION AVEC LE CLIENT A ETE PERDU \n");
                    goto step7;
                }
                if( is_concede(buf)){
                    printf("LE CLIENT A RECU DU SERVEUR: QUE L ADVERSAIRE A ABANDONNER LA PARTIE \n");
                    goto step7;
                }
                if( is_gamewin(buf)){
                    decode_tlv(buf);
                    printf("JEU FINI AVEC VAINQUEUR\n");
                    printf("DESOLE TU AS PERDU ! \n");
                    printf("Vainqueur : %s\n", (get_winner(buf) == 0) ? "Rouge": "Jaune");
                    
                    goto step7;
                }
                if( is_gameover(buf) ){
                    printf("JEU FINIS SANS VAINQUEUR\n");
                    goto step7;
                }
               
                
                //printf("Le joueur est  %s : %d\n", j.pseudo,j.couleur);
                read_tlv(fd_sock,buf);
                decode_tlv(buf);
                
                //printf("Dans GRID : state1 : %d state 2: %d\n", buf[2],buf[3]);
                goto step2;
                
            }

        }
    }
    else {
        printf("LE CLIENT ENVOIE LE TLV CONCEDE  AU SERVEUR \n");
        send_concede(fd_sock);
        rc = close(fd_sock);
        HANDLE_ERROR(rc, "close");
        printf("Salut à la prochaine.\n");
        exit(0);
    }

   //affiche_grille(gril[3]);

step5:
    read_tlv(fd_sock,buf);
    if( is_concede(buf)){
        printf("LE CLIENT A RECU DU SERVEUR: QUE L ADVERSAIRE A ABANDONNER LA PARTIE \n");

step7:      
        rc = close(fd_sock);
        HANDLE_ERROR(rc, "close");
        printf("Salut à la prochaine.\n");
        exit(0);
    }else{
//step6:
        //printf("LE CLIENT A RECU UN TLV MOVE indiquant un coup valide de l’adversaire \n");
        decode_tlv(buf);
        //printf("\t ------ step 6------- le client doit jouer son coup \n");        
        //read_tlv(fd_sock,buf);
        //printf("ON ATTEND UN TLV GRID \n");
        //decode_tlv(buf);
        //printf("Le joueur est  %s : %d\n", j.pseudo,j.couleur);
        //printf("Dans GRID : state1 : %d state 2: %d\n", buf[2],buf[3]);
        goto step2;
    }

    return 0;
}