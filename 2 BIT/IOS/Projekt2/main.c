/*
 * Soubor:  main.c
 * Datum:   4.5.2014
 * Autor:   1BIA - Filip Gulan, xgulan00@stud.fit.vutbr.cz
 * Projekt: 1BIT - IOS - Synchronizacia procesov - rivercrossing problem
 * Popis viz:
        http://www.fit.vutbr.cz/study/courses/IOS/public/Lab/projekty/projekt2/projekt2.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//na vytvorenie childov a cakanie
#include <unistd.h>
#include <sys/wait.h>

//Zdielana pamat a semafory
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>

#define PODMIENKA_MOLA ((*pocetHacker == 4 && *pocetSerf == 0) || (*pocetHacker == 0 && *pocetSerf == 4) || (*pocetHacker == 2 && *pocetSerf == 2) || (*pocetHacker == 2 && *pocetSerf == 3) || (*pocetHacker == 3 && *pocetSerf == 2) || (*pocetHacker == 4 && *pocetSerf == 1) || (*pocetHacker == 1 && *pocetSerf == 4))
#define CAPTAIN (1) //oznacenie procesu ci je kapitan
#define MEMBER (0) //oznacenie procesu ci je member
#define ERROR (-1) //vracia -1 pri chybe napr pri nepreveeneom delokacii semaforu alebo zdielanej premennej
#define POCIATOK (0) //pociatocna hodnota niektoryuch veci, alebo vynulovanie
#define DIETA (0) //detekuje dieta, child process
#define POSUNUTIE (1) //posunutie v poli
#define OK (0) //vracia 0 ak sa ukoncilo spravne
#define CHYBA (1) //vracia chybu pri argumentoch alebo incyh nesystemovych chybach
#define CHYBA2 (2) //vracia 2 pru systemovych chybach
#define TOMS (1000) //konvertuje na milisekundy
#define HORNAHRANICA (5001) //horna hranica argumentov 
#define DOLNAHRANICA (0) //dolna hranica argumentov
#define ZDVOJNASOB (2) //pozuiva sa na pockanie vsetkych procesov, lebo zadavame argument 2x mensi musime cakat 2x viac procesov
#define ZATVOR (1) //pozuiva sa na zavretie prichodu rpocesov na molo cakanie
#define OTVOR (0) //pouziva sa na otvorenie prichodu procesov na molo cakanie

//**********Globalne deklaracie***************
//Subor
FILE *vypis=NULL; //subor, do ktoreho budem zapisovat akcie procesov
//Semafory
sem_t *sem_vypis; //semafor, na uvolnovanie a zamykanie vypisu a inkrementacie zdielanej rpemennej
sem_t *molo_wait; //semafor, na korigovanie poctu prichadzajucich na molo, aby sa dostali iba tolki kolki treba
sem_t *hacker_boarding_wait; //semafor, na znemoznenie hackerom prichod na molo, pri selekcii sa pouziva aby som si vedel vybrat koho pustiti
sem_t *serf_boarding_wait; //semafor, na znemoznenie serfom prichod na molo, pri selekcii sa pouziva aby som si vedel vybrat koho pustiti
sem_t *memberc_wait; //semafor, na korigovanie aby sas spustili naraz 4 procesy do vypisovania captain member
sem_t *captain_wait; //semafor, aby sa spsutili vsetci spou po spanku kapitana
sem_t *moloship_wait; //na zbranenie procesom dostat sa na molo pocas plavby lode, spusta sa az po landingu
sem_t *plavba_wait; //aby sa psustili spolu na uspanie kapitana
sem_t *landing_wait; //aby sa spustili vsetci naraz do finished
//Zdielane premenne
int *cisloAkcie; //cislo akcie pre vypis
int *pocetSerf; //pocet serfov na mole
int *pocetHacker; //pocet hackerov na mole
int *pocetMolo; //celkovy pocet osob na mole
int *pocetBoarding; //aby ich spustilo spolocne do vypisovania captain member
int *moloShip; //premenna, vdaka ktorej zablokujeme rovnomenny semafor
int *pocet_CM; //pocet clenov, na vzajomne sa cakanie pocas pisania  captain member
int *pocet_landing; //aby sa dalsi nalodili, az ked su prvy vylodeni
int *pocet_finished_wait; //premenna aby sa mohli pustit vsetky procesy na raz, vyuzita v finished
//Miesto pre zdielane premenne, su v oradi ako tu vyssie zdielane premenne, a poradie odpoveda poradiu premenej, ku ktorej miesto nalezi
int shm_akcia; 
int shm_serf;
int shm_hacker;
int shm_pocetmolo;
int shm_pocetboarding;
int shm_moloship;
int shm_landing_CM;
int shm_landing_pocet;
int shm_pocet_finished_wait;

/*
Funkcia na uzavretie semaforo, ked sa s nimi uz nepracuje
*/
void close_semafory()
{
    sem_close(sem_vypis);
    sem_close(molo_wait);
    sem_close(hacker_boarding_wait);
    sem_close(serf_boarding_wait);
    sem_close(memberc_wait);
    sem_close(captain_wait);
    sem_close(moloship_wait);
    sem_close(plavba_wait);
    sem_close(landing_wait);

    //Odalokovanie miesta semaforov
    sem_unlink("/xgulan00start");
    sem_unlink("/xgulan00molowait");
    sem_unlink("/xgulan00hackerboardingwait");
    sem_unlink("/xgulan00serfboardingwait");
    sem_unlink("/xgulan00membercwait");
    sem_unlink("/xgulan00captainwait");
    sem_unlink("/xgulan00volnowait");
    sem_unlink("/xgulan00plavbawait");
    sem_unlink("/xgulan00landingwait");
}

