#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
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
    stop_Aclient_sur_alert
 Fonction qui permet au serveur de débloquer le write d'apres le signal SIGPIPE
 un client s'est arreté.
*********************************************************************************************/
void stop_Aclient_sur_alert( int code ) {
    printf( ">>>>>>><<<< stop_Aclient_sur_alert received [%d]\n", code );
}

int
main(int argc, char **argv)
{
    unsigned char buf[TAILLE_BUF];
    unsigned char buf2[TAILLE_BUF];
    
signal(SIGPIPE, stop_Aclient_sur_alert);
    if (argc < 1 + 1) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int port = atoi(argv[1]);

    int rc;

    int fd_sock_pass= socket(PF_INET6, SOCK_STREAM, 0);
    HANDLE_ERROR(fd_sock_pass, "création socket passive");


    struct sockaddr_in6 sin6 = {
        .sin6_family = PF_INET6,
        .sin6_port = htons(port),
    };

    printf("Je suis le server %d \n", getpid());


    int opt_reuseaddr = 1;
    rc = setsockopt(fd_sock_pass, SOL_SOCKET, SO_REUSEADDR,
                    &opt_reuseaddr, sizeof(opt_reuseaddr));
    if (rc < 0) {
        HANDLE_ERROR(rc, "setsockopt(SO_REUSEADDR)");
        return -1;
    }

    rc = bind(fd_sock_pass,
              (struct sockaddr *) &sin6, sizeof(sin6));
    HANDLE_ERROR(rc, "bind");

    rc = listen(fd_sock_pass, 1024);
    HANDLE_ERROR(rc, "listen");

    printf("Listening on port %d\n", port);

   // uint32_t client_num = 0;
    

    while (1) {
        init_buf(buf, TAILLE_BUF); 
        init_buf(buf2, TAILLE_BUF);    
        struct joueur paire_joueur[2];
        
        int fd_sock_conn =
            accept(fd_sock_pass, NULL, NULL);
        HANDLE_ERROR(fd_sock_conn, "accept");

        paire_joueur[0].fd=fd_sock_conn;

        // LIRE PSEUDO 1 ENVOYER PAR CLIENT 
        rc=read_tlv(fd_sock_conn,buf);
        
        printf("\nLE SERVEUR A RECU LE TLV PSEUDO 1 : \n");
        decode_tlv(buf);
        
        char * bufptr=(char *)buf;
        set_pseudo(&paire_joueur[0], &bufptr[3]);  


int fd_sock_conn2 =
            accept(fd_sock_pass, NULL, NULL);
        HANDLE_ERROR(fd_sock_conn2, "accept");

        paire_joueur[1].fd=fd_sock_conn2;
        
        // LIS PSEUDO 2 ENVOYER PAR CLIENT 
        rc=read_tlv(fd_sock_conn2,buf2);
        
        printf("LE SERVEUR A RECU LE TLV PSEUDO 2: \n");
        decode_tlv(buf2);

        bufptr=(char *)buf2;
        set_pseudo(&paire_joueur[1], &bufptr[3]);

        int pid;
        pid=fork();

        if( pid == -1){
                perror("fork");
                exit(1);
            }
        if( pid == 0){ // FILS
        signal(EINTR,&stop_Aclient_sur_alert );
        printf("----------Je suis le server FILS %d \n", getpid()); 
            printf("On va faire jouer les deux joueurs\n");

        // TIRE AU HASARD LA COULEUR DES DEUX JOUEURS CONNECTES
       int n = rand() % 2;
        
        unsigned char couleur1=n;
        unsigned char couleur2=(n==0)? 1 :0;
        paire_joueur[0].couleur=couleur1;
        paire_joueur[1].couleur=couleur2;
printf("Le premier joueur %s est %d \n",paire_joueur[0].pseudo, couleur1);
printf("Le deuxieme joueur %s est %d \n",paire_joueur[1].pseudo,couleur2);
        fflush(stdout);
        printf("\nLE SERVEUR ENVOIE LES TLV STARTS AUX CLIENTS \n");
        send_start( paire_joueur[0].fd,  paire_joueur[0].couleur, &buf[2], &buf2[2]); // JOUEUR1
        fflush(stdout);
        send_start( paire_joueur[1].fd,  paire_joueur[1].couleur,  &buf2[2], &buf[2]); // JOUEUR2
        fflush(stdout);

        printf("LE SERVEUR ENVOIE LE TLV GRID : AU CLIENT ROUGE\n");
        unsigned char gril[TAILLE_GRILLE];
        init_grille(gril); // GRILLE INITIALISER ( GRILLE VIDE )
        //affiche_grille(gril);
        struct joueur *cur_joueur, *other_joueur;
        if( paire_joueur[0].couleur == 0){
            cur_joueur=&paire_joueur[0];
            other_joueur=&paire_joueur[1];
        }else{
            cur_joueur=&paire_joueur[1];
            other_joueur=&paire_joueur[0];
        }
        rc=send_grid( cur_joueur->fd, 0,cur_joueur->couleur,gril);
        HANDLE_DISCON(rc,other_joueur->fd);
        //while()
stepjeu:      
        printf("LE SERVEUR RECOIT LE TLV MOVE OU CONCEDE DU CLIENT ROUGE \n");
        rc=read_tlv(cur_joueur->fd ,buf);
        HANDLE_DISCON(rc, other_joueur->fd);
        decode_tlv(buf);


        if( is_concede(buf)){
            printf("LE SERVEUR RECOIT LE TLV CONCEDE DU CLIENT ROUGE \n");
            printf("LE SERVEUR ENVOIE AU CLIENT : QUE L ADVERSAIRE A ABANDONNER LA PARTIE \n");
            rc=send_concede(other_joueur->fd);
            HANDLE_DISCON(rc, cur_joueur->fd);
            

            // Ferme la socket de connection d'un joueur
            rc = close(fd_sock_conn2);
            HANDLE_ERROR(rc, "close connection");
            printf("Connection fermée2.\n");
            // Ferme la seconde socket de connection d'un joueur
            rc = close(fd_sock_conn);
            HANDLE_ERROR(rc, "close connection");
            printf("Connection fermée1.\n");
            
            printf("Je suis le serverFILS %d \n", getpid());
            exit(0);

        }else if (is_coup_invalide(buf[2],gril)){
            printf("COUP INVALIDE\n");
            printf("LE SERVEUR ENVOIE UN TLV MOVEACK AU CLIENT \n");
            rc=send_moveack(cur_joueur->fd, buf[2], 0);
            HANDLE_DISCON(rc, other_joueur->fd);
            printf("LE SERVEUR ENVOIE UN TLV GRID AU CLIENT \n");
            rc=send_grid(cur_joueur->fd,0,cur_joueur->couleur ,gril );
            HANDLE_DISCON(rc, other_joueur->fd);

            goto stepjeu;
        }
        else{
            printf("LA PARTIE CONTINUE \n");
            printf("COUP VALIDE\n");
            printf("LE SERVEUR ENVOIE UN TLV MOVEACK AU CLIENT \n");
            unsigned char colonne=get_colonne(buf);
            rc=send_moveack(cur_joueur->fd, buf[2], 1);
            HANDLE_DISCON(rc, other_joueur->fd);
            //GRID MAJ ENVOYER CURJOUEUR
            printf("Grille avant ajout coup \n");
             affiche_grille(gril);
            ajout_coup (colonne,cur_joueur->couleur, gril);
            if( coup_gagnee(gril,colonne,cur_joueur->couleur)) {
                printf(" ON A UN VAINQUEUR\n");
                printf("LE SERVEUR ENVOIE AU CLIENT GAGNANT LE TLV GRID \n");
                rc=send_grid( cur_joueur->fd, 1, cur_joueur->couleur, gril);
                HANDLE_DISCON(rc, other_joueur->fd);
                printf("LE SERVEUR ENVOIE AU CLIENT PERDANT LE TLV GRID \n");
                rc=send_grid( other_joueur->fd, 1, cur_joueur->couleur, gril);
                HANDLE_DISCON(rc, cur_joueur->fd);

                printf("LE SERVEUR FERME LES CONNEXION\n");
                rc = close(fd_sock_conn);
                HANDLE_ERROR(rc, "close connection");
                rc = close(fd_sock_conn2);
                HANDLE_ERROR(rc, "close connection");
                rc = close(fd_sock_pass);
                HANDLE_ERROR(rc, "fermeture de la socket passive");

                return 0;

            }
            if( grille_remplie(gril)) {
                printf(" ON A PAS DE VAINQUEUR\n");
                printf("LE SERVEUR ENVOIE AUX CLIENT LE TLV GRID \n");
                rc=send_grid( cur_joueur->fd, 2,0, gril);
               HANDLE_DISCON(rc, other_joueur->fd);
                rc=send_grid( other_joueur->fd, 2, 0, gril);
                HANDLE_DISCON(rc, cur_joueur->fd);

                printf("LE SERVEUR FERME LES CONNEXION\n");
                rc = close(fd_sock_conn);
                HANDLE_ERROR(rc, "close connection");
                rc = close(fd_sock_conn2);
                HANDLE_ERROR(rc, "close connection");
                rc = close(fd_sock_pass);
                HANDLE_ERROR(rc, "fermeture de la socket passive");

                return 0;

            }
            
            printf("Grille après ajout coup \n");
            affiche_grille(gril);

            printf("LE SERVEUR ENVOIE UN TLV GRID MAJ AU CLIENT \n");
            rc=send_grid(cur_joueur->fd,0,other_joueur->couleur ,gril );
            HANDLE_DISCON(rc, other_joueur->fd);

            rc=send_move(other_joueur->fd,colonne );
            HANDLE_DISCON(rc, cur_joueur->fd);

            rc=send_grid(other_joueur->fd,0,other_joueur->couleur ,gril );
            HANDLE_DISCON(rc, cur_joueur->fd);

            struct joueur *tmp;
            tmp=cur_joueur;
            cur_joueur=other_joueur;
            other_joueur=tmp;

            goto stepjeu;
            
        }

        }else{ //PERE 
            printf("----------Je suis le serverPERE %d -- je viens de creer mon fils %d \n", getpid(), pid);
            printf("je dois relire deux autre joueur\n");
            int kidpid, status;

            if((kidpid = waitpid(-1, &status, WNOHANG)) > 0)
            {
                 printf("***************Je suis le serverPERE %d --  mon fils %d terminated ( will not be a zombie) \n", getpid(), pid);
            }
        }

    }

printf("----------Je suis le server %d \n", getpid()); 
//sleep(60);
/*
rc = close(fd_sock_conn);
            HANDLE_ERROR(rc, "close connection");
            rc = close(fd_sock_conn2);
            HANDLE_ERROR(rc, "close connection");*/
    rc = close(fd_sock_pass);
    HANDLE_ERROR(rc, "fermeture de la socket passive");

    return 0;
}