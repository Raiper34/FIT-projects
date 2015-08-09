<?php
#Filip Gulan
#XTD:xgulan00
	$argumenty = array();

	/*
	 *Funkcia navypisanie napovedy help
	 */
	function napoveda()
	{
		echo
'Definice parametru skriptu:
• --help vitiskne napovedu
• --input=filename zadany vstupni soubor ve formatu XML
• --output=filename zadany vystupni soubor ve formatu definovanem vyse	
• --header=’hlavicka’ na zacatek vystupniho souboru se vlozi zakomentovana hlavicka
• --etc=n pro n ≥ 0 urcuje maximalni pocet sloupcu vzniklych ze stejnojmennych podelementu
• -a nebudou se generovat sloupce z atributu ve vstupnim XML souboru
• -b pokud bude element obsahovat vice podelementu stejneho nazvu, bude se uvazovat, jakoby zde byl pouze jediny takovy (tento parametr nesmi byt kombinovan s parametrem --etc=n)
• -g lze jej uplatnit v kombinaci s jakymikoliv jinymi prepinaci vyjma --help. Pri jeho aktivaci bude vystupnim souborem pouze XML tvaru:
<?xml version="1.0" encoding="UTF-8"?>
<tables>
<table name="nazev_tabulky">
<relation to="nazev_cizi_tabulky" relation_type="vztah" />
...
</table>
...
</tables>
';
	}

	/*
	 *Funkcia na ziskanie a spracovanie argumentov
	 *Argumenty: argv - pole surovych argumentov, argc - pocet surovych argumentov
	 */
	function ziskajArgumenty($argv, $argc)
	{
		global $STDERR;
		global $argumenty;
		$kratkeArgumenty = "abg"; //vypis moznych kratkych argumentov
		$dlheArgumenty  = array( //vypis moznych dlhych argumentov
		    "output:",
		    "input:",
		    "header:",
		    "etc:",
		    "help",
		    "isvalid:",
		    "",
		);
		if(!($argumenty = getopt($kratkeArgumenty, $dlheArgumenty))) //ak funkcia vrati false, tak argumenty boli zadane nespravne
		{
			//echo "Argumenty neboli zdane spravne! \n";
			//exit(1);
		}

		if(isset($argumenty["help"])) //ak je zadany help
		{
			if($argc > 2) //otestujem lebo help moze byt zadany iba samotny
			{
				fwrite($STDERR, "Argumenty neboli zdane spravne! \n");
				fclose($STDERR);
				exit(1);
			}
			else //ak je zadany spravne vypisem napovedu
			{
				napoveda();
				fclose($STDERR);
				exit(0);
			}
		}

		if(sizeof($argumenty) + 1 <> $argc) //ak nesuhlasi pocet argumentov a pocet spracovanych argumentov getopts
		{
			fwrite($STDERR, "Argumenty neboli zdane spravne! \n");
			fclose($STDERR);
			exit(1);
		}

		if(isset($argumenty["a"]))
		{
			if(!(in_array("-a", $argv))) //osetrenie oddelenia aby sa nemohlo napriklad stat ze -anieco zoberie ako -a
			{
				fwrite($STDERR, "Argumenty neboli zdane spravne! \n");
				fclose($STDERR);
				exit(1);
			} 
		}
		if(isset($argumenty["b"]))
		{
			if(!(in_array("-b", $argv))) //to iste ako pre -a
			{
				fwrite($STDERR, "Argumenty neboli zdane spravne! \n");
				fclose($STDERR);
				exit(1);
			} 
		}
		if(isset($argumenty["g"]))
		{
			if(!(in_array("-g", $argv))) //tiez to iste ako pre -a
			{
				fwrite($STDERR, "Argumenty neboli zdane spravne! \n");
				fclose($STDERR);
				exit(1);
			} 
		}

		if(isset($argumenty["etc"])) //ak je zadany prepinac etc
		{
			if($argumenty["etc"] < 0) //ak je etc mensi ako 0
			{
				fwrite($STDERR, "Argumenty neboli zdane spravne! \n");
				fclose($STDERR);
				exit(1);
			}
			if(!(is_numeric($argumenty["etc"]))) //ak to neni cislo
			{
				fwrite($STDERR, "Argumenty neboli zdane spravne! \n");
				fclose($STDERR);
				exit(1);
			}
			else //ak to je cislo ale 
			{
				if(strrpos($argumenty["etc"], ".")) //ak je tam bodka
				{
					fwrite($STDERR, "Argumenty neboli zdane spravne! \n");
					fclose($STDERR);
					exit(1);
				}
			}
			if(isset($argumenty["b"])) //ak je zadany aj argument b, ktory sa nesmie s etc kombinovat
			{
				fwrite($STDERR, "Argumenty neboli zdane spravne! \n");
				fclose($STDERR);
				exit(1);
			}
		}
	}
?>