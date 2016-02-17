/*
 * ISA Projekt 2015 - Jednoduchy SNMP manager
 * Autor: Filip Gulan (xgulan00)
 * mail: xgulan00@stud.fit.vutbr.cz
 */

#include <stdlib.h>
//#include <stdio.h>
#include <string.h>
#include <fstream>
#include <netdb.h>
//#include <sys/socket.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <unistd.h>
//#include <vector>
//#include <time.h>
#include <sys/time.h>
//#include <bits/signum.h>
#include <signal.h>

using namespace std;

#define SNMP_PORT 161
#define SESTNASTKOVA 16
#define MAXSOCKETBUF 50000
#define MAXDLZKACAS 20
#define NOFLAG 0
#define POCETOID 22
#define CHYBA 1
#define OK 0
#define ZISTENIEROZHRANI 0
#define MILISEKUNDA 1000

/*
 * Funkcia na odchytenie signalu SIGINT akorektne ukoncenie programu s navratovym kodom 0
 * signal: odchyteny signal
 */
void sigIntFunkcia(int signal)
{
	exit(OK);
}

/*
 * Funkcia konvertuje sting hexa cislis na normalny string
 * data: hexa string ktory chcem konvertovat
 * Vracia: konvertovany normlany cisty string
 */
string hexaString(string data)
{
	string hexaString = "";
	for(int i = 0; i < data.length(); i += 2) //ideme po dvoch znakoch
	{
		string dvojica = data.substr(i,2); //dvojica reprezentuje znak
		char znak = (char)(int)strtol(dvojica.c_str(), NULL, SESTNASTKOVA); //premenime na znak
		hexaString.push_back(znak);
	}
	return hexaString;
}

/*
 * Funkcia konvertuje int na hexa cislov v string
 * cislo: ktore konvertujeme na hexa cislo v string
 * Vracia: hexa cislo v string
 */
string intNaHex(int cislo)
{
	stringstream stream;
	stream << hex << cislo; //konvertujeme na hexa
	string hexa = stream.str();
	if (hexa.length() % 2 != 0) //musi byt dvojica, ak je dlzka neparna, pridame 0 aby to boli dovjice
	{
		hexa.insert(0, "0");
	}
	return hexa;
}

/*
 * Funkcia na konverziu obycajneho stringu na hexa string, tj kazdy znak bude reprezentovany hexa cislo
 * text: ktory chceme konvertovat na hexa string
 * Vracia: hexa string
 */
string stringNaHex(string text)
{
	stringstream stream;
	string pomocny = "";
	string hexa = "";
	for(int i = 0; i < text.length(); i++) //idem po znaku a premienam na hexa dvojice
	{
		stream.str("");
		stream << hex << (int)((unsigned char)text[i]); //konvertujem znak na hexa
		pomocny = stream.str(); //aby som mohol pouzivat lenght a appendovat
		if (pomocny.length() % 2 != 0) //ak su to neni dvojice, tak pridam 0
		{
			pomocny.insert(0, "0");
		}
		hexa.append(pomocny.c_str());
	}
	return(hexa);
}

/*
 * Funkcia na vypis casu vo formate aky potrebujeme, teda s milisekundovou prestnostou
 */
void vypisCas()
{
	char formatovanyCas[MAXDLZKACAS];
	struct timeval cas;
	time_t aktualnyCas;
	gettimeofday(&cas, NULL);
	aktualnyCas = cas.tv_sec;
	string milisekundy = to_string(cas.tv_usec); //milisekundy musia byt zvlast
	for(int i = milisekundy.length(); i < 6; i++) //doplnenie 0 aby som bolo urobit substr na milisekundy
	{
		milisekundy = milisekundy.insert(0, "0");
	}
	strftime(formatovanyCas, MAXDLZKACAS, "%Y-%m-%d %H:%M:%S", gmtime(&aktualnyCas));
	cout << formatovanyCas << "." << milisekundy.substr(0, 3);
}

/*
 * Ziska z hexa stringu uz konretke value data a vrati ich ako string, ale uz v spravnom formate
 * sprava: z ktorej chceme ziskat value data
 * Vracia: string s datami v spravnom formate
 */
