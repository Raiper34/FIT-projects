<?php
#Filip Gulan
#XTD:xgulan00
	/*
	 *Hlavne telo program, kde otvaram subory a zapisujem do nich...
	 */
	include_once "argumenty.php"; //kniznica na spracovanie argumentov
	include_once "xml.php"; //kniznica na parsovanie xmlka
	include_once "prepinacg.php";
	include_once "validacia.php";

	mb_internal_encoding('UTF-8'); //aby to bolo v UTF8 vystup

	$STDERR = fopen('php://stderr', 'w+');

	ziskajArgumenty($argv, $argc);


	if(isset($argumenty["input"])) //ak je argument input zadany tak berem zo suboru 
	{
		if(!(file_exists($argumenty["input"])) or !(is_readable($argumenty["input"]))) //ak subor neexistuje alebo sa donho neda citat
		{
			fwrite($STDERR, "XML subor neexistuje alebo nema povolene citanie!\n");
			fclose($STDERR);
			exit(2);
		}
		else
		{
			if(($subor = file_get_contents($argumenty["input"])) === FALSE) //zo suboru sme nedostali to co sme potrebovali
			{
				fwrite($STDERR, "Chyba pri citani suboru!\n");
				fclose($STDERR);
				exit(2);
			}
		}
	}
	else //ak neni input zadany, tak berem zo stdin
	{
		if(($subor = file_get_contents('php://stdin')) === FALSE) //zo suboru sme nedostali to co sme potrebovali
		{
			fwrite($STDERR, "Chyba pri citani suboru!\n");
			fclose($STDERR);
			exit(2);
		}
	}

	if(!($xml = simplexml_load_string($subor))) //ak sa vyskitne chyba pri prevadzani zo xml, nie je validny.... 
	{
		if(isset($argumenty["output"])) //ak bol zadany vystup tak iba nagenerujem prazdny dokuemtn
		{
			file_put_contents($argumenty["output"], "");
			fclose($STDERR);
			exit(0);
		}
		else //inak nic nevypisem prazdny riadok
		{
			echo "";
			fclose($STDERR);
			exit(0);
		}
	}

	if(isset($argumenty["isvalid"])) //ak je zadany argument isvalid
	{
		if(($suborValidacia = file_get_contents($argumenty["isvalid"])) === FALSE) //zo suboru sme nedostali to co sme potrebovali
		{
				fwrite($STDERR, "Chyba pri citani suboru!\n");
				fclose($STDERR);
				exit(2);
		}
		$xmlValidacia = simplexml_load_string($suborValidacia); //nacitam si subor na validaciu
		xmlDoDdl($xmlValidacia); //aj z validacie urobim na ddl
		$databazaValidacia = $databaza; //uchovam si celu ddl databazu do premenej
		$databaza = array();
	}

	if(isset($argumenty["g"])) //aj je zadany prepinac g
	{
		xmlDoDdl($xml);
		xmlDoG($xml);
		if(isset($argumenty["isvalid"])) //kontrolujem validaciu ak je zadany prepinac
		{
			//xmlDoDdl($xml);
			jevalidny($databazaValidacia);
		}
	}
	else //neni prepinac g
	{
		xmlDoDdl($xml);
		if(isset($argumenty["isvalid"])) //kontrolujem validaciu
		{
			jevalidny($databazaValidacia);
		}
	}

	if(isset($argumenty["output"])) //ak je nastaveny output tak pisem na vystup
	{
		if(isset($argumenty["header"])) //ak je zadany parameter header vypisem hlavicku
		{
			if((file_put_contents($argumenty["output"], "--" . $argumenty["header"] . "\n\n")) === FALSE)
			{
				fwrite($STDERR, "Do suboru sa nepodarilo zapisat!\n");
				fclose($STDERR);
				exit(3);
			}
			if(isset($argumenty["g"])) //ak je zadany g tak vypisem pole obsahujuce nagenerovane g
			{
				zapisDoSuboruG();
			}
			else
			{
				foreach($databaza as $tabulka) //vsetko vypisem obyc pole bez g 
				{
					foreach($tabulka as $tabulkaRiadok)
					{
						if((file_put_contents($argumenty["output"], $tabulkaRiadok . "\n", FILE_APPEND)) === FALSE)
						{
							fwrite($STDERR, "Do suboru sa nepodarilo zapisat!\n");
							fclose($STDERR);
							exit(3);
						}
					}
				}
			}
		}
		else //header neni nastaveny
		{
			if((file_put_contents($argumenty["output"], "")) === FALSE)
			{
				fwrite($STDERR, "Do suboru sa nepodarilo zapisat!\n");
				fclose($STDERR);
				exit(3);
			}
			if(isset($argumenty["g"]))
			{
				zapisDoSuboruG();
			}
			else
			{
				foreach($databaza as $tabulka)
				{
					foreach($tabulka as $tabulkaRiadok)
					{
						if((file_put_contents($argumenty["output"], $tabulkaRiadok . "\n", FILE_APPEND)) === FALSE)
						{
							fwrite($STDERR, "Do suboru sa nepodarilo zapisat!\n");
							fclose($STDERR);
							exit(3);
						}
					}
				}
			}
		}
	}
	else //inak ak neni nastaveny output vypisem na stdin
	{
		if(isset($argumenty["header"])) //ak je zadany header vypisem najprv na stdin hlavicku
		{
			echo "--" . $argumenty["header"] . "\n\n";
		}
		if(isset($argumenty["g"]))
		{
			echo '<?xml version="1.0" encoding="UTF-8"?>' . "\n<tables>\n";
			foreach($databazaG as $index => $tabulka)
			{
				foreach ($tabulka as $polozka)
				{	
					echo $polozka . "\n";
				}
				echo "	</table>\n";
			}
			echo "</tables>\n";
		}
		else
		{
			foreach($databaza as $tabulka) //vsetko vypisem
			{
				foreach($tabulka as $tabulkaRiadok)
				{
					echo $tabulkaRiadok . "\n";
				}
			}
		}
	}
?>