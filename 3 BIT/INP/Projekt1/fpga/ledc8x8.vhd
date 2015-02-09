-----------------------------------------------------------------
--INP Projekt 1 Ovladánie maticového displaya--------------------
--Autor: Filip Gulán -- xgulan00 -- xgulan00@stud.fit.vutbr.cz---
-----------------------------------------------------------------

-------------------------------------------------
----------------------Library--------------------
-------------------------------------------------
library IEEE; --nahranie knihovien
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-------------------------------------------------
---------------------Entity----------------------
-------------------------------------------------
entity ledc8x8 is --definovanie entity zo zadanie obrazok 2
	port (
		ROW		: out std_logic_vector (0 to 7);
		LED		: out std_logic_vector (0 to 7);
		RESET	: in std_logic;
		SMCLK	: in std_logic
	);
end ledc8x8;

-------------------------------------------------
-------------------Architecture------------------
-------------------------------------------------
architecture architekt of ledc8x8 is --vykonavanie architektury
signal ledky: std_logic_vector(7 downto 0); --vnutorne signaly
signal riadky: std_logic_vector(7 downto 0);
signal ce_cnt: std_logic_vector(7 downto 0);
signal ce: std_logic;

begin
	---------------------------------
	-- Citac na znizenie frekvencie--
	---------------------------------
    generator_ce: process(SMCLK, RESET) -- odvodenie od SMCLK
    begin
		if RESET = '1' then --asynchrony reset
			ce_cnt <= "00000000";
        elsif SMCLK'event and SMCLK = '1' then --even nastupna hrana
            ce_cnt <= ce_cnt + 1;
        end if;
    end process generator_ce;
    ce <= '1' when ce_cnt = X"FF" else '0';
	
	---------------------------------
	-------Rotacia riadkov ----------
	---------------------------------
	rotacia: process(RESET, ce, SMCLK)
	begin	
		if RESET = '1' then --asynchrony reset
			riadky <= "10000000"; 
		elsif SMCLK'event and SMCLK = '1' and ce = '1' then
			riadky <= riadky(0) & riadky(7 downto 1); --konkatenacia na posunutie jednotky
		end if;
	end process rotacia;
	
	---------------------------------
	--------Dekoder riadkov----------
	---------------------------------
	dekoder: process(riadky)
	begin
		case riadky is
			when "10000000" => ledky <= "00001111";
			when "01000000" => ledky <= "01111111";
			when "00100000" => ledky <= "01110000";
			when "00010000" => ledky <= "00010110";
			when "00001000" => ledky <= "01110111";
			when "00000100" => ledky <= "01110100";
			when "00000010" => ledky <= "01110110";
			when "00000001" => ledky <= "11110000";
			when others =>     ledky <= "11111111";
		end case;
	end process dekoder;
	
	---------------------------------
	--------Zasvietenie riadkov------
	---------------------------------
	ROW <= riadky;
	LED <= ledky;

end architecture architekt;