string ziskajValue(string sprava, int objekt)
{
	//value
	string datovyTyp = sprava.substr(0,2); //typ
	sprava.erase(0, 2); //typ
	unsigned long int dlzkaValue = stoul(sprava.substr(0, 2), nullptr, 16);
	int i = 1;
	while(dlzkaValue != (sprava.length()/2) - i)
	{
		i++;
		dlzkaValue= stoul(sprava.substr(0, 2*i), nullptr, 16);
	}
	sprava.erase(0, 2*i); //vymazem dlzku
	if(dlzkaValue == 0) //ak je dlzka 0 tak to je nul nic tam neni vraciam prazdny string
	{
		return "";
	}

	//Rozhodovanie odatovom type
	if(datovyTyp.compare("02") == 0 || datovyTyp.compare("42") == 0 || datovyTyp.compare("41") == 0 || datovyTyp.compare("43") == 0 || datovyTyp.compare("06") == 0) //integer a gauge32 a counter32 a timeticks a oid
	{
		if(objekt == 22) //ak je to objekt 22 tak pridam este oproti normalnemu bodku na zaciatok stringu
		{
			return to_string(stoul(sprava, nullptr, 16)).insert(0, ".");
		}
		return to_string(stoul(sprava, nullptr, 16));
	}
	else if(datovyTyp.compare("04") == 0) //string a octet string
	{
		if(objekt != 6) //ak to neni objekt 6, tak normalne vratim string
		{
			return hexaString(sprava);
		}
	}

	//neni to nic, vraciam data ako hexa dvojice s dvojbodkou
	string pomocny = "";
	while(sprava.length() != 0) //postupne prechadzam a berem dvojice a oddelujem dvojbodkou v novom strignu
	{
		pomocny.append(sprava.substr(0, 2));
		if(sprava.length() > 2) //ak tam je dalsia dvojica, teda toto nei posledna tak davam dvojbodku
		{
			pomocny.append(":");
		}
		sprava.erase(0, 2);
	}
	for(int i = 0; i < pomocny.length(); i++) //konverrtujem znaky na velke
	{
		pomocny[i] = toupper(pomocny[i]);
	}
	return pomocny;
}

/*
 * Dekoduje spravu, ktora prisla zo SNMP, sprava uz musi byt vo formate hexa stringu
 * sprava: sprava, ktoru budeme dekodovat
 * communityString: nase heslo k SNMP agentovi
 * Vracia: upraveny hexa string, ktory obsahuje uz iba oid a value
 */