/*
Funkcia na odalokovanie, odmazanie, uzavretie vsetkych zdrojov, ktore sme pouzivali
*/
void clean_dealokacia()
{
    close_semafory();
    fclose(vypis);

    //Odmapovanie pamate 1ukazatel co vratil mmap 2velkost pamate
    munmap(cisloAkcie, sizeof(int));
    munmap(pocetSerf, sizeof(int));
    munmap(pocetHacker, sizeof(int));
    munmap(pocetMolo, sizeof(int));
    munmap(pocetBoarding, sizeof(int));
    munmap(moloShip, sizeof(int));
    munmap(pocet_CM, sizeof(int));
    munmap(pocet_landing, sizeof(int));
    munmap(pocet_finished_wait, sizeof(int));

    //Zmazanie pamate a zavrenie
    shm_unlink("/xgulan00mem");
    close(shm_akcia);
    shm_unlink("/xgulan00serf");
    close(shm_serf);
    shm_unlink("/xgulan00hacker");
    close(shm_hacker);
    shm_unlink("/xgulan00pocetmolo");
    close(shm_pocetmolo);
    shm_unlink("/xgulan00pocetboarding");
    close(shm_pocetboarding);
    shm_unlink("/xgulan00moloship");
    close(shm_moloship);
    shm_unlink("/xgulan00landingcm");
    close(shm_landing_CM);
    shm_unlink("/xgulan00landingpocet");
    close(shm_landing_pocet);
    shm_unlink("/xgulan00pocetfinishedwait");
    close(shm_pocet_finished_wait);
}

/*
Funkcia na ukoncovanie v pripade chyby
*/
void ukoncenie(int sighandler) {
    (void)sighandler; //kvoli prekladacu
    kill(getpid(), SIGTERM);
    clean_dealokacia();
    exit(CHYBA2);
}


