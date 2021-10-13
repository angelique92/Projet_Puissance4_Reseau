#define TAILLE_BUF  256 

void init_buf(unsigned char *buf,int taille);

void code_pseudo( unsigned char *buf, char *pseudo);
void decode_pseudo( unsigned char *buf);
int send_pseudo( int fd_sock, char * pseudo);

void code_start( unsigned char *buf, unsigned char joueur, unsigned char *bufj1, unsigned char *bufj2);
void decode_start( unsigned char *buf);
int send_start( int fd_sock, unsigned char couleur, unsigned char* buf1 , unsigned char *buf2);


void code_grid( unsigned char *buf, unsigned char state1,unsigned char state2, unsigned char *grille);
void decode_grille(unsigned char *buf);
int send_grid(int fd, unsigned char state1, unsigned char state2, unsigned char *gril);

void code_move( unsigned char *buf, unsigned char colonne);
void decode_move(unsigned char *buf);
int send_move ( int fd ,unsigned char colonne);

void code_moveack( unsigned char *buf, unsigned char colonne, unsigned char ok);
void decode_moveack(unsigned char *buf);
int send_moveack(int fd, unsigned char colonne, int accept_or_not);

void code_concede( unsigned char *buf);
void decode_concede( unsigned char *buf);
int send_concede( int fd);

void code_discon( unsigned char *buf);
void decode_discon( unsigned char *buf);
int send_discon( int fd);

ssize_t read_tlv(int fd, void *buf);
ssize_t write_tlv(int fd, void *buf);
void decode_tlv (unsigned char *buf);

_Bool is_concede(unsigned char *buf);
_Bool is_discon(unsigned char *buf);
_Bool is_coup_invalide(unsigned char colonne, unsigned char *grille);
_Bool is_colonne_remplie(unsigned char colonne, unsigned char* grille);
_Bool is_moveack_ok(unsigned char *buf);
_Bool is_gameover(unsigned char *buf);
_Bool is_gamewin(unsigned char *buf);
unsigned char get_next_joueur( unsigned char *buf);
unsigned char get_winner(unsigned char *buf);
unsigned char get_colonne( unsigned char *buf);