string dekodujGetResponse(string sprava, string communityString, string oid)
{
	/////////////////////////////////message////////////////////////////
	if(sprava.substr(0, 2).compare("30") != 0) //kontrola sequencie
	{
		cerr << "SNMP Message typ sequencia nesuhlasi!" << endl;
		exit(CHYBA);
	}
	sprava.erase(0, 2); //30 typ sequencia
	unsigned long int velkostMessage = stoul(sprava.substr(0, 2), nullptr, 16); //ziskam dlzku spravy, dalsi hexa
	int i = 1;
	while(velkostMessage != (sprava.length()/2) - i) //cyklus na zistenie ci je dlzka spravna, ci nepotrebujem nahodou dalsie 2 hexa na dlzku
	{
		i++;
		velkostMessage = stoul(sprava.substr(0, 2*i), nullptr, 16); //ak neni dlzka vyhovujuca, ziskam dalsie 2 hexa
	}
	sprava.erase(0, 2*i); //vymazem dlzku

	//////////////////////////////version///////////////////////////////
	if(sprava.substr(0, 6).compare("020100") != 0) //kontrola verzie
	{
		cerr << "SNMP verzia nesuhlasi!" << endl;
		exit(CHYBA);
	}
	sprava.erase(0, 6); //vymazem verziu 02 typ integer 01 dlzka  00 hodnota

	//////////////////////////community string/////////////////////////////
	unsigned long int velkostCommunity = communityString.length();
	if(sprava.substr(0, 2).compare("04") != 0) //kontrola typu community strignu
	{
		cerr << "Comunity string typ octet string nesuhlasi!" << endl;
		exit(CHYBA);
	}
	if(sprava.substr(2, 2).compare(intNaHex(communityString.length())) != 0) //kontrola dlzky community stringu
	{
		cerr << "Dlzka community stringu nesuhlasi!" << endl;
		exit(CHYBA);
	}
	if(sprava.substr(4, velkostCommunity * 2).compare(stringNaHex(communityString)) != 0) //kontrola community stringu
	{
		cerr << "Comunity string nesuhlasi!" << endl;
		exit(CHYBA);
	}
	sprava.erase(0, (velkostCommunity * 2) + 4); //vymazem aj typ and dlzku aj hodnotu

	////////////////////////////////pdu///////////////////////////////
	if(sprava.substr(0, 2).compare("a2") != 0) //kontrola pdu
	{
		cerr << "PDU Get Response typ nesuhlasi!" << endl;
		exit(CHYBA);
	}
	sprava.erase(0, 2); //a0/01.. typ integer typ spravy bud request/request next...
	unsigned long int velkostPDU = stoul(sprava.substr(0, 2), nullptr, 16);
	i = 1;
	while(velkostPDU != (sprava.length()/2) - i)
	{
		i++;
		velkostPDU = stoul(sprava.substr(0, 2*i), nullptr, 16);
	}
	sprava.erase(0, 2*i); //vymazem velkost

	/////////////////////////requestid + error + error index/////////////////////////
	if(sprava.substr(0, 6).compare("020101") != 0) //kontrola request id
	{
		cerr << "Request ID nesuhlasi!" << endl;
		exit(CHYBA);
	}
	if(sprava.substr(6, 2).compare("02") != 0) //kontrola error typu samotnu chybu zanedbavam vypisujem iba prazdny string
	{
		cerr << "Typ error nesuhlasi!" << endl;
		exit(CHYBA);
	}
	if(sprava.substr(12, 2).compare("02") != 0) //kontrola error index tyupu samornu chybu zanedbavam vypisujem iba prazdny string
	{
		cerr << "Typ error indexu nesuhlasi!" << endl;
		exit(CHYBA);
	}
	sprava.erase(0, 18); //typ dlzka hodnota kazda ma

	///////////////////////////varbindlist//////////////////////////
	if(sprava.substr(0, 2).compare("30") != 0) //kontrola varbindlistu
	{
		cerr << "Varbindlist typ sequencia nesuhlasi!" << endl;
		exit(CHYBA);
	}
	sprava.erase(0, 2); //typ sequencia
	unsigned long int velkostVarListu = stoul(sprava.substr(0, 2), nullptr, 16);
	i = 1;
	while(velkostVarListu != (sprava.length()/2) - i)
	{
		i++;
		velkostVarListu = stoul(sprava.substr(0, 2*i), nullptr, 16);
	}
	sprava.erase(0, 2*i); //vymazem dlzku varbindlistu

	////////////////////////////////varbindtype///////////////////////////
	if(sprava.substr(0, 2).compare("30") != 0) //kontrola varbindu
	{
		cerr << "Varbindu typ sequencia nesuhlasi!" << endl;
		exit(CHYBA);
	}
	sprava.erase(0, 2); //typ sequencia
	unsigned long int velkostVarType = stoul(sprava.substr(0, 2), nullptr, 16);
	i = 1;
	while(velkostVarType != (sprava.length()/2) - i)
	{
		i++;
		velkostVarType = stoul(sprava.substr(0, 2*i), nullptr, 16);
	}
	sprava.erase(0, 2*i); //vymazem dlzku varbindu

	//////////////////////object identifier iba vymazem////////////////////////
	if(sprava.substr(0, 2).compare("06") != 0) //kontrola oid typu
	{
		cerr << "OI typ object identifier nesuhlasi!" << endl;
		exit(CHYBA);
	}
	unsigned long int velkostOID = stoul(sprava.substr(2, 2), nullptr, 16);
	sprava.erase(0, 4); //typ a velkost
	if(sprava.substr(0, velkostOID*2).compare(oid) != 0) //kontrola oidcka
	{
		cerr << "OID nesuhlasi!" << endl;
		exit(CHYBA);
	}
	sprava.erase(0, velkostOID*2); //samotny odi vymazem

	return sprava;
}

/*
 * Vytvara spravu/protokol, ktoru odosielame SNMP agentovi
 * communityString: heslo SNMP agenta
 * oid: oid objektu ktoreho chcceme ziskat
 * Vracia: vytvorenu spravu/protkol, ktoru mozme odoslat na SNMP
 */