/*
Funkcia na alokovanie zdrojov, ktore pouzivame v projekte, napr semafory, zdielana pamat...
*/
void alokacia_zdrojov()
{
    //Vytvorenie semaforov 1nazov semaforu 2flagy 3prava 4pociatocna hodnota
    sem_vypis = sem_open("/xgulan00start", O_CREAT | O_EXCL, 0644, 1);
    molo_wait = sem_open("/xgulan00molowait", O_CREAT | O_EXCL, 0644, 0);
    hacker_boarding_wait = sem_open("/xgulan00hackerboardingwait", O_CREAT | O_EXCL, 0644, 0);
    serf_boarding_wait = sem_open("/xgulan00serfboardingwait", O_CREAT | O_EXCL, 0644, 0);
    memberc_wait = sem_open("/xgulan00membercwait", O_CREAT | O_EXCL, 0644, 0);
    captain_wait = sem_open("/xgulan00captainwait", O_CREAT | O_EXCL, 0644, 0);
    moloship_wait = sem_open("/xgulan00volnowait", O_CREAT | O_EXCL, 0644, 1);
    plavba_wait = sem_open("/xgulan00plavbawait", O_CREAT | O_EXCL, 0644, 0);
    landing_wait = sem_open("/xgulan00landingwait", O_CREAT | O_EXCL, 0644, 0);
    //Ak sa nesravne alokovali, vyhod chybu a skonci program
    if(sem_vypis == SEM_FAILED || molo_wait == SEM_FAILED || hacker_boarding_wait == SEM_FAILED || serf_boarding_wait == SEM_FAILED || memberc_wait == SEM_FAILED || captain_wait == SEM_FAILED || moloship_wait == SEM_FAILED || plavba_wait == SEM_FAILED || landing_wait == SEM_FAILED)
    {
        fprintf(stderr,"Nastala chyba pri vytvarani zdroju!\n");
        clean_dealokacia();
        exit(CHYBA2);
    }

    //Vytvorenie zdielanej pamate 1nazov zdielanej pamate 2flagy 3prava
    shm_akcia = shm_open("/xgulan00mem", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_serf = shm_open("/xgulan00serf", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_hacker = shm_open("/xgulan00hacker", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_pocetmolo = shm_open("/xgulan00pocetmolo", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_pocetboarding = shm_open("/xgulan00pocetboarding", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_moloship = shm_open("/xgulan00moloship", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_landing_CM = shm_open("/xgulan00landingcm", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_landing_pocet = shm_open("/xgulan00landingpocet", O_CREAT | O_EXCL | O_RDWR, 0644);
    shm_pocet_finished_wait = shm_open("/xgulan00pocetfinishedwait", O_CREAT | O_EXCL | O_RDWR, 0644);
    //Ak sa nespravne vytvorila zdielana pamat, vytlac na stderr chybu a skonci
    if(shm_akcia == ERROR || shm_serf == ERROR || shm_hacker == ERROR || shm_pocetmolo == ERROR || shm_pocetboarding == ERROR || shm_moloship == ERROR || shm_landing_pocet == ERROR || shm_landing_CM == ERROR || shm_pocet_finished_wait == ERROR)
    {
        fprintf(stderr,"Nastala chyba pri vytvarani zdroju!\n");
        clean_dealokacia();
        exit(CHYBA2);
    }

    //Vytvorenie miesta v zdielanej pamate pre INTeger
    ftruncate(shm_akcia, sizeof(int));
    ftruncate(shm_serf, sizeof(int));
    ftruncate(shm_hacker, sizeof(int));
    ftruncate(shm_pocetmolo, sizeof(int));
    ftruncate(shm_pocetboarding, sizeof(int));
    ftruncate(shm_moloship, sizeof(int));
    ftruncate(shm_landing_CM, sizeof(int));
    ftruncate(shm_landing_pocet, sizeof(int));
    ftruncate(shm_pocet_finished_wait, sizeof(int));

    //Namapujeme zdielanu pamat 1NULL 2velkost pamate 3prava 4ci budu moct vyuzivat childy 5file descriptor 6offset
    cisloAkcie = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_akcia, 0);
    pocetSerf = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_serf, 0);
    pocetHacker = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_hacker, 0);
    pocetMolo = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_pocetmolo, 0);
    pocetBoarding = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_pocetboarding, 0);
    moloShip = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_moloship, 0);
    pocet_CM = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_landing_CM, 0);
    pocet_landing = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_landing_pocet, 0);
    pocet_finished_wait = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_pocet_finished_wait, 0);
    if(cisloAkcie == MAP_FAILED || pocetSerf == MAP_FAILED || pocetHacker == MAP_FAILED || pocetMolo == MAP_FAILED || pocetBoarding == MAP_FAILED || moloShip == MAP_FAILED ||pocet_CM == MAP_FAILED || pocet_landing == MAP_FAILED || pocet_finished_wait == MAP_FAILED)
    {
        fprintf(stderr,"Nastala chyba pri vytvarani zdroju!\n");
        clean_dealokacia();
        exit(CHYBA2);
    }

}

