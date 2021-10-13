
#define TAILLE_PSEUDO 65

struct joueur{
    char *pseudo;
    unsigned char couleur;
    int fd;
};

unsigned char get_couleur(struct joueur *j);
void set_couleur(struct joueur *j, unsigned char couleur);
char * get_pseudo(struct joueur *j);
void free_joueur(struct joueur *j);
void set_pseudo(struct joueur *j, char *pseudo);
void set_fd( struct joueur *j, int f);
int get_fd( struct joueur *j);