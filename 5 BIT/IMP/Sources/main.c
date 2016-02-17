/*
 * IMP Projekt - Simulacia CodeWarrior Svetelne Noviny
 * Autor: Filip Gulan
 * mai: xgulan00@stud.fit.vutbr.cz
 * original
 */
 
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#define ZACADRESA 612 //zaciatocna adresa od ktorej zacinaju data, kam ukladam stlpce lediek
#define INIT 0
#define HORIZONT 1
#define VERTIKAL 2

unsigned char *login[64];
unsigned char *bar;
unsigned char *init;
unsigned char *horizont;
unsigned char *vertikal;
int j;
int mod;

/*
 * Funkcia ktora premennym priradi adresy 
 */
void nastavAdresu() {
  //Adresy pre stlpce lediek
  int i;
  for(i = 0; i < 64; i++) {
    login[i] = (char *) ZACADRESA + i;  
  }
  //Adresy pre vstupne tlacitka  
  init = (char *) 608;
  horizont = (char *) 609; 
  vertikal = (char *) 610;
  bar = (char *) 611;
}

/*
 * Inicializuje pamat 
 * Jednomu pismenu prislucha jendo pamatove miesto 1 led = 1 bit
 * Ledka najvyssie je najmenej vyznamny bit
 */
void inicializuj() {
  //X
  *login[0] = 0;
  *login[1] = 66;
  *login[2] = 102;
  *login[3] = 24;
  *login[4] = 24;
  *login[5] = 102;
  *login[6] = 66;
  *login[7] = 0;
  //G
  *login[8] = 0; 
  *login[9] = 60; 
  *login[10] = 70; 
  *login[11] = 66; 
  *login[12] = 82; 
  *login[13] = 82; 
  *login[14] = 50; 
  *login[15] = 0;
  //U
  *login[16] = 0; 
  *login[17] = 126; 
  *login[18] = 96; 
  *login[19] = 64; 
  *login[20] = 64; 
  *login[21] = 96; 
  *login[22] = 126; 
  *login[23] = 0;
  //L
  *login[24] = 0; 
  *login[25] = 126; 
  *login[26] = 126; 
  *login[27] = 64; 
  *login[28] = 64; 
  *login[29] = 64; 
  *login[30] = 96; 
  *login[31] = 0;
  //A
  *login[32] = 0; 
  *login[33] = 96; 
  *login[34] = 56; 
  *login[35] = 38; 
  *login[36] = 38; 
  *login[37] = 56; 
  *login[38] = 96; 
  *login[39] = 0;
  //N
  *login[40] = 0; 
  *login[41] = 126; 
  *login[42] = 12; 
  *login[43] = 24; 
  *login[44] = 48; 
  *login[45] = 96; 
  *login[46] = 126; 
  *login[47] = 0;
  //0
  *login[48] = 0; 
  *login[49] = 60; 
  *login[50] = 102; 
  *login[51] = 66; 
  *login[52] = 66; 
  *login[53] = 102; 
  *login[54] = 60; 
  *login[55] = 0;
  //0
  *login[56] = 0; 
  *login[57] = 60; 
  *login[58] = 102; 
  *login[59] = 66; 
  *login[60] = 66; 
  *login[61] = 102; 
  *login[62] = 60; 
  *login[63] = 0;
     
}

/*
 * Funkcia, ktpora zabezpecuje rotaciu loginu dolava
 */
void rotaciaDolava() {
  unsigned char pomocny = *login[0];
  int i;
  for(i = 0; i < 63; i++) {
    *login[i] = *login[i + 1];  
  }
  *login[63] = pomocny;
}

/*
 * Funkcia zabezpecuje rotaciu loginu dole
 */
void rotaciaDole() {
  int i;
  for(i = 0; i < 63; i++) {
    if(*login[i] > 127) { //bitovy rotacia je tu urobena ako bitovy posun, ak je najvyssi bit 1, tak iba po posune pricitam 1ku
     *login[i] = *login[i] << 1;
     *login[i] = *login[i] + 1;
    } else {
      *login[i] = *login[i] << 1;
    }
  }
}

/*
 * Funkcia, ktora zistuje aky rezim uzivatel zadal, tj v danom amatovom mieste je v dany moment nastavena 1ka
 */
void zistiMod() {
   if(*horizont == 1) {
     mod = HORIZONT;
    } else if(*vertikal == 1) {
     mod = VERTIKAL;
    } else if(*init == 1) {
      mod = INIT;
    }
}

/*
 * Funkcia, ktora meni spravanie podla priznaku nastavenom vo funkcii rezim
 */
void nastavMod() {
 if(mod == HORIZONT) {
     rotaciaDolava(); 
    } else if(mod == VERTIKAL) {
     rotaciaDole(); 
    } else if(mod == INIT) {
     inicializuj();
    } 
}

/*
 * Funkcia na spomalenie, teda riesi akou rychlostou sa bude login posuvat
 */
void spomalenie() {
 for(j = 0; j < (1400 - (*bar * 4)); j++) { //for na nastavovanie rychlosti rotacie loginu
      zistiMod();
    } 
}

/*
 * Hlavna nekonecna funkcia
 */
void main(void) {

  EnableInterrupts; /* enable interrupts */
  
  
  /* include your code here */
  nastavAdresu();
  inicializuj();
  
  *init = 0;
  *horizont = 0;
  *vertikal = 0;
  *bar = 100;
  

  for(;;) {
    
    spomalenie();
    zistiMod();
    nastavMod();
    
    __RESET_WATCHDOG(); /* feeds the dog */
    
  } /* loop forever */
  /* please make sure that you never leave main */
}