string urobGetRequest(string communityString, string oid)
{
	string data = "";
	string value = "0500"; //05 type = null, 00 lenght = 0
	data.insert(0, value);

	data.insert(0, oid); //oid
	data.insert(0, intNaHex(oid.length()/2)); // dlzka oid
	data.insert(0, "06"); //06 typ = object identifier

	string varbind = "30"; //typ sequence
	varbind.append(intNaHex(data.length() / 2)); //dlzka varbindu
	data.insert(0, varbind); //samornty varbind

	string varbinList = "30"; //typ sequence
	varbinList.append(intNaHex(data.length() / 2)); //dlzka varbind listu
	data.insert(0, varbinList); //samotny varbindlist

	string errorIndex = "020100"; //02 type = integer, 01 dlzka, 00 hodnota
	data.insert(0, errorIndex);
	string error = "020100"; //02 type = integer, 01 dlzka, 00 hodnota
	data.insert(0, error);
	string requestId = "020101"; //02 type = integer, 01 dlzka, 01 hodnota
	data.insert(0, requestId);

	string pdu = "a0"; //pdu type = getRequest
	pdu.append(intNaHex(data.length() / 2)); //dlzka celeho pdu
	data.insert(0, pdu);

	communityString = stringNaHex(communityString); //comunity string
	communityString.insert(0, intNaHex(communityString.length() / 2)); //dlzka community stringu
	communityString.insert(0, "04"); //04 type = octet string
	data.insert(0, communityString);

	string version = "020100"; //02 type = integer, 01 dlzka, 01 hodnota
	data.insert(0, version);

	string message = intNaHex(data.length() / 2); //cela sprava dlzka
	message.insert(0, "30"); //type =sequence
	data.insert(0, message);

	data = hexaString(data);
	return(data);
}

/*
 * Funkcia na vytvaranie socketu, odosielanie a prijmanie dat a nasledny vypis na STDIN
 * communityString: heslo k SNMP agentovi
 * agent: adresa SNMP
 * interval: v akom pozadujem data z SNMP
 * Vracia: 1 v pr9pade chyby, inak bezi stale dokola
 */
