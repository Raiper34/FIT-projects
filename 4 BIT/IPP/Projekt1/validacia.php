<?php
#Filip Gulan
#XTD:xgulan00

	/*
	 *Funkcia ktora zistuje ci je subor zadany prepinacom isvalid validny v zmysle ze ho mozem vlozit do databaze zo input
	 *Argumenty: databaza yrcena na validaciu
	 */
	function jevalidny($databazaValidacia)
	{
		global $databaza;
		global $STDERR;
		$surovnake = array_diff_key($databazaValidacia, $databaza); //daju sa vsetky tabulky vlozit do databazovej struktury
		if(!(empty($surovnake))) //ak sa tam prvky nezmestia vypisem chybu
		{
			fwrite($STDERR, "Subor je navalidny! \n");
			fclose($STDERR);
			exit(91);
		}
		foreach($databazaValidacia as $tabulkaValidacia)
		{
			$menotabulkyV =  preg_replace("/CREATE TABLE /", "", $tabulkaValidacia[0]); //istim si meno tabulky
			$menotabulkyV = preg_replace("/\($/", "", $menotabulkyV);
			foreach($tabulkaValidacia as $riadokValidacia)
			{
				if(preg_match('/CREATE TABLE /',$riadokValidacia) or preg_match('/\);$/',$riadokValidacia) or preg_match('/ PRIMARY KEY.*$/',$riadokValidacia))
				{//ak to neni cudzi kluc
					continue;
				}
				$polozka = preg_replace("/ BIT.*/", "", $riadokValidacia); //ziskam si meno cudzieho kluca
				$polozka = preg_replace("/ INT.*/", "", $polozka);
				$polozka = preg_replace("/ FLOAT.*/", "", $polozka);
				$polozka = preg_replace("/ NVARCHAR.*/", "", $polozka);
				$polozka = preg_replace("/ NTEXT.*/", "", $polozka);
				if(preg_match('/BIT/',$riadokValidacia)) //zistujem ci datovy ak je BIT mozem vlozit do druhej tabulky ako ostatne
				{
					if(!(in_array($polozka . " NTEXT,", $databaza[$menotabulkyV])) and !(in_array($polozka . " NTEXT", $databaza[$menotabulkyV])))
					{
						if(!(in_array($polozka . " NVARCHAR,", $databaza[$menotabulkyV])) and !(in_array($polozka . " NVARCHAR", $databaza[$menotabulkyV])))
						{
							if(!(in_array($polozka . " FLOAT,", $databaza[$menotabulkyV])) and !(in_array($polozka . " FLOAT", $databaza[$menotabulkyV])))
							{	
								if(!(in_array($polozka . " INT,", $databaza[$menotabulkyV])) and !(in_array($polozka . " INT", $databaza[$menotabulkyV])))
								{
									if(!(in_array($polozka . " BIT,", $databaza[$menotabulkyV])) and !(in_array($polozka . " BIT", $databaza[$menotabulkyV])))
									{
										fwrite($STDERR, "Subor je navalidny! \n");
										fclose($STDERR);
										exit(91);
									}
								}
							}
						}
					}
				}
				if(preg_match('/INT/',$riadokValidacia)) //zistujem ci mozem vlozit INT do druhej tabulky, moze mat hocico okrem BIT, do BIT nemozem vlozit
				{
					if(!(in_array($polozka . " NTEXT,", $databaza[$menotabulkyV])) and !(in_array($polozka . " NTEXT", $databaza[$menotabulkyV])))
					{
						if(!(in_array($polozka . " NVARCHAR,", $databaza[$menotabulkyV])) and !(in_array($polozka . " NVARCHAR", $databaza[$menotabulkyV])))
						{
							if(!(in_array($polozka . " FLOAT,", $databaza[$menotabulkyV])) and !(in_array($polozka . " FLOAT", $databaza[$menotabulkyV])))
							{	
								if(!(in_array($polozka . " INT,", $databaza[$menotabulkyV])) and !(in_array($polozka . " INT", $databaza[$menotabulkyV])))
								{
									fwrite($STDERR, "Subor je navalidny! \n");
									fclose($STDERR);
									exit(91);
								}
							}
						}
					}
				}
				if(preg_match('/FLOAT/',$riadokValidacia)) //nemozem vlozit do bit a int
				{
					if(!(in_array($polozka . " NTEXT,", $databaza[$menotabulkyV])) and !(in_array($polozka . " NTEXT", $databaza[$menotabulkyV])))
					{
						if(!(in_array($polozka . " NVARCHAR,", $databaza[$menotabulkyV])) and !(in_array($polozka . " NVARCHAR", $databaza[$menotabulkyV])))
						{
							if(!(in_array($polozka . " FLOAT,", $databaza[$menotabulkyV])) and !(in_array($polozka . " FLOAT", $databaza[$menotabulkyV])))
							{	
								fwrite($STDERR, "Subor je navalidny! \n");
								fclose($STDERR);
								exit(91);
							}
						}
					}
				}
				if(preg_match('/NVARCHAR/',$riadokValidacia)) //nemozem vlzit do bit int a float
				{
					if(!(in_array($polozka . " NTEXT,", $databaza[$menotabulkyV])) and !(in_array($polozka . " NTEXT", $databaza[$menotabulkyV])))
					{
						if(!(in_array($polozka . " NVARCHAR,", $databaza[$menotabulkyV])) and !(in_array($polozka . " NVARCHAR", $databaza[$menotabulkyV])))
						{	
							fwrite($STDERR, "Subor je navalidny! \n");
							fclose($STDERR);
							exit(91);
						}
					}
				}
				if(preg_match('/NTEXT/',$riadokValidacia)) //mozem vlozit iba do ntext
				{
					if(!(in_array($polozka . " NTEXT,", $databaza[$menotabulkyV])) and !(in_array($polozka . " NTEXT", $databaza[$menotabulkyV])))
					{	
						fwrite($STDERR, "Subor je navalidny! \n");
						fclose($STDERR);
						exit(91);
					}
				}
			}
		}
	}

?>