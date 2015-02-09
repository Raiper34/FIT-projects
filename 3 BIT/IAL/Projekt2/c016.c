
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Polo¾kami
**                      První implementace: Petr Pøikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masaøík, øíjen 2014
**                      Úpravy: Radek Hranický, øíjen 2014
**
** Vytvoøete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Polo¾kami = Hash table)
** s explicitnì øetìzenými synonymy. Tabulka je implementována polem
** lineárních seznamù synonym.
**
** Implementujte následující procedury a funkce.
**
**  xxxHTInit ....... inicializuje tabulku pøed prvním pou¾itím
**  HTInsert ..... vlo¾ení prvku
**  HTSearch ..... zji¹tìní pøítomnosti prvku v tabulce
**  HTDelete ..... zru¹ení prvku
**  HTRead ....... pøeètení hodnoty prvku
**  HTClearAll ... zru¹ení obsahu celé tabulky (inicializace tabulky
**                 poté, co ji¾ byla pou¾ita)
**
** Definici typù naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelù na polo¾ky, je¾ obsahují slo¾ky
** klíèe 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na dal¹í synonymum 'ptrnext'. Pøi implementaci funkcí
** uva¾ujte maximální rozmìr pole HTSIZE.
**
** U v¹ech procedur vyu¾ívejte rozptylovou funkci hashCode.  Pov¹imnìte si
** zpùsobu pøedávání parametrù a zamyslete se nad tím, zda je mo¾né parametry
** pøedávat jiným zpùsobem (hodnotou/odkazem) a v pøípadì, ¾e jsou obì
** mo¾nosti funkènì pøípustné, jaké jsou výhody èi nevýhody toho èi onoho
** zpùsobu.
**
** V pøíkladech jsou pou¾ity polo¾ky, kde klíèem je øetìzec, ke kterému
** je pøidán obsah - reálné èíslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíè a pøidìlit
** mu index v rozmezí 0..HTSize-1.  V ideálním pøípadì by mìlo dojít
** k rovnomìrnému rozptýlení tìchto klíèù po celé tabulce.  V rámci
** pokusù se mù¾ete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitnì zøetìzenými synonymy.  Tato procedura
** se volá pouze pøed prvním pou¾itím tabulky.
*/

void htInit ( tHTable* ptrht ) {	
	for(int i = 0; i < HTSIZE; i++) //vsetky prvky tabullky budu ukazovat na NULL
	{
		(*ptrht)[i] = NULL;
	}
 //solved = 0; /*v pripade reseni, smazte tento radek!*/
}

