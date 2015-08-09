/*
 * IPK Projekt 1 - Klient <==> Server aplikacia
 * Autor: Filip Gulan xgulan00@stud.fit.vutbr.cz
 * Datum: 1.4.2015
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

#define ROVNAJUSA (0)
#define CHYBA (-1)
#define OK (0)
#define MAX (1000)
#define ZAC (0)
#define NOFLAG (0)
#define KONIEC (-1)
#define POSUNUTIE (1)

/*
 *Funkcia na komunikaciu so serverom,pripaja sa na server, odosiela a prijma socket
 *Argumenty: host, cislo portu, moznosti - string ktory odosiela serveru
 *Vracia: OK v pripade ze vsekto prebehlo v poriadku, inak CHYBA
 */
int komunikaciaServer(string host, string port, string moznosti)
{
  int socketik; //premena na socket
  struct sockaddr_in sin;
  struct hostent *hptr;
  string sprava; //sprava ktora sa prijme zo serveru
  char buffer[MAX]; //pomocny buffer na prijamnie hocikoko dlhych stringov
  int pocetRecv = ZAC; //pocet ci nam recv nieco vratil a ci to mam hadzat do bufferu
  string vypis; //sluzi na finalny vypis kedy sa rozhodujem medzi tym co ide na stderr a stdin
  unsigned pozicia;

  if ((socketik = socket(PF_INET, SOCK_STREAM, 0 )) < 0) //vytvaranie socketu
  {
    cerr << "Chyba vytvarania socketu!" << endl;
    return(CHYBA);
  }

  sin.sin_family = PF_INET; //protokol
  sin.sin_port = htons(atoi(port.c_str()));  //cislo portu
  if ( (hptr =  gethostbyname(host.c_str()) ) == NULL) //ziskanie mena hostu
  {
    cerr << "gethostname chyba: " << host.c_str() << endl;
    return(CHYBA);
  }

  memcpy(&sin.sin_addr, hptr->h_addr, hptr->h_length);
  if (connect (socketik, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ) //pripajanie sa na server
  { 
    cerr << "Chyba pripojenie!" << endl;
    return(CHYBA);
  }

  /////////////Posielanie spravy serveru//////////////
  if(send(socketik, moznosti.c_str(), strlen(moznosti.c_str()) + POSUNUTIE, NOFLAG) < 0 ) //odosielanie na server
  {
    cerr << "Chyba odosielania dat!" << endl;
    return(CHYBA);
  }
  ////////////////Prijmanie spravy z serveru///////////
  sprava = "";
  while((pocetRecv = recv(socketik, buffer, sizeof(buffer), NOFLAG) ) > 0 )
  {
    if(pocetRecv < 0)
    {
      cerr << "Chyba prijmana dat!" << endl;
      return(CHYBA);
    }
    sprava.append(buffer,pocetRecv);
    if(pocetRecv < MAX - POSUNUTIE) //ak uz neni co prijmat ukoncim cyklus
    {
      break;
    }
  }

  ///////////////Finalny vypis///////////
  istringstream parsovanie(sprava.c_str()); 
  while(getline(parsovanie, vypis))
  {
    if(vypis.find("?!@#$") != string::npos)
    {
      continue;
    }
    if(vypis.find("Chyba: ") == string::npos) //ak to neni chyba tak davam na stdin
    {
      pozicia = vypis.find("<>#$");
      int dlzka = vypis.length();
      vypis = vypis.substr(pozicia + 4, dlzka - pozicia - 1);
      printf("%s\n", vypis.c_str());
    }
    else //inak chyba patri na stderr
    {
      cerr << vypis << endl;
    }
  }
  
  if (close(socketik) < 0) //uzavrieme socket
  { 
    cerr << "Cyba uzatvarania socketu" << endl;
    return(CHYBA);
  }

  return(OK);
}

/*
 *Funkcia main na spracovanie parametrov
 */
int main (int argc, char *argv[] )
{
  string host = "";
  string port = "";
  string login = "";
  string uid = "";
  string moznosti = "<<xgulan00 protocol 1.0>>\n";
  string poslednyLU = ""; //cely string, ktory budem odosielat socketom, vsetky potrebne info
  int Lc, Uc, Gc, Nc, Hc, Sc, hostC, portC; //pocty jendotlivych argumentov
  Lc = Uc = Gc = Nc = Hc = Sc = hostC = portC = 0;

  int argumenty;
  while ((argumenty = getopt(argc, argv, "h:p:l:u:LUGNHS")) != KONIEC) //ziskam vsetky argumenty
  {
    switch (argumenty)
    {
      case 'h':
        host = optarg;
        hostC++;
        break;
      case 'p':
        port = optarg;
        portC++;
        break;
      case 'l':
        optind--;
        login = "";
        for( ;optind < argc && *argv[optind] != '-'; optind++) //vsetky loginy ziskam 
        {
            login.append("\n");
            login.append(argv[optind]);       
        }
        if(login.compare("") != ROVNAJUSA)
        {
          poslednyLU = "l";
        }
        break;
      case 'u':
        optind--;
        uid = "";
        for( ;optind < argc && *argv[optind] != '-'; optind++) //vsetky loginy ziskam 
        { 
          uid.append("\n");
          uid.append(argv[optind]);       
        }
        if(uid.compare("") != ROVNAJUSA)
        {
          poslednyLU = "u";
        }
        break;
      case 'L':
        moznosti.append("L");
        Lc++;
        break;
      case 'U':
        moznosti.append("U");
        Uc++;
        break;
      case 'G':
        moznosti.append("G");
        Gc++;
        break;
      case 'N':
        moznosti.append("N");
        Nc++;
        break;
      case 'H':
        moznosti.append("H");
        Hc++;
        break;
      case 'S':
        moznosti.append("S");
        Sc++;
        break;
      default:
        cerr << "Argumenty neboli zadane spravne!" << endl;
        return(CHYBA);
        break;
    }
  }

  if(host.compare("") == ROVNAJUSA || port.compare("") == ROVNAJUSA || (login.compare("") == ROVNAJUSA && uid.compare("") == ROVNAJUSA)) //neni su zadane potrebne argumenty
  {
    cerr << "Argumenty neboli zadane spravne!" << endl;
    return(CHYBA);
  }

  if((port.find_first_not_of("0123456789") == string::npos) != true) //ak port neni cislo
  {
      cerr << "Argumenty neboli zadane spravne!" << endl;
      return(CHYBA);
  }
  if(atoi(port.c_str()) > 65535 || atoi(port.c_str()) < 1024) //port v nespravnom rozsahu
  {
    cerr << "Chyba rozsahu portu!" << endl;
    return(CHYBA);
  }

  if(poslednyLU.compare("l") == ROVNAJUSA) //ak bol posledny login vyhladavme podla loginu
  {
    moznosti.append("l");
    moznosti.append(login);
  }
  else if(poslednyLU.compare("u") == ROVNAJUSA) //ak bol posledny uid vyhladavame podla uidu
  {
    moznosti.append("u");
    moznosti.append(uid);
  }
  if(optind != argc) //je tam naviac argumentov
  {
    cerr << "Argumenty neboli zadane spravne!" << endl;
    return(CHYBA);
  }
  if(Lc > 1 || Uc > 1 || Gc > 1 || Nc > 1 || Hc > 1 || Sc > 1 || portC > 1 || hostC > 1) //ak tam su viacnasobne argumenty
  {
    cerr << "Argumenty neboli zadane spravne!" << endl;
    return(CHYBA);
  }

  if(komunikaciaServer(host, port, moznosti) == OK) //spustanie komunikacie so serverom
  {
    return(OK);
  }
  else
  {
    return(CHYBA);
  }
}