/*
Funkcia na vyber, kolko koho ma spustit na molo.
*/
void selektovanie()
{
    if(*pocetHacker == 4 && *pocetSerf == 0)
    {
        sem_post(hacker_boarding_wait);
        sem_post(hacker_boarding_wait);
        sem_post(hacker_boarding_wait);
        sem_post(hacker_boarding_wait);
        *pocetHacker = *pocetHacker - 4;
    }
    if(*pocetHacker == 4 && *pocetSerf == 1)
    {
        sem_post(hacker_boarding_wait);
        sem_post(hacker_boarding_wait);
        sem_post(hacker_boarding_wait);
        sem_post(hacker_boarding_wait);
        *pocetHacker = *pocetHacker - 4;
    }
    if(*pocetHacker == 0 && *pocetSerf == 4)
    {
        sem_post(serf_boarding_wait);
        sem_post(serf_boarding_wait);
        sem_post(serf_boarding_wait);
        sem_post(serf_boarding_wait);
        *pocetSerf = *pocetSerf - 4;
    }
    if(*pocetHacker == 1 && *pocetSerf == 4)
    {
        sem_post(serf_boarding_wait);
        sem_post(serf_boarding_wait);
        sem_post(serf_boarding_wait);
        sem_post(serf_boarding_wait);
        *pocetSerf = *pocetSerf - 4;
    }
    if((*pocetHacker == 2 && *pocetSerf == 2) || (*pocetHacker == 3 && *pocetSerf == 2) || (*pocetHacker == 2 && *pocetSerf == 3))
    {
        sem_post(hacker_boarding_wait);
        sem_post(serf_boarding_wait);
        sem_post(hacker_boarding_wait);
        sem_post(serf_boarding_wait);
        *pocetSerf = *pocetSerf - 2;
        *pocetHacker = *pocetHacker - 2;
    }

}

/*
Funkcia na zistenie, ci je proces CAPTAIN alebo MEMBER
*/
int ktosom()
{
    if(*pocetMolo == 5) //Ak su tam 5 na mole, tak automaticky je 5. clen kapitan, lebo vstupil posledny
    {
        return CAPTAIN;
    }
    else //AK su 4 uz vybere podla kriterii kapitana
    {
        if((*pocetHacker == 0 && *pocetSerf == 4) || (*pocetHacker == 4 && *pocetSerf == 0) || (*pocetHacker == 2 && *pocetSerf == 2)) //Ak jedna z podmienok potom kapitan
        {
           return CAPTAIN; 
        }
    }
    return MEMBER;
}

