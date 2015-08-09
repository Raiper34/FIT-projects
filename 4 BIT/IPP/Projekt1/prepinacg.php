<?php
#Filip Gulan
#XTD:xgulan00

	$databazaG = array(); //uchovavam riadky tabulky
	$databazaNM = array();

	/*
	 *Funkcia na zapisanie vytvorenych tabuliek do suboru
	 */
	function zapisDoSuboruG()
	{
		global $STDERR;
		global $argumenty;
		global $databazaG;
		if((file_put_contents($argumenty["output"], '<?xml version="1.0" encoding="UTF-8"?>' . "\n<tables>\n", FILE_APPEND)) === FALSE)
		{
			fwrite($STDERR, "Do suboru sa nepodarilo zapisat!\n");
			fclose($STDERR);
			exit(3);
		}
		foreach($databazaG as $index => $tabulka)
		{
			foreach ($tabulka as $polozka)
			{	
				if((file_put_contents($argumenty["output"], $polozka . "\n", FILE_APPEND)) === FALSE)
				{
					fwrite($STDERR, "Do suboru sa nepodarilo zapisat!\n");
					fclose($STDERR);
					exit(3);
				}
			}
			if((file_put_contents($argumenty["output"], "	</table>\n", FILE_APPEND)) === FALSE)
			{
				fwrite($STDERR, "Do suboru sa nepodarilo zapisat!\n");
				fclose($STDERR);
				exit(3);
			}
		}
		if((file_put_contents($argumenty["output"], "</tables>\n", FILE_APPEND)) === FALSE)
		{
			fwrite($STDERR, "Do suboru sa nepodarilo zapisat!\n");
			fclose($STDERR);
			exit(3);
		}
	}

	/*
	 *Funkcia na zistenie ci ukazuu na seba navzojaom a teda ide o vztah NM
	 *Argumenty: menotabulky v ktorej som, a meno tabulky ktora sa nachadz v tabulke v ktorej som
	 *Vracia: true ak ukazuju na seba, inak false
	 */
	function jeToMN($menoTabulky, $prehladavam)
	{
		global $databazaG;
		global $databaza;
		foreach($databaza[$prehladavam] as $sqlPolozka) //prehladavam tabulky danu parametrem prehladavam
		{
			$polozka = preg_replace("/_id INT.*/", "", $sqlPolozka); //na zistenie mena cudzieho kluca
			if($menoTabulky == $polozka) //ak tam je, tak ukazuju na seba navzjom
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	/*
	 *Funkcia na generovanie vztahu pri prvom priechode zhora dole
	 *Argumenty: menotabulky do ktorej generujem vztah, polozka ktoru vkladam do tabulky, zanorenie ci mozem nagenerovat MN vztah
	 */
	function generujVztah($menoTabulky, $polozka, $zanorenie)
	{
		global $databazaG;
		if(!(in_array('		<relation to="' . $polozka . '" relation_type="1:1"/>', $databazaG[$menoTabulky])) and !(in_array('		<relation to="' . $polozka . '" relation_type="N:M"/>', $databazaG[$menoTabulky])))
		{
			if(!(in_array('		<relation to="' . $polozka . '" relation_type="N:1"/>', $databazaG[$menoTabulky])) and !(in_array('		<relation to="' . $polozka . '" relation_type="1:N"/>', $databazaG[$menoTabulky])))
			{
				if(jeToMN($menoTabulky, $polozka) and $zanorenie == 0) //ak ukazuju na seba navzjom tak generujem MN
				{
					array_push($databazaG[$menoTabulky], '		<relation to="' . $polozka . '" relation_type="N:M"/>');
				}
				elseif(($polozka == $menoTabulky) and $zanorenie != 0) //nte zanorenie teda vztah 1N
				{
					array_push($databazaG[$menoTabulky], '		<relation to="' . $polozka . '" relation_type="1:N"/>');
				}
				else //normalne generovanie N1
				{
					array_push($databazaG[$menoTabulky], '		<relation to="' . $polozka . '" relation_type="N:1"/>');
				}
			}
		}
	}

	/*
	 *Vztah podradenost funkcia na generovanie vztah z hora nadol
	 *Argumenty: menotabulky v ktorej sa nachadzam, prehladavam je meno tabulky v ktorej budem hladat vztah pre menotabulky, poradove cislo zanorenia
	 */
	function vztahPodradenost($menoTabulky, $prehladavam, $zanorenie)
	{
		global $databazaG;
		global $databaza;
		foreach($databaza[$prehladavam] as $sqlPolozka)
		{
			$polozka = preg_replace("/_id INT.*/", "", $sqlPolozka);
			if(!(array_key_exists($polozka, $databaza))) //ak neni tabulka s cislom tak odstranim cislo a pozrem sa ci to neni tabulka
			{
				$polozka = preg_replace("/\d/", "", $polozka);
				if(!(array_key_exists($polozka, $databaza))) //ak ani bez cisla to neni tabulka tak to preksocim
				{
					continue;
				}
				else
				{
					if($zanorenie > 1) //ak to je nte zanorenie tak musim kontrolovat ci sa tam uz taka polozka nenachadza kvoli zacykleniu
					{
						if(!(in_array('		<relation to="' . $polozka . '" relation_type="1:1"/>', $databazaG[$menoTabulky])) and !(in_array('		<relation to="' . $polozka . '" relation_type="N:M"/>', $databazaG[$menoTabulky])))
						{
							if(!(in_array('		<relation to="' . $polozka . '" relation_type="N:1"/>', $databazaG[$menoTabulky])) and !(in_array('		<relation to="' . $polozka . '" relation_type="1:N"/>', $databazaG[$menoTabulky])))
							{
								generujVztah($menoTabulky, $polozka, $zanorenie);
								vztahPodradenost($menoTabulky, $polozka, ++$zanorenie);
							}
						}
					}
					else //inak 0 zanorenie, teda mozem hned generovat
					{
						generujVztah($menoTabulky, $polozka, $zanorenie);
						vztahPodradenost($menoTabulky, $polozka, ++$zanorenie);
					}
				}
			}
			else //ak to je tabulka s cislom
			{
				if($zanorenie > 1) //analogia to co vyssie
				{
					if(!(in_array('		<relation to="' . $polozka . '" relation_type="1:1"/>', $databazaG[$menoTabulky])) and !(in_array('		<relation to="' . $polozka . '" relation_type="N:M"/>', $databazaG[$menoTabulky])))
					{
						if(!(in_array('		<relation to="' . $polozka . '" relation_type="N:1"/>', $databazaG[$menoTabulky])) and !(in_array('		<relation to="' . $polozka . '" relation_type="1:N"/>', $databazaG[$menoTabulky])))
						{
							generujVztah($menoTabulky, $polozka, $zanorenie);
							vztahPodradenost($menoTabulky, $polozka, ++$zanorenie);
						}
					}
				}
				else
				{
					generujVztah($menoTabulky, $polozka, $zanorenie);
					vztahPodradenost($menoTabulky, $polozka, ++$zanorenie);
				}
			}
		}
	}

	/*
	 *Funkcia na generovanie vztahov z dola symetricke vztahy k N1 teda 1N
	 */
	function vztahNadradenost()
	{
		global $databazaG;
		global $databaza;
		foreach($databazaG as $gTabulka)
		{
			$menoNovejPolozky = preg_replace("/.*table name=\"/", "", $gTabulka[0]); //zistim si meno tabulky, vktorej som
			$menoNovejPolozky = preg_replace("/\">/", "", $menoNovejPolozky);
			foreach($gTabulka as $gPolozka)
			{
				if(preg_match("/ relation_type=\"N:1\"\/>/", $gPolozka)) //ak tam je vztah N1 tak nageneruje symetricky k nemu
				{
					$menoTabulky = preg_replace("/.*<relation to=\"/", "", $gPolozka); //meno cudzieho kluca ktory budem pushovat
					$menoTabulky = preg_replace("/\" relation_type=\"N:1\"\/>/", "", $menoTabulky);
					if(!(in_array('		<relation to="' . $menoNovejPolozky . '" relation_type="1:1"/>', $databazaG[$menoTabulky])) and !(in_array('		<relation to="' . $menoNovejPolozky . '" relation_type="N:M"/>', $databazaG[$menoTabulky])))
					{
						if(!(in_array('		<relation to="' . $menoNovejPolozky . '" relation_type="N:1"/>', $databazaG[$menoTabulky])) and !(in_array('		<relation to="' . $menoNovejPolozky . '" relation_type="1:N"/>', $databazaG[$menoTabulky])))
						{
							array_push($databazaG[$menoTabulky], '		<relation to="' . $menoNovejPolozky . '" relation_type="1:N"/>');
						}
					}
				}
			}
		}
	}

	/*
	 *Funkcia na generovanie vztahov NM
	 */
	function vztahRovnocennost()
	{
		global $databazaG;
		global $databazaNM;
		foreach($databazaG as $gTabulka1)
		{
			$menoTabulky = preg_replace("/.*table name=\"/", "", $gTabulka1[0]); //vytiahnem si meno tabulky
			$menoTabulky = preg_replace("/\">/", "", $menoTabulky);
			foreach($databazaG as $gTabulka2)
			{
				$spolocne = array_intersect($gTabulka1, $gTabulka2); //ak maju 2 tabulky spolocne prvky tak ich mozem nagenerovat
				if(!(empty($spolocne))) //maju spolocne
				{
					foreach($gTabulka2 as $polozka)
					{
						if(preg_match("/.*\<table name=.*/", $polozka)) //neni to cudzi kluc
						{
							continue;
						}
						$polozkaSprac = preg_replace("/.*<relation to=\"/", "", $polozka); //vytiehnem si z toho meno
						$polozkaSprac = preg_replace("/\" relation_type=\"N:1\"\/>/", "", $polozkaSprac);
						$polozkaSprac = preg_replace("/\" relation_type=\"1:N\"\/>/", "", $polozkaSprac);
						$polozkaSprac = preg_replace("/\" relation_type=\"N:M\"\/>/", "", $polozkaSprac);
						$polozkaSprac = preg_replace("/\" relation_type=\"1:1\"\/>/", "", $polozkaSprac);
						if(!(in_array('		<relation to="' . $polozkaSprac . '" relation_type="1:1"/>', $databazaG[$menoTabulky])) and !(in_array('		<relation to="' . $polozkaSprac . '" relation_type="N:M"/>', $databazaG[$menoTabulky])))
						{
							if(!(in_array('		<relation to="' . $polozkaSprac . '" relation_type="N:1"/>', $databazaG[$menoTabulky])) and !(in_array('		<relation to="' . $polozkaSprac . '" relation_type="1:N"/>', $databazaG[$menoTabulky])))
							{
								if(!(in_array('		<relation to="' . $polozkaSprac . '" relation_type="N:M"/>', $databazaNM[$menoTabulky])))
								{
									array_push($databazaNM[$menoTabulky], '		<relation to="' . $polozkaSprac . '" relation_type="N:M"/>'); //pushujem do pomocnej tabulky aby sa to nezacyklilo ak stale pridavam
								}
							}
						}
					}
				}
			}
		}
		foreach($databazaG as $gTabulka1) //prechadzam tabulky a spojujem ju s rovnakym indexom s pomocou tabulkou
		{
			$menoTabulky = preg_replace("/.*table name=\"/", "", $gTabulka1[0]);
			$menoTabulky = preg_replace("/\">/", "", $menoTabulky);
			$databazaG[$menoTabulky] = array_merge($databazaG[$menoTabulky], $databazaNM[$menoTabulky]);
		}
	}

	
	/*
	 *Funkcia iba spusta jednotlive funkcie na spracovanie xml suboru 
	 *Argumenty: cely xml subor
	 */
	function xmlDoG($vstup)
	{
		global $databazaG;
		global $databazaNM;
		global $databaza;
		foreach($databaza as $sqlTabulka) //najprv si vytvorim prazdne tabulky, nech nemusim kontrolovat ci existuju
		{
			$menoTabulky = preg_replace("/CREATE TABLE /", "", $sqlTabulka[0]);
			$menoTabulky = preg_replace("/\($/", "", $menoTabulky);
			$menoTabulkyNM = $menoTabulky . "NM";
			foreach($sqlTabulka as $sqlPolozka)
			{
				if(!(array_key_exists($menoTabulky, $databazaG)))
				{
					$$menoTabulky = array('	<table name="'. $menoTabulky .'">');
					$$menoTabulkyNM = array();
					array_push($$menoTabulky, '		<relation to="' . $menoTabulky . '" relation_type="1:1"/>');
					$databazaG[$menoTabulky] = $$menoTabulky;
					$databazaNM[$menoTabulky] = $$menoTabulkyNM;
				}
			}
		}
		foreach($databaza as $sqlTabulka) //generujem vztahy z hora dole
		{
			$menoTabulky = preg_replace("/CREATE TABLE /", "", $sqlTabulka[0]);
			$menoTabulky = preg_replace("/\($/", "", $menoTabulky);
			vztahPodradenost($menoTabulky, $menoTabulky, $zanorenie = 0);	
		}
		vztahNadradenost();
		vztahRovnocennost();
	}
?>