int komunikacia(string communityString, string agent, int interval)
{
	/**************** Praca so socketom (vytvaranie...) ******************/
	int socketik;
	struct sockaddr_in mojaAdresa;
	struct sockaddr_in cielovaAdresa;
	struct hostent *hptr;
	socklen_t velkostSocketu = sizeof(cielovaAdresa);

	if ((socketik = socket(AF_INET, SOCK_DGRAM, NOFLAG)) < 0) //vytvaranie socketu
	{
		cerr << "Chyba vytvarania socketu!" << endl;
		return(CHYBA);
	}

	//Zdrojova adresa
	memset((char *) &mojaAdresa, NOFLAG, sizeof(mojaAdresa));
	mojaAdresa.sin_family = AF_INET;
	mojaAdresa.sin_addr.s_addr = htonl(INADDR_ANY);
	mojaAdresa.sin_port = htons(0);

	if (bind(socketik, (struct sockaddr *) &mojaAdresa, sizeof(mojaAdresa)) < 0) //bindovanie socketu
	{
		cerr << "Chyba bindovania!" << endl;
		return(CHYBA);
	}

	//Cielova adresa
	memset((char *) &cielovaAdresa, NOFLAG, sizeof(cielovaAdresa));
	cielovaAdresa.sin_family = AF_INET;
	cielovaAdresa.sin_port = htons(SNMP_PORT);
	if((hptr = gethostbyname(agent.c_str())) == NULL)
	{
		cerr << "Chyba prekladu adresy!" << endl;
		return CHYBA;
	}
	memcpy(&cielovaAdresa.sin_addr, hptr->h_addr, hptr->h_length);

	//Timeout nastavenie
	struct timeval timeout;
	timeout.tv_sec = 1 + ((interval * 2) / MILISEKUNDA);
	//timeout.tv_usec = 1000 + (interval * 2);
	setsockopt(socketik, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	setsockopt(socketik, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

	/**************** Odosielanie, prijmanie a vypis dat **************/
	while(1)
	{
		string oid = "2b06010201020100"; //oid na objekt v ktorom je pocet rozhrani
		int pocetRozhrani = 1;

		//Pocitam cas pociatku
		struct timeval start , koniec;
		double cakanie = 0;
		gettimeofday(&start , NULL);

		vypisCas();

		//Odosielanie a ziskavanie oid
		for(int i = 0; i <= pocetRozhrani; i++) //prechadzam vsetky rozhrania
		{
			for(int j = 1; j <= POCETOID; j++) //prechadzam vsetky objekty
			{
				if(i != ZISTENIEROZHRANI) //ak je 0 cyklus zistujem najprv iba pocet rozhrani, teda negnerujem nove oid
				{
					oid = "2b06010201020201";
					oid.append(intNaHex(j));
					oid.append(intNaHex(i));
				}

				string poziadavka = urobGetRequest(communityString, oid);
				if(sendto(socketik, poziadavka.c_str(), poziadavka.length(), NOFLAG, (struct sockaddr *) &cielovaAdresa, velkostSocketu) <= 0) //kontrola timeoutu
				{
					cerr << "Timeout vyprsal cas pre odosielanie dat na agenta!" << endl;
					return CHYBA;
				}
				char buf[MAXSOCKETBUF];
				string sprava = "";
				long int pocet = recvfrom(socketik, buf, MAXSOCKETBUF, NOFLAG, (struct sockaddr *) &cielovaAdresa, &velkostSocketu);
				if(pocet <= 0) //kontrola timeoutu
				{
					cerr << "Timeout vyprsal cas pre ziskavanie dat z Agenta!" << endl;
					return CHYBA;
				}
				for (int i = 0; i < pocet; i++) //musim porejst rucne, lebo mi v buf nevypisuje inak spravu, iba po newline
				{
					sprava += buf[i];
				}

				sprava = dekodujGetResponse(stringNaHex(sprava), communityString, oid);

				if(i == ZISTENIEROZHRANI) //prvy cyklus, teda chcem ziskat iba pocet rozhrani, nevypisujem nic
				{
					pocetRozhrani = stoi(ziskajValue(sprava, i));
					break;
				}

				cout << ";" << ziskajValue(sprava, j);
				//cout << "OID:" << j  << "   " << ziskajValue(sprava, j) << endl;
			}
		}
		cout << endl;

		gettimeofday(&koniec , NULL);
		cakanie = ((double)koniec.tv_sec*1000 + (double)koniec.tv_usec/1000) - ((double)start.tv_sec*1000 + (double)start.tv_usec/1000);
		if(cakanie > interval)
		{
			cerr << "Zadany casovy interval prekroceny cca o " << cakanie - interval << " milisekund!" << endl;
		}
		else
		{
			usleep(MILISEKUNDA * (interval - cakanie)); //uspim na dobu intervalu
		}
	}
}

int main(int argc, char *argv[])
{
	signal(SIGINT, sigIntFunkcia);
	int interval = 100;
	string communityString = "public";
	string agent = "127.0.0.1";
	if(argc == 4) //ak su zadane 3 parametre tak ocakavam parameter c
	{
		if(strcmp(argv[1], "-c") != 0 && strcmp(argv[1], "--community_string") != 0) // c arameter neni zadnany, teda chyba
		{
			cerr << "Parametry neboli zadane spravne!" << endl;
			return(CHYBA);
		}
		communityString = argv[2];
		agent = argv[3];
	}
	else if(argc == 6) //ak je zadanych 5 parametrov, ocakavam c aj i parameter
	{
		if(strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--community_string") == 0) //ak je zadany -c na prvom mieste
		{
			if(strcmp(argv[4], "-i") != 0 && strcmp(argv[4], "--interval") != 0) //ale neni i na druhom
			{
				cerr << "Parametry neboli zadane spravne!" << endl;
				return(CHYBA);
			}
			communityString = argv[2];
			agent = argv[3];
			interval = atoi(argv[5]);
		}
		else if(strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--interval") == 0) //ak je zadany -i na prvom mieste
		{
			if(strcmp(argv[3], "-c") != 0 && strcmp(argv[3], "--community_string") != 0) //ale neni c na druhom
			{
				cerr << "Parametry neboli zadane spravne!" << endl;
				return(CHYBA);
			}
			communityString = argv[4];
			agent = argv[5];
			interval = atoi(argv[2]);
		}
		else //neni tam nic zname na prvom mieste, chyba
		{
			cerr << "Parametry neboli zadane spravne!" << endl;
			return(CHYBA);
		}
	}
	else //viacej alebo menej poarametrov je chyba
	{
		cerr << "Parametry neboli zadane spravne!" << endl;
		return(CHYBA);
	}


	if(komunikacia(communityString, agent, interval) != OK)
	{
		return(CHYBA);
	}
}