/*
Funkcia v ktorej vykonava svoje cinnosti Serf
*/
void SerfWork(int poradie, int pocet, int uspanie)
{
    //**********Started***********
    sem_wait(sem_vypis);
    (*cisloAkcie)++;
    fprintf(vypis, "%d: serf: %d: started\n", *cisloAkcie, poradie);
    sem_post(sem_vypis);


    sem_wait(moloship_wait); //zastavenie aby sa nedostali na molo pocas plavby lode
   

    //*************Waiting for boarding*************
    if(!(PODMIENKA_MOLA))
    {
        sem_post(molo_wait);
    }
    sem_wait(molo_wait);
    sem_wait(sem_vypis);
    (*cisloAkcie)++;
    (*pocetSerf)++;
    fprintf(vypis, "%d: serf: %d: waiting for boarding: %d: %d\n", *cisloAkcie, poradie, *pocetHacker, *pocetSerf);
    (*pocetMolo)++;
    sem_post(sem_vypis);
    int som = ktosom();

    if(PODMIENKA_MOLA) //ak je ich dostatok, tak nastavim molohsip na 1 a zastavim posielanie novych procesov na cakanie na molo
    {
         *moloShip = ZATVOR;
    }

    if(*moloShip == OTVOR) //neplavia sa tak posli proces dalej, na cakanie na molo
    {
        sem_post(moloship_wait);
    } 

    //***********Boarding*******************
    if(PODMIENKA_MOLA) //ak ich je uz dostatok, tak vybere kolko moze pustit dalej 
    {
        sem_wait(sem_vypis);
        selektovanie();
        sem_post(sem_vypis); 
    }
    sem_wait(serf_boarding_wait);
    sem_wait(sem_vypis);
    (*cisloAkcie)++;
    fprintf(vypis, "%d: serf: %d: boarding: %d: %d\n", *cisloAkcie, poradie, *pocetHacker, *pocetSerf);
    sem_post(sem_vypis);

    (*pocetBoarding)++;
    if(*pocetBoarding == 4) //vsetci sa pockaju aby mohli ist do member/captain, vsetci 4 naraz
    {
        sem_post(memberc_wait);
        sem_post(memberc_wait);
        sem_post(memberc_wait);
        sem_post(memberc_wait);
        *pocetBoarding = POCIATOK;
    }


    //***********Member / Captain****************
    sem_wait(memberc_wait);
    sem_wait(sem_vypis);
    (*cisloAkcie)++;
    if(som == CAPTAIN) //printuje podla toho ci je kapitan alebo member
    {
        fprintf(vypis, "%d: serf: %d: captain\n", *cisloAkcie, poradie);
        (*pocet_CM)++;
    }
    else
    {
        fprintf(vypis, "%d: serf: %d: member\n", *cisloAkcie, poradie);
        (*pocet_CM)++;
    }
    sem_post(sem_vypis);

    sem_wait(sem_vypis);
    if(*pocet_CM == 4) //aby mohli ist vsetci naraz 
    {
        sem_post(plavba_wait);
        sem_post(plavba_wait);
        sem_post(plavba_wait);
        sem_post(plavba_wait);
        *pocet_CM = POCIATOK;
        *pocetMolo = *pocetMolo - 4;
    }
    sem_post(sem_vypis);


    //************Plavba uspanie kapitana**************
    sem_wait(plavba_wait);
    if(som == CAPTAIN) //captain sa uspi v tejto vetve a potom spusti procesy aj seba dalej
    {
        usleep((rand() % uspanie + POSUNUTIE)*TOMS);
        sem_post(captain_wait);
        sem_post(captain_wait);
        sem_post(captain_wait);
        sem_post(captain_wait);
    }
    sem_wait(captain_wait);

    
    //******************Landing*******************
    sem_wait(sem_vypis);
    (*pocet_landing)++;
    (*pocet_finished_wait)++;
    (*cisloAkcie)++;
    fprintf(vypis, "%d: serf: %d: landing: %d: %d\n", *cisloAkcie, poradie, *pocetHacker, *pocetSerf);
    sem_post(sem_vypis);
    if (*pocet_landing == 4) //pockaju sa  apotom zapne posielanie na cakanie na molo
    {
        *pocet_landing = POCIATOK;
        *moloShip = OTVOR;
        sem_post(moloship_wait);

    }
    if (*pocet_finished_wait == (ZDVOJNASOB*pocet)) //ak presli vsetci moze pokracovat na Finished
    {
        for(int c = POCIATOK; c < (ZDVOJNASOB*pocet); c++) //otovrime semafor kazdemu
        {
            sem_post(landing_wait);
        }

    }
    sem_wait(landing_wait);

    //*****************Finished******************
    sem_wait(sem_vypis);
    (*cisloAkcie)++;
    fprintf(vypis, "%d: serf: %d: finished\n", *cisloAkcie, poradie);
    sem_post(sem_vypis);
    close_semafory();
}

