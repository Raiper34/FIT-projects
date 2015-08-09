/*
 * IPK Projekt 1 - Klient <==> Server aplikacia (Server)
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
#include <regex.h>
#include <locale.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sys/wait.h>
#include <signal.h>

using namespace std;

#define ROVNAJUSA (0)
#define CHYBA (-1)
#define OK (0)
#define MAX (1000)
#define ZAC (0)
#define NOFLAG (0)
#define KONIEC (-1)
#define POSUNUTIE (1)
#define STALEDOKOLA (1)
#define POCETSPOJENI (5)

/*
 *Funkcia na spracovanie passwd pomocou kniznice pwd.h
 *Argumenty: loginuid podla ktoreho hladam, moznosti informacie ktore chcem ziskat
 *Vracia: dany zaznam v danom formate
 */
string passwdSpracuj(string loginuid, string moznosti)
{
  string cast = "";
  string pomocny = "";
  string pomocnlogin; //pomocny lgoin aby som mohol vyhladavat ako case insensitive
  struct passwd *pswd;
  stringstream pomocny2;
  stringstream pomocnyuid;
  bool najdeny = false; //indikuje ci bol zaznam najdeny a podla toho nieco vraciame
  if(moznosti.find('l') != string::npos)
  {
    setpwent(); //ideme na zaciatok
    while((pswd = getpwent()) != NULL) //prechadzame vsetky zaznamy
    {
      pomocnlogin = pswd->pw_name;
      transform(pomocnlogin.begin(), pomocnlogin.end(), pomocnlogin.begin(), ::tolower);
      transform(loginuid.begin(), loginuid.end(), loginuid.begin(), ::tolower);
      if(pswd->pw_name == loginuid) //ak tam nieco take ako hladame je budeme vytahovat konkretne informacie
      {
        najdeny = true;
        cast.append(pswd->pw_name);
        cast.append("<>#$");
        for(char& c : moznosti) //postupne pridavam do stringu jednotlive informacie ktore pozadujeme
        {
          if(c == 'L')
          {
            cast.append(pswd->pw_name);
            cast.append(" ");
          }
          else if(c == 'U')
          {
            pomocny2.str(string());
            pomocny2.clear();
            pomocny2 << pswd->pw_uid;
            cast.append(pomocny2.str());
            cast.append(" ");
          }
          else if(c == 'G')
          {
            pomocny2.str(string());
            pomocny2.clear();
            pomocny2 << pswd->pw_gid;
            cast.append(pomocny2.str());
            cast.append(" ");
          }
          else if(c == 'H')
          {
            pomocny = pswd->pw_dir;
            cast.append(pomocny.c_str());
            cast.append(" ");
          }
          else if(c == 'N')
          {
            pomocny = pswd->pw_gecos;
            cast.append(pomocny.c_str());
            cast.append(" ");
          }
          else if(c == 'S')
          {
            pomocny = pswd->pw_shell;
            cast.append(pomocny.c_str());
            cast.append(" ");
          }
          if(moznosti.length() > 1) //ak je to posledna informacia ktoru pozadujem, tak pridam aj odriadkovanie
          {
            if(moznosti.at(moznosti.length() - 2) == c)
            {
              cast = cast.substr(0, cast.size() - POSUNUTIE);
              cast.append("\n");
            }
          }
        }
      }
    }
  }
  else //cele analogia skoro ako hore, iba hladam podla uid 
  {
    setpwent();
    while((pswd = getpwent()) != NULL)
    {
      pomocnyuid.str(string());
      pomocnyuid.clear();
      pomocnyuid << pswd->pw_uid;
      if(loginuid.c_str() == pomocnyuid.str())
      {
        najdeny = true;
        cast.append(pswd->pw_name);
        cast.append("<>#$");
        for(char& c : moznosti) 
        {
          if(c == 'L')
          {
            cast.append(pswd->pw_name);
            cast.append(" ");
          }
          if(c == 'U')
          {
            pomocny2.str(string());
            pomocny2.clear();
            pomocny2 << pswd->pw_uid;
            cast.append(pomocny2.str());
            cast.append(" ");
          }
          if(c == 'G')
          {
            pomocny2.str(string());
            pomocny2.clear();
            pomocny2 << pswd->pw_gid;
            cast.append(pomocny2.str());
            cast.append(" ");
          }
          if(c == 'H')
          {
            pomocny = pswd->pw_dir;
            cast.append(pomocny.c_str());
            cast.append(" ");
          }
          if(c == 'N')
          {
            pomocny = pswd->pw_gecos;
            cast.append(pomocny.c_str());
            cast.append(" ");
          }
          if(c == 'S')
          {
            pomocny = pswd->pw_shell;
            cast.append(pomocny.c_str());
            cast.append(" ");
          }
          if(moznosti.length() > 1)
          {
            if(moznosti.at(moznosti.length() - 1) == c)
            {
              cast = cast.substr(0, cast.size() - 1);
              cast.append("\n");
            }
          }
        }
      }
    }
  }
  if(najdeny == true) //ak bol najdeny
  {
    if(moznosti.length() < 2)
    {
      return("?!@#$\n");
    }
    else //nnasiel sa
    {
      if(cast == "\n") //ale ak obsahoval iba odriakdovanie vraciam prazdny retazec
      {
        return("?!@#$\n");
      }
      else //inak vraciam co som nasiel
      {
        return(cast.c_str());
      }
    }
  }
  else //ak nebol najdeny vraciam chyby podla toho co som hladal
  {
    if(moznosti.find('l') != string::npos)
    {
      return("Chyba: neznamy login " + loginuid + "\n");
    }
    else
    {
      return("Chyba: neznamy uid " + loginuid + "\n");
    }
  }
}

