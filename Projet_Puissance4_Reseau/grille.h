#define TAILLE_GRILLE 42
#define TAILLE_COLONNE 7
#define TAILLE_LIGNE 6

#define JOUEUR_ROUGE 0
#define JOUEUR_JAUNE 1
#define JOUEUR_VIDE 2


int grille[TAILLE_GRILLE];

void init_grille (unsigned char *grille);
void affiche_grille(unsigned char *grille);
void ajout_coup (unsigned char colonne,unsigned char joueur, unsigned char *grille);
_Bool is_win_ligne( unsigned char * grille,unsigned char colonne, unsigned char couleur);
_Bool coup_gagnee(unsigned char * grille,unsigned char colonne, unsigned char couleur);
_Bool is_win_diagonal( unsigned char * grille,int colonne, unsigned char couleur);
_Bool is_win_colonne( unsigned char * grille,int colonne, unsigned char couleur);
_Bool grille_remplie(unsigned char *gril);