/*
Funkcia v ktorej vykonava svoje cinnosti Hacker
*/
void HackerWork(int poradie, int pocet, int uspanie)
{
    //*************Started*******************
    sem_wait(sem_vypis);
    (*cisloAkcie)++;
    fprintf(vypis, "%d: hacker: %d: started\n", *cisloAkcie, poradie);
    sem_post(sem_vypis);
    

    sem_wait(moloship_wait); //zastavuje ak sa lod plavi
   

    //**************Waiting for boarding**************
    if(!(PODMIENKA_MOLA)) //kontroluje, kolkych este treba poslat
    {
        sem_post(molo_wait);
    }
    sem_wait(molo_wait);
    sem_wait(sem_vypis);
    (*cisloAkcie)++;
    (*pocetHacker)++;
    fprintf(vypis, "%d: hacker: %d: waiting for boarding: %d: %d\n", *cisloAkcie, poradie, *pocetHacker, *pocetSerf);
    (*pocetMolo)++;
    sem_post(sem_vypis);
    int som = ktosom();

    if(PODMIENKA_MOLA) // nastavi na 1 aby sa nemohli posielat procesy na cakanie na molo, ked sa lod plavi alebo nastupuju...
    {
         *moloShip = ZATVOR;
    }

    if(*moloShip == OTVOR) //posli proces na cakanie na molo, ak sa lod neplavi
    {
        sem_post(moloship_wait);
    } 
 


    //**************Boarding*********************
    if(PODMIENKA_MOLA) //ak je dostatok, tak vybera koho spustit
    {
        sem_wait(sem_vypis);
        selektovanie();
        sem_post(sem_vypis);
    }
    sem_wait(hacker_boarding_wait);
    sem_wait(sem_vypis);
    (*cisloAkcie)++;
    fprintf(vypis, "%d: hacker: %d: boarding: %d: %d\n", *cisloAkcie, poradie, *pocetHacker, *pocetSerf);
    (*pocetBoarding)++;
    sem_post(sem_vypis);
    if(*pocetBoarding == 4) //aby spustili naraz vsetkych na Member/Captain
    {
        sem_post(memberc_wait);
        sem_post(memberc_wait);
        sem_post(memberc_wait);
        sem_post(memberc_wait);
        *pocetBoarding = POCIATOK;
    }


    //***************Member / Captain**************
    sem_wait(memberc_wait);
    sem_wait(sem_vypis);
    (*cisloAkcie)++;
    if(som == CAPTAIN) //podla toho ci je member alebo captain tak tlaci hlaksu
    {
        fprintf(vypis, "%d: hacker: %d: captain\n", *cisloAkcie, poradie);
        (*pocet_CM)++;
    }
    else
    {
        fprintf(vypis, "%d: hacker: %d: member\n", *cisloAkcie, poradie);
        (*pocet_CM)++;
    }
    sem_post(sem_vypis);

    sem_wait(sem_vypis);
    if(*pocet_CM == 4) //aby sa pockali a pekne spolu vsetci isli douspania
    {
        sem_post(plavba_wait);
        sem_post(plavba_wait);
        sem_post(plavba_wait);
        sem_post(plavba_wait);
        *pocet_CM = POCIATOK;
        *pocetMolo = *pocetMolo - 4;
    }
    sem_post(sem_vypis); 


    //***********Plavba uspanie kapitana**************
    sem_wait(plavba_wait);
    if(som == CAPTAIN) //uspi sa kapitan a ostatny cakaju na zobudenie
    {
        usleep((rand() % uspanie + POSUNUTIE)*TOMS);
        sem_post(captain_wait);
        sem_post(captain_wait);
        sem_post(captain_wait);
        sem_post(captain_wait);
    }
    sem_wait(captain_wait);

    
    //**************Landing************
    sem_wait(sem_vypis);
    (*pocet_landing)++;
    (*pocet_finished_wait)++;
    (*cisloAkcie)++;
    fprintf(vypis, "%d: hacker: %d: landing: %d: %d\n", *cisloAkcie, poradie, *pocetHacker, *pocetSerf);
    sem_post(sem_vypis);
    if (*pocet_landing == 4) //ak su 4 tak mozem spsutit prichod na molo 
    {
        *pocet_landing = POCIATOK;
        *moloShip = OTVOR;
        sem_post(moloship_wait);

    }
    if (*pocet_finished_wait == (ZDVOJNASOB*pocet)) //ak su vsetci tak mozem ich pustit na tlacenie finished
    {
        for(int c = POCIATOK; c < (ZDVOJNASOB*pocet); c++) //vsetkym otovrim semafor
        {
            sem_post(landing_wait);
        }

    }
    sem_wait(landing_wait);

    //**************Finished**************
    sem_wait(sem_vypis);
    (*cisloAkcie)++;
    fprintf(vypis, "%d: hacker: %d: finished\n", *cisloAkcie, poradie);
    sem_post(sem_vypis);
    close_semafory();
}

