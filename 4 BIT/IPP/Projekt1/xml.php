<?php
#Filip Gulan
#XTD:xgulan00
	$databaza = array(); //globalna premenna uchovavajuca databazove dotazy

	/*
	 *Funkcia na ziskanie datoveho typu
	 *Argumenty: retazec ktoreho datovy typ chcem zistit , a hodnota ci sa retazec nachadza v elemente alebo nie
	 *Vracia: datovy typ ako string
	 */
	function datovyTyp($retazec, $element)
	{
		$retazec = strval($retazec);
		$retazec = str_replace(' ', '', $retazec);
		if($retazec == "0" or $retazec == "1" or $retazec == "") //ak je tam prazdny retazec
		{
			return "BIT"; //tak je to boolean
		}
		if(is_numeric($retazec)) //ak sa tam anchadzaju iba cisla
		{
			if(strrpos($retazec, ".") or strrpos($retazec, "E") or strrpos($retazec, "e") or strrpos($retazec, "f") or strrpos($retazec, "F")) //ak je tam bodka alebo je to zadane ako E..
			{
				return "FLOAT"; //tak je to float
			}
			else
			{
				return "INT"; //inak to je integer
			}
		}
		else //ak su tam aj ine znaky ako len cisla
		{
			$retazec = strtoupper($retazec);
			if($retazec == "TRUE" or $retazec == "FALSE") //ak je tam iba false alebo true
			{
				return "BIT"; //tak je to boolean
			}
			else //inak to je cisty text
			{
				if($element === FALSE) //ak je to v elemente
				{
					return "NVARCHAR";
				}
				else //mimo elementu
				{
					return "NTEXT";
				}
			}
		}
	}

	/*
	 *Funkcia kontroluje, ci doslo ku kolizii stlpcov atributu a cudzieho klucu
	 *Argumenty: $polozka, ktoru kontroluejm a tabulka v ktorej kontrolujem
	 */
	function skontrolujKolizie($polozka, $tabulka)
	{
		global $STDERR;
		if(preg_match("/_id .*,<atr>$/", $polozka))
		{
			$zhoda1 = preg_replace("/_id .*,<atr>/", "_id INT,", $polozka);
			$zhoda2 = preg_replace("/_id .*,<atr>/", "1_id INT,", $polozka);
			if(in_array($zhoda1, $tabulka) or in_array($zhoda2, $tabulka)) //nachadza sa v poli kolizia?
			{
				fwrite($STDERR, "Doslo ku kolizii cudzieho kluca a atributu alebo value!\n");
				fclose($STDERR);
				exit(90);
			}
		}
		elseif(preg_match("/1_id .*,<atr>$/", $polozka))
		{
			$zhoda1 = preg_replace("/1_id .*,<atr>/", "1_id INT,", $polozka);
			$zhoda2 = preg_replace("/1_id .*,<atr>/", "_id INT,", $polozka);
			if(in_array($zhoda1, $tabulka) or in_array($zhoda2, $tabulka)) //nachadza sa v poli kolizia?
			{
				fwrite($STDERR, "Doslo ku kolizii cudzieho kluca a atributu alebo value!\n");
				fclose($STDERR);
				exit(90);
			}
		}
	}

	/*
	 *Funkcia na spocitanie jendotlivych podelementov
	 *Argumenty: element/prvok,  ktoreho deti spocitujeme
	 *Vracia: pole poctov vyskytov
	 */
	function spocitajElementy($prvok)
	{
		$pocetPodelementov = array();
		foreach($prvok->children() as $podelement) //zistim pocty jednotlivych podelementov a naplnim pole kde index je meno podelementu a polozka je pocet elementov
		{
			$podelementLower = strtolower($podelement->getName());
			if(array_key_exists($podelementLower, $pocetPodelementov))
			{
				$pocetPodelementov[$podelementLower] += 1; //nasiel sa dalsi
			}
			else
			{
				$pocetPodelementov[$podelementLower] = 1; //nasiel sa prvy
			}
		}	
		foreach($pocetPodelementov as &$pocetElement) //nastavim polozkam, ktore sa v poli vyskituju iba raz hodnotu 0 aby sa negenerovalo cislo
		{
			if($pocetElement == 1)
			{
				$pocetElement = 0;
			}
		}
		return $pocetPodelementov;
	}

	/*
	 *Funkcia na spocitanie duplikatov
	 *Argumenty: prvok, ktoreho duplikaty spocitavam, pocet podelementov
	 */
	function spocitajDuplikaty($prvok, $pocetPodelementov)
	{
		foreach($prvok->children() as $podelement) //zistim pocty jednotlivych podelementov a naplnim pole kde index je meno podelementu a polozka je pocet elementov
		{
			$podelementLower = strtolower($podelement->getName());
			if(array_key_exists($podelementLower, $pocetPodelementov))
			{
				$pocetPodelementov[$podelementLower] += 1; //nasiel sa dalsi
			}
			else
			{
				$pocetPodelementov[$podelementLower] = 1; //prve este tam neni
			}
		}
		foreach($pocetPodelementov as &$pocetElement) //nastavim polozkam, ktore sa v poli vyskituju iba raz hodnotu 0 aby sa negenerovalo cislo
		{
			if($pocetElement == 1)
			{
				$pocetElement = 0;
			}
		}
	}

	/*
	 *Funkcia na odstranenie duplikatu z uz generovanej tabulky
	 *Argumenty: podelement ktory sa uz v tabulke nachadza, taabulka v ktorej sa nachadza
	 *Vracia: upravenu tabulku
	 */
	function odstranDuplikat($podelement, $menoTabulky)
	{
		$podelementLower = strtolower($podelement->getName());
		if($indexDuplikatu = array_search($podelementLower . "_id INT,", $menoTabulky)) //ak tam je iba jeden odstranim iba jeho
		{
			array_splice($menoTabulky, $indexDuplikatu,1);	
		}
		if($indexDuplikatu = array_search($podelementLower . "1_id INT,", $menoTabulky)) //je tam viacero odstranujem vsektych postupne
		{
			for($i = 1; ;$i++) //odstranujem ostupne vsektych
			{
				if($indexDuplikatu = array_search($podelementLower . $i . "_id INT,", $menoTabulky)) //je tam viacero odstranujem vsektych postupne
				{
					array_splice($menoTabulky, $indexDuplikatu,1);
				}
				else
				{
					return $menoTabulky;
				}
			}
		}
		return $menoTabulky;
	}

	/*
	 *Funkcia na generovanie obsahu elementu, zaistuje prioritu datoveho typu
	 *Argumenty: obsah elementu, tabulku ktoru uopravujem
	 *Vracia: upravenu tabulku
	 */
	function generatorObsahuEl($prvok, $menoTabulky)
	{
		if(trim($prvok) != "") //ak obsah elementu existuje
		{
			$obsahElementu = preg_replace('/\s+/', '',$prvok) . "\n";
			$obsahElementu  = trim(preg_replace('/\s\s+/', '', $obsahElementu)); //odstranim biele znaky
			if($indexDuplikatu = array_search("value BIT,<atr>", $menoTabulky)) //ak existuje uz prvok zistujem prioritu dat typu a nastavujem novu
			{
				if(datovyTyp($obsahElementu, $element = TRUE) == "INT" or datovyTyp($obsahElementu, $element = TRUE) == "FLOAT" or datovyTyp($obsahElementu, $element = TRUE) == "NVARCHAR" or datovyTyp($obsahElementu, $element = TRUE) == "NTEXT")
				{
					$menoTabulky[$indexDuplikatu] = "value " . datovyTyp($obsahElementu, $element = TRUE) . ",<atr>";
				}
			}
			elseif($indexDuplikatu = array_search("value INT,<atr>", $menoTabulky))
			{
				if(datovyTyp($obsahElementu, $element = TRUE) == "FLOAT" or datovyTyp($obsahElementu, $element = TRUE) == "NVARCHAR" or datovyTyp($obsahElementu, $element = TRUE) == "NTEXT")
				{
					$menoTabulky[$indexDuplikatu] = "value " . datovyTyp($obsahElementu, $element = TRUE) . ",<atr>";
				}
			}
			elseif($indexDuplikatu = array_search("value FLOAT,<atr>", $menoTabulky))
			{
				if(datovyTyp($obsahElementu, $element = TRUE) == "NVARCHAR" or datovyTyp($obsahElementu, $element = TRUE) == "NTEXT")
				{
					$menoTabulky[$indexDuplikatu] = "value " . datovyTyp($obsahElementu, $element = TRUE) . ",<atr>";
				}
			}
			elseif($indexDuplikatu = array_search("value NVARCHAR,<atr>", $menoTabulky))
			{
				if(datovyTyp($obsahElementu, $element = TRUE) == "NTEXT")
				{
					$menoTabulky[$indexDuplikatu] = "value " . datovyTyp($obsahElementu, $element = TRUE) . ",<atr>";
				}
			}
			elseif($indexDuplikatu = array_search("value NTEXT,<atr>", $menoTabulky))
			{
				//uz tam je
			}
			else //este sa tam nenachadza s rovnakym menom tak prvok pushujem 
			{
				array_push($menoTabulky, "value " . datovyTyp($obsahElementu, $element = TRUE) . ",<atr>");
			}
		}
		return $menoTabulky;
	}

	/*
	 *Funkcia na generovanie atributu, zaistuje prioritu datoveho typu
	 *Argumenty: obsah elementu, tabulka ktoru upravujeme
	 *Vracia: upravenu tabulku
	 */
	function generatorAtributu($prvok, $menoTabulky)
	{
		foreach($prvok->attributes() as $mojAtribut) //ziskame vsetky atributy v elemente
		{
			if($indexDuplikatu = array_search(strtolower($mojAtribut->getName()) . " BIT,<atr>", $menoTabulky)) //prepisovanie stavajucich atributov podla pravidla BIT < INT < FLOAT < NVARCHAR < NTEXT
			{
				if(datovyTyp($mojAtribut, $element = FALSE) == "INT" or datovyTyp($mojAtribut, $element = FALSE) == "FLOAT" or datovyTyp($mojAtribut, $element = FALSE) == "NVARCHAR" or datovyTyp($mojAtribut, $element = FALSE) == "NTEXT")
				{
					$menoTabulky[$indexDuplikatu] = strtolower($mojAtribut->getName()) . " " . datovyTyp($mojAtribut, $element = FALSE) . ",<atr>";
				}
			}
			elseif($indexDuplikatu = array_search(strtolower($mojAtribut->getName()) . " INT,<atr>", $menoTabulky))
			{
				if(datovyTyp($mojAtribut, $element = FALSE) == "FLOAT" or datovyTyp($mojAtribut, $element = FALSE) == "NVARCHAR" or datovyTyp($mojAtribut, $element = FALSE) == "NTEXT")
				{
					$menoTabulky[$indexDuplikatu] = strtolower($mojAtribut->getName()) . " " . datovyTyp($mojAtribut, $element = FALSE) . ",<atr>";
				}
			}
			elseif($indexDuplikatu = array_search(strtolower($mojAtribut->getName()) . " FLOAT,<atr>", $menoTabulky))
			{
				if(datovyTyp($mojAtribut, $element = FALSE) == "NVARCHAR" or datovyTyp($mojAtribut, $element = FALSE) == "NTEXT")
				{
					$menoTabulky[$indexDuplikatu] = strtolower($mojAtribut->getName()) . " " . datovyTyp($mojAtribut, $element = FALSE) . ",<atr>";
				}
			}
			elseif($indexDuplikatu = array_search(strtolower($mojAtribut->getName()) . " NVARCHAR,<atr>", $menoTabulky))
			{
				if(datovyTyp($mojAtribut, $element = FALSE) == "NTEXT")
				{
					$menoTabulky[$indexDuplikatu] = strtolower($mojAtribut->getName()) . " " . datovyTyp($mojAtribut, $element = FALSE) . ",<atr>";
				}
			}
			elseif($indexDuplikatu = array_search(strtolower($mojAtribut->getName()) . " NTEXT,<atr>", $menoTabulky))
			{
				//nerobim nic uz tam je 
			}
			else //vkladam novy atribut do tabulky
			{
				array_push($menoTabulky, strtolower($mojAtribut->getName()) . " " . datovyTyp($mojAtribut, $element = FALSE) . ",<atr>");
			}
		}
		return $menoTabulky;
	}

	/*
	 *Funkcia na generovanie elementu, spracovany prepinac -etc a -b
	 *Argumenty: element/prvok a tabulka ktoru upravujem
	 *Vracia: upravenu tabulku
	 */
	function generatorPodelementu($prvok, $menoTabulky)
	{
		global $databaza;
		global $argumenty;
		if(!(isset($argumenty["b"]))) //ak je zadany b tak nemusim ocitat pocet vyskytov elementu, inak ich spocitam
		{
			$pocetPodelementov = array();
			$pocetPodelementov = spocitajElementy($prvok);
		}
		foreach($prvok->children() as $podelement)
		{
			$podelementLower = strtolower($podelement->getName());
			$prvokLower = strtolower($prvok->getName());
			if(isset($argumenty["etc"]) and array_key_exists($podelementLower, $databaza)) //ak tabulka podelementu uz existuje, tak budem kontrolovat ci neobsahuje cudzi kluc
			{
				if(isset($argumenty["b"]))
				{
					if(!(array_search($podelementLower . "_id INT,", $menoTabulky))) //ak uz v tabulke je nevkladam znovu prepinac -b
					{
						array_push($menoTabulky, $podelementLower . "_id INT,"); //v tabulke neni tak pushujem
					}
				}
				elseif(!(array_search($prvokLower . "_id INT,", $databaza[$podelementLower]))) //ak neexistuje cudzi kluc
				{
					if(isset($argumenty["etc"]) and $pocetPodelementov[$podelementLower] > $argumenty["etc"]) //ak pocet elementov prekrocil prepinac etc
					{
						$menoTabulkyPomocnej = $podelementLower;
						$$menoTabulkyPomocnej = $databaza[$menoTabulkyPomocnej];
						if(!(array_search($prvokLower . "_id INT,", $$menoTabulkyPomocnej))) //ak tam este neni
						{
							array_push($$menoTabulkyPomocnej, $prvokLower . "_id INT,");
							$databaza[$menoTabulkyPomocnej] = $$menoTabulkyPomocnej;	
							$menoTabulky = odstranDuplikat($podelement, $menoTabulky);
						}
					}
					elseif(isset($argumenty["etc"]) and $argumenty["etc"] == 0) //ak je prepinac etc rovny nuletak hned vymienam kluce
					{
						$menoTabulkyPomocnej = $podelementLower;
						$$menoTabulkyPomocnej = $databaza[$menoTabulkyPomocnej];
						if(!(array_search($prvokLower . "_id INT,", $$menoTabulkyPomocnej))) //ak tam este neni
						{
							array_push($$menoTabulkyPomocnej, $prvokLower . "_id INT,");
							$databaza[$menoTabulkyPomocnej] = $$menoTabulkyPomocnej;	
							$menoTabulky = odstranDuplikat($podelement, $menoTabulky);
						}
					}
					////////////////////////////////////////START nomralneho vypisovania
					else
					{
						if($pocetPodelementov[$podelementLower] == 0) //ak sa podelement nachadz iba raz v elemente
						{
							if(!(array_search($podelementLower . $pocetPodelementov[$podelementLower] . "_id INT,", $menoTabulky))) //ak tam este neni
							{
								if(!(array_search($podelementLower . "1_id INT,", $menoTabulky))) //ak tam este neni
								{
									if(!(array_search($podelementLower . "_id INT,", $menoTabulky)))
									{
										array_push($menoTabulky, $podelementLower . "_id INT,");
									}
								}
							}
						}
						else //ak sa nachadza podlement v elemente viacejkrat
						{
							if(($indexDuplikatu = array_search($podelementLower . "_id INT,", $menoTabulky))) //ak tam este neni
							{
								array_splice($menoTabulky, $indexDuplikatu, 1);
							}
							if(!(array_search($podelementLower . $pocetPodelementov[$podelementLower] . "_id INT,", $menoTabulky))) //ak tam este neni
							{
								array_push($menoTabulky, $podelementLower . $pocetPodelementov[$podelementLower] . "_id INT,");
								$pocetPodelementov[$podelementLower] -= 1;
							}
						}
					}
					///////////////////////////////////////END
				}
			}
			else //este neni vymeneny kluc, alebo je to bez prpeinaca --etc
			{
				if(isset($argumenty["etc"]) and $pocetPodelementov[$podelementLower] > $argumenty["etc"]) //tabulka este neexistuje takze ju musim vytvorit ked chcem vymenit kluce
				{
					$menoTabulkyPomocnej = $podelementLower;
					$$menoTabulkyPomocnej = array("CREATE TABLE " . $podelementLower . "(");
					array_push($$menoTabulkyPomocnej, "prk_" . $podelementLower . "_id INT PRIMARY KEY,");
					array_push($$menoTabulkyPomocnej, $prvokLower . "_id INT,");
					$databaza[$menoTabulkyPomocnej] = $$menoTabulkyPomocnej;
					$menoTabulky = odstranDuplikat($podelement, $menoTabulky);	
				}
				elseif(isset($argumenty["etc"]) and $argumenty["etc"] == 0) //to iste ako blok nad ale pre etc 0
				{
					$menoTabulkyPomocnej = $podelementLower;
					$$menoTabulkyPomocnej = array("CREATE TABLE " . $podelementLower . "(");
					array_push($$menoTabulkyPomocnej, "prk_" . $podelementLower . "_id INT PRIMARY KEY,");
					array_push($$menoTabulkyPomocnej, $prvokLower . "_id INT,");
					$databaza[$menoTabulkyPomocnej] = $$menoTabulkyPomocnej;
					$menoTabulky = odstranDuplikat($podelement, $menoTabulky);
				}
				elseif(isset($argumenty["b"])) //je zadany rpepinac b kontolujem ci tam uz prvok neni a ak je nevkladam
				{
					if(!(array_search($podelementLower . "_id INT,", $menoTabulky)))
					{
						array_push($menoTabulky, $podelementLower . "_id INT,");
					}
				}
				//////////////////////////////////////////START normalneho vypisovania
				else
				{
					if($pocetPodelementov[$podelementLower] == 0) //ak sa podelement nachadz iba raz v elemente
					{
						if(!(array_search($podelementLower . $pocetPodelementov[$podelementLower] . "_id INT,", $menoTabulky))) //ak tam este neni
						{
							if(!(array_search($podelementLower . "1_id INT,", $menoTabulky))) //ak tam este neni
							{
								if(!(array_search($podelementLower . "_id INT,", $menoTabulky)))
								{
									array_push($menoTabulky, $podelementLower . "_id INT,");
								}
							}
						}
					}
					else //ak sa nachadza podlement v elemente viacejkrat
					{
						if(($indexDuplikatu = array_search($podelementLower . "_id INT,", $menoTabulky))) //ak tam este neni
						{
							array_splice($menoTabulky, $indexDuplikatu, 1);
						}
						if(!(array_search($podelementLower . $pocetPodelementov[$podelementLower] . "_id INT,", $menoTabulky))) //ak tam este neni
						{
							array_push($menoTabulky, $podelementLower . $pocetPodelementov[$podelementLower] . "_id INT,");
							$pocetPodelementov[$podelementLower] -= 1;
						}
					}
				}
				//////////////////////////////////////////////END
			}
		}
		return $menoTabulky;
	}

	/*
	 *Funkcia ktora rekurzivne prechadza objekty vygenerovane Simplexml kniznicou spracovanie -a prepinaca
	 *Argumenty: element
	 */
	function parser($xml)
	{
		global $databaza;
		global $argumenty;
		foreach($xml->children() as $prvok) //ziskame vsetky elementy
		{			
			$menoTabulky = strtolower($prvok->getName()); //vytvorim si navo premenej
			if(array_key_exists($menoTabulky, $databaza)) //ak uz tabulka existuje, iba ju nakopirujem do premennej s premenm naxovm
			{
				$$menoTabulky = $databaza[$menoTabulky];
			}
			else //inak tabulku aj vytvorim
			{
				$$menoTabulky = array("CREATE TABLE " . strtolower($prvok->getName()) . "(");
				array_push($$menoTabulky, "prk_" . strtolower($prvok->getName()) . "_id INT PRIMARY KEY,");
			}

			$$menoTabulky = generatorPodelementu($prvok, $$menoTabulky);
			$$menoTabulky = generatorObsahuEl($prvok, $$menoTabulky);
			if(!(isset($argumenty["a"]))) //ak nieje zadnay prepinac tak nevypisujem atributy
			{
				$$menoTabulky = generatorAtributu($prvok, $$menoTabulky);
			}
			$databaza[$menoTabulky] = $$menoTabulky; //aktualizujem databazu tabulkou

			if($prvok->count() !== 0) //ak su tam childy, tak idem rekurziou spracovavatch ich
			{
				parser($prvok);
			}
		}
	}

	/*
	 *Funkcia na finalne upravenie tabulky, ako je odstranenie ciarky na predposlednej polozke, kontrola kolizii, odriadkovanie
	 *Argumenty: vstupne xml
	 */
	function xmlDoDdl($xml)
	{
		global $databaza;
		global $STDERR;
		parser($xml);
		foreach($databaza as $index => $tabulka) //prechadzam pole poli
		{
			foreach ($tabulka as $indexik => $polozka) //prechadzam male polia
			{
				skontrolujKolizie($polozka, $tabulka);
				if(preg_match("/,<atr>$/", $polozka)) //odstranime identifikatory atributov
				{
				 	$tabulka[$indexik] = preg_replace("/,<atr>/", ",", $polozka);
				}

			}
			$tabulka[sizeof($tabulka) - 1] = preg_replace("/,$/", "", $tabulka[sizeof($tabulka) - 1]); //predposlenej polozke vymazem ciarku
			$tabulka[sizeof($tabulka)] = ");\n"; //doplnim ukoncenie tabulky
			$databaza[$index] = $tabulka;
		}
	}
?>