/* TRP s explicitnì zøetìzenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíèe key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	if(ptrht != NULL) //ak tabulka existuje
	{
		int index = hashCode(key); //najdeme index, pomocou hashovacej funkcie 
		tHTItem *pomocna = (*ptrht)[index]; //a vytvorime pomocnu premennu s indexom
		while(pomocna != NULL)
		{
			if(!strcmp(pomocna->key, key)) //ak sa kluc zhoduje s miestom v tabulke tak vrati ze naslo a kde
			{
				return pomocna;
			}
			pomocna = pomocna->ptrnext; //posune sa v zozname doprava na dalsie synonimum
		}
		return NULL; //nenaslo
	}
	return NULL; //neobsahuje
}

/* 
** TRP s explicitnì zøetìzenými synonymy.
** Tato procedura vkládá do tabulky ptrht polo¾ku s klíèem key a s daty
** data.  Proto¾e jde o vyhledávací tabulku, nemù¾e být prvek se stejným
** klíèem ulo¾en v tabulce více ne¾ jedenkrát.  Pokud se vkládá prvek,
** jeho¾ klíè se ji¾ v tabulce nachází, aktualizujte jeho datovou èást.
**
** Vyu¾ijte døíve vytvoøenou funkci htSearch.  Pøi vkládání nového
** prvku do seznamu synonym pou¾ijte co nejefektivnìj¹í zpùsob,
** tedy proveïte.vlo¾ení prvku na zaèátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
	if(ptrht != NULL) //ak tabulka existuje
	{
		tHTItem *pomocna = htSearch(ptrht, key); //zistime ci neexistuje v tabulke polozka s totoznym klucom
		if(pomocna != NULL)
		{
			pomocna->data = data; //ak sa nachadza, zaktualizujeme iba data
		}
		else //neexistuje polozka s rovnakym klucom
		{
			pomocna = malloc(sizeof(tHTItem)); //naalokujeme miesto pre polozku
			if(pomocna == NULL) //alokacia sa nepodarila //asi zmazem este...
			{
				return;
			}
			else //podarila sa alokacia
			{
				int dlzka = strlen(key); //zistime si dlzku pre kluc
				int index = hashCode(key); //zistime si index kde budeme ukladat
				pomocna->key = malloc(dlzka + 1); //naalokujeme miesto pre kluc
				if(pomocna->key == NULL) //alokacia sa nepodarila //asi zmazem este...
				{
					free(pomocna);
				}
				else //podarila sa alokacia
				{
					strcpy(pomocna->key, key); //nakopirujeme kluc do novej polozky
					pomocna->data = data; //nakopirujeme data
					pomocna->ptrnext = (*ptrht)[index]; //previazeme
					(*ptrht)[index] = pomocna; //dame na prve miesto
				}
			}
		}
	}
}

/*
** TRP s explicitnì zøetìzenými synonymy.
** Tato funkce zji¹»uje hodnotu datové èásti polo¾ky zadané klíèem.
** Pokud je polo¾ka nalezena, vrací funkce ukazatel na polo¾ku
** Pokud polo¾ka nalezena nebyla, vrací se funkèní hodnota NULL
**
** Vyu¾ijte døíve vytvoøenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
	if(ptrht != NULL) //ak existuje
	{
		tHTItem *pomocna = htSearch(ptrht, key); //vyhladame polozku
		if(pomocna != NULL) //ak sa polozka nachadza v tabulke 
		{
			return &(pomocna->data); //vratime ju
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}

/*
** TRP s explicitnì zøetìzenými synonymy.
** Tato procedura vyjme polo¾ku s klíèem key z tabulky
** ptrht.  Uvolnìnou polo¾ku korektnì zru¹te.  Pokud polo¾ka s uvedeným
** klíèem neexistuje, dìlejte, jako kdyby se nic nestalo (tj. nedìlejte
** nic).
**
** V tomto pøípadì NEVYU®ÍVEJTE døíve vytvoøenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
	if(ptrht != NULL) //ak existuje tabulka
	{
		int index = hashCode(key); //zistime si index, kde sa nachadza polozka, ktoru chceme zrusit
		tHTItem *pomocna = (*ptrht)[index];
		tHTItem *pomocna2 = (*ptrht)[index];
		if(pomocna == NULL) //ak je index tabulky prazdny
		{
			return;
		}
		if(!strcmp(pomocna->key, key)) //nasli sme polozku na prom mieste v tabulke
			{
				(*ptrht)[index] = pomocna->ptrnext; //dalsia bude prva 
				free(pomocna->key);
				free(pomocna);
				return;
			}
		while(pomocna->ptrnext != NULL)	//ak nebola na prvom mieste musime prechadzat vsetky synonima
		{
			pomocna2 = pomocna; //aby sme nestratili ukazatel
			pomocna = pomocna->ptrnext; //dalsi prvok
			if(!strcmp(pomocna->key, key)) //uz me ju nasli
			{
				pomocna2->ptrnext = pomocna->ptrnext; //naviazeme na ob polozku
				free(pomocna->key);
				free(pomocna);
				return;
			}
			pomocna2 = pomocna; 	
		}
	}
}

/* TRP s explicitnì zøetìzenými synonymy.
** Tato procedura zru¹í v¹echny polo¾ky tabulky, korektnì uvolní prostor,
** který tyto polo¾ky zabíraly, a uvede tabulku do poèáteèního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
	if(ptrht != NULL) //ak existuje tabulka
	{
		tHTItem *pomocna;
		tHTItem *pomocna2; //pomocna aby sme nestratili ukazatel po uvolneni
		for(int i = 0; i < HTSIZE; i++) //budeme prechadzat v tabulke
		{
			pomocna = (*ptrht)[i];
			while(pomocna != NULL) //a tiez budeme prechadzat vsetky synonyma
			{
				pomocna2 = pomocna->ptrnext; //posuneme sa 
				free(pomocna->key);
				free(pomocna);
				pomocna = pomocna2;
			}
			(*ptrht)[i] = NULL; //nakoniec uvolnime polozku pola tabulky
		}
	}
}