/*
Hlavna funkcia main, kde spracovava argumenty a konrolujem ich vstypy, vytvaram procesy 
*/
int main(int argc, char *argv[])
{
    signal(SIGTERM, ukoncenie); //odchytenie signalov a nasledne volanie funkcie
    signal(SIGINT, ukoncenie); 

    srand(time(0)); //naozaj nahodne cislo

    int pole[4]; //deklarovanie pola, kam ukladam jednotlive argumenty, pocet 4, 0prvok - pocet osob v skupone, 1prvok -doba generovnaia procsu hacker, 2-doba generovania procesu serf, 3-uspanie
    if (argc == 5) //musia byt 5, inak vypisem chybu, mame 5 , nulty je cesta 
    {
        int a = POCIATOK; //premenna
        char *chyby; //tam ukladam chybu
        char *argumenty; //sem argumenty
        while (a < 4)       // naplnenie pola jednotlivymi argumentami pre lahsiu pracu
        {
            argumenty = argv[a + POSUNUTIE]; 
            pole[a] = strtod(argumenty, &chyby);
            if (chyby == argumenty) //ak je argument v nespravnom formate
            {
                fprintf(stderr, "Argumenty neboli zadane spravne!");
                return CHYBA;
            }
                if (*chyby != '\n' && *chyby != 0) // ak je argument v nespravnom formate
            {
                fprintf(stderr, "Argumenty neboli zadane spravne!");
                return CHYBA;
            }
            a++;
        }
        if(((pole[0] % 2) != 0) || (pole[0] <= DOLNAHRANICA)) //ak argument neodpoveda pozadovanemu intervalu
        {
            fprintf(stderr, "Argumenty neboli zadane spravne!");
            return CHYBA;
        }
        if((pole[1] < DOLNAHRANICA) || (pole[1] > HORNAHRANICA)) //ak argument neodpoveda pozadovanemu intervalu
        {
            fprintf(stderr, "Argumenty neboli zadane spravne!");
            return CHYBA;
        }
        if((pole[2] < DOLNAHRANICA) || (pole[2] > HORNAHRANICA)) //ak argument neodpoveda pozadovanemu intervalu
        {
            fprintf(stderr, "Argumenty neboli zadane spravne!");
            return CHYBA;
        }
        if((pole[2] < DOLNAHRANICA) || (pole[2] > HORNAHRANICA)) //ak argument neodpoveda pozadovanemu intervalu
        {
            fprintf(stderr, "Argumenty neboli zadane spravne!");
            return CHYBA;
        }
    }
    else //AK neni 5 argumentov, ale viac alebo menej vypis na stderr chybu
    {
        fprintf(stderr, "Argumenty neboli zadane spravne!");
        return CHYBA;
    }


    if ((vypis = fopen("rivercrossing.out", "w"))== NULL) // otvorenie suboru + test chyby
    {
        fprintf(stderr,"Subor sa nepodarilo otvorit!\n");
        return CHYBA2;
    }
    setbuf(vypis, NULL); //nastavenie bufferu aby sa vypisovalo do suboru po riadkoch

    alokacia_zdrojov();

    //**************Hackers**************
    //Vytvaranie Hackerov, funkcii a cinnosti k tomu    
    pid_t Hacker; 
    pid_t MasterOfHacker = fork(); //vytvorime child proces Masterofhacker
    if (MasterOfHacker == DIETA) // Child
    {
        for (int i = POCIATOK; i < pole[0]; ++i) // v cykle vytvaram procesy hackerov 
        {
            usleep((rand() % pole[1] + POSUNUTIE)*TOMS); //uspanie na nahodnu dobu
            Hacker = fork(); //vytvorime child process Hacker
            if (Hacker == DIETA) //ak je dieta vykonava svoje cinnosti
            {
                HackerWork(i + POSUNUTIE, pole[0], pole[3]);
                close_semafory();
                fclose(vypis);
                exit(OK);
            }
            if(Hacker < 0) //Child nebol spravne vytvoreny, chyba a vypis na stderr a zabijem uz vytvorene procesy
            {
                fprintf(stderr,"Nepodaril sa vytvorit potomka Hacker!\n");
                kill(MasterOfHacker, SIGTERM);
                ukoncenie(0);
                exit(CHYBA2);
            }
        }
        if(Hacker > 0) //Parent
        {
            while(wait(NULL) > 0); //to cakanie
        }
        close_semafory();
        fclose(vypis);
        exit(OK);
    }
    if(MasterOfHacker < DIETA)
    {
        fprintf(stderr,"Nepodaril sa vytvorit potomka!\n");
        kill(MasterOfHacker, SIGTERM);
        ukoncenie(0);
        exit(CHYBA2);
    }

    //*************Serfs**************
    //Vytvaranie serfov, funkcii a cinnosti jemu risluchajucemu
    pid_t Serf;
    pid_t MasterOfSerf = fork(); //vytvorime child proces MasterofSerf
    if (MasterOfSerf == DIETA) // Child
    {
        for (int j = POCIATOK; j < pole[0]; ++j) //v cykle vytvaram procesy serfov 
        {
            usleep((rand() % pole[2] + POSUNUTIE)*TOMS); //uspanie na nahodnu dobu
            Serf = fork(); //vytvorime child process Serf
            if (Serf == DIETA) //ak dieta tak vykonava svoje cinnosti
            {
                SerfWork(j + POSUNUTIE, pole[0], pole[3]);
                close_semafory();
                fclose(vypis);
                exit(OK);
            }
            if(Serf < 0) //Child nebol spravne vytvoreny, chyba a vypis na stderr a zabijem uz vytvorene procesy
            {
                fprintf(stderr,"Nepodarilo sa vytvorit potomka Serf!\n");
                kill(MasterOfSerf, SIGTERM);
                ukoncenie(0);
                exit(CHYBA2);
            }
        }
        if(Serf > 0) //Perent
        {
            while(wait(NULL) > 0); //cakanie
        }
        close_semafory();
        fclose(vypis);
        exit(OK);
    }
    if(MasterOfHacker < DIETA)
    {
        fprintf(stderr,"Nepodaril sa vytvorit potomka!\n");
        kill(MasterOfHacker, SIGTERM);
        kill(MasterOfSerf, SIGTERM);
        ukoncenie(0);
        exit(CHYBA2);
    }
        
    
    if((MasterOfHacker > 0) && (MasterOfSerf > 0)) //cakanie na deti halvneho procesu
    {
        while(wait(NULL) > 0); //cakam
    }
    
    //fprintf(vypis,"Subor sa nepodarilo otvorit!\n");
    clean_dealokacia();

    return OK;
}