/*
 *Funkcia ktora zabezpecuje komunikaciu s clientom
 *Argumenty: port cez ktory sa komunikuje
 *Vracia: OK ak prebehlo vsetko v poriadku, inak CHYBA
 */
int komunikaciaClient(string port)
{
  int socketik, Psocketik, sinlen;
  struct sockaddr_in sin;
  char buffer[MAX]; //pomocny buffer na postupne skladanie prichadzajuceho stringu
  int pocetRecv = ZAC;
  struct hostent * hp;
  string overenie; //pomocny string, na ulozenie prveho riakdy spravy ci sa naozaj jedna o moj protokol
  string moznosti; //string uchovavajci moznosti, teda to co ccheme z passwd zistit
  string loginuid; //string, ktory obsahuje bud login alebo uid na hladanie sa vyuziva
  string cast; //cast

  if((socketik = socket(PF_INET, SOCK_STREAM, NOFLAG)) < 0) //vytvori socket
  {
    cerr << "Chyba vytvarania socketu!" << endl;
    return(CHYBA);
  }

  sin.sin_family = PF_INET; //protokol
  sin.sin_port = htons(atoi(port.c_str())); //nastavi port
  sin.sin_addr.s_addr  = INADDR_ANY; //nastavi IP adresu

  if(bind(socketik, (struct sockaddr *)&sin, sizeof(sin)) < 0) //priradi adresu socketu do premenej
  {
    cerr << "Chyba pri pridelovani adresy socketu!" << endl;
    return(CHYBA);
  }

  if(listen(socketik, POCETSPOJENI))
  { 
    cerr << "Chyba pri nadviazani komunikacie!" << endl;
    return(CHYBA);
  }

  /////////////////////////While aby bol server neustale spusteny ///////////////////////////////
  sinlen = sizeof(sin);
  while(STALEDOKOLA) 
  {
    string odpoved = "";
    string prijate = "";
    if((Psocketik = accept(socketik, (struct sockaddr *) &sin, (socklen_t*)&sinlen)) < 0) //akceptuje spojenie od klienta a ulozi socket do t
    {
      cerr << "Chyba pri akceptovani spojenia!" << endl;
      return(CHYBA);
    }

    pid_t proces = fork(); //forknem si proces kvoli konkurenci schopnosti
    if(proces == 0) //child proces
    {
      hp = (struct hostent *)gethostbyaddr((char *)&sin.sin_addr,4,AF_INET);

      //////////////Prijmanie spravy od klienta//////////
      while((pocetRecv = recv(Psocketik, buffer, sizeof(buffer), NOFLAG) ) > 0) //prijmam spravu z clienta
      {
        if(pocetRecv < 0) //ak nastala chyba prijmania dat
        {
          cerr << "Chyba prijmanania dat!" << endl;
          return(CHYBA);
        }
        prijate.append(buffer,pocetRecv);
        if(pocetRecv < MAX - POSUNUTIE) //ak uz neni co prijmat ukoncim cyklus
        {
          break;
        }
      }

      ///////////Spracovanie toho co potrebujem posalt spat////////
      istringstream parsovanieSocketu(prijate.c_str());  
      getline(parsovanieSocketu, overenie);
      if(overenie.compare("<<xgulan00 protocol 1.0>>") != 0) //overim ci sa jedna o moj protokol
      {
        cerr << "Nezname pripojenie!" << endl;
        close(Psocketik);
        return(CHYBA);
      }
      getline(parsovanieSocketu, moznosti); //ziskam informacie o tom, ktore informacie chcem poslat spat
      while(getline(parsovanieSocketu, loginuid)) //ziskam loginy alebo uidy, ktore hladam a spustum funkciu na ich hladanie
      {
        cast = passwdSpracuj(loginuid, moznosti); //vytiahnem si z paswd konkretnu polozku
        if(odpoved.find(cast.c_str()) == string::npos) //ak neni prazdny tak ju pridam k celkovej odpovednej sprave
        {
         odpoved.append(cast.c_str());
        }
      }

      //////////////Posielanie spravy klientovi//////////
      if (send(Psocketik, odpoved.c_str(), strlen(odpoved.c_str()), NOFLAG) < 0)
      {
        cerr << "Chyba odosielania spravy klientovi!" << endl;
        return(CHYBA);
      }
      if(close(Psocketik) < 0) //uzavetie socketu
      { 
        cerr << "Chyba zatvarania socketu!" << endl;
        return(CHYBA);
      } 
      break;
    }
    else if(proces < 0) //chyba vytvarania procesu
    {
      cerr << "Chyba vytvarania procesu!" << endl;
      close(Psocketik);
      return(CHYBA);
    }
  }
  
  if (close(socketik) < 0) //uzavretie socketu
  {
    cerr << "Chyba zatvarania socketu!" << endl;
    return(CHYBA);
  }
  return(OK);

}

/*
 *Funkcia, ktora odchytava signaly a v pripade odchytu ukonci program
 *Argument: signal
 */
void sigHandler(int signal) 
{
  cerr << "Nahle ukoncnie serveru!" << signal << endl;
  exit(OK);
}

/*
 *Funkcia main, ktora spracovava parametre a vola funkciu na komunikaciu s clientom
 */
int main (int argc, char *argv[])
{
  signal(SIGTERM, sigHandler);
  signal(SIGINT, sigHandler);
  signal(SIGCHLD, SIG_IGN);
  int argumenty;
  string port;
  while ((argumenty = getopt(argc, argv, "p:")) != KONIEC) //ziskam vsetky argumenty
  {
    switch (argumenty)
    {
      case 'p':
        port = optarg;
        break;
      default: //ak tam je nieco ine
        cerr << "Argumenty neboli zadane spravne!" << endl;
        return(CHYBA);
        break;
    }
  }
  if(port.compare("") == ROVNAJUSA) //neni su zadane potrebne argumenty
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
  if(optind != argc) //je tam naviac argumentov
  {
    cerr << "Argumenty neboli zadane spravne!" << endl;
    return(CHYBA);
  }

  if(komunikaciaClient(port) == OK)
  {
    return(OK);
  }
  else
  {
    return(CHYBA);
  }
}