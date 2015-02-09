-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2014 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Filip Gulan xgulan00@stud.fit.vutbr.cz
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------

architecture behavioral of cpu is

--------------------------------
--			Stavy
--------------------------------
--1row inicializacne
--2row dekoder
--3row stavy pre pracu s hodnotou ukazatela
--4row stavy pre pracu s aktual bunkou
--5row stavy na vstup
--6row stavy na vystup
--7row stavy zaciatku cyklov
--8row stavy koncu cyklov
--9row koncove stavy/nic nerob...
type fsm_state is (
sidle, sfetch,
sdecode, 
sv_inc, sv_dec,
sp_inc, sp_inc_do, sp_dec, sp_dec_do,  
sget, sget_do, 
sput, sput_do, 
ss_while1, ss_while2, ss_while3, ss_while4, ss_while5,
se_while1, se_while2, se_while3, se_while4, se_while5,
shalt, snop
);
signal pstate : fsm_state;
signal nstate : fsm_state;

---------------------------------
--			Instrukcie
---------------------------------
--pre popis viz dekoder
type instruction_type is (
v_inc, v_dec,
p_inc, p_dec,
while_s, while_e,
put, get,
halt, nop
);
signal instruction : instruction_type;

---------------------------------
--			Vnutorne signaly
---------------------------------
signal reg_pc_addr:	std_logic_vector(12 downto 0);
signal reg_pc_inc :	std_logic;
signal reg_pc_dec : std_logic;

signal reg_ptr_addr: std_logic_vector(12 downto 0);
signal reg_ptr_inc : std_logic;
signal reg_ptr_dec : std_logic;

signal reg_cnt_addr : std_logic_vector(7 downto 0);
signal reg_cnt_inc : std_logic;
signal reg_cnt_dec : std_logic;

signal sel1 : std_logic;
signal sel2 : std_logic_vector(1 downto 0);

------------------------------------------------------------------
--							Komponenty
------------------------------------------------------------------
begin

----------------------------
--		Register PC
----------------------------
reg_pc_proc: process (RESET, CLK)
begin
	if (RESET = '1') then --asynchronny reset
		reg_pc_addr <= (others => '0');
	elsif (CLK'event) and (CLK='1') then
		if (reg_pc_inc = '1') then
			reg_pc_addr <= reg_pc_addr + 1;
		elsif (reg_pc_dec = '1') then
			reg_pc_addr <= reg_pc_addr - 1;
		end if;
	end if;
end process;

---------------------------
--		Register PTR
---------------------------
reg_ptr_proc: process (RESET, CLK)
begin
	if (RESET = '1') then --asynchronny reset
		reg_ptr_addr <= "1000000000000"; --musim to dat takto lebo 13 bitovy, je to na adr X1000
	elsif (CLK'event) and (CLK = '1') then
		if (reg_ptr_inc = '1') then
			reg_ptr_addr <= reg_ptr_addr + 1;
		elsif (reg_ptr_dec = '1') then
			reg_ptr_addr <= reg_ptr_addr - 1;
		end if;
	end if;
end process;

---------------------------
--		Register CNT
---------------------------
reg_cnt_proc: process (RESET, CLK)
begin
	if (RESET = '1') then --asynchronny reset
		reg_cnt_addr <= (others => '0');
	elsif (CLK'event) and (CLK = '1') then
		if (reg_cnt_inc = '1') then
			reg_cnt_addr <= reg_cnt_addr + 1;
		elsif (reg_cnt_dec = '1') then
			reg_cnt_addr <= reg_cnt_addr - 1;
		end if;
	end if;
end process;

--------------------------
--	Dekoder instrukcii
--------------------------
decode_proc: process (DATA_RDATA)
begin
	case (DATA_RDATA) is
		when X"3E" => instruction <= v_inc; -->
		when X"3C" => instruction <= v_dec; --<
		when X"2B" => instruction <= p_inc; --+
		when X"2D" => instruction <= p_dec; ---
		when X"5B" => instruction <= while_s; --[
		when X"5D" => instruction <= while_e; --]
		when X"2E" => instruction <= put; --.
		when X"2C" => instruction <= get; --,
		when X"00" => instruction <= halt; --null
		when others=> instruction <= nop; --other
	end case;
end process;

----------------------------
--		Multiplexor 1
----------------------------
mx1: process(CLK, sel1, reg_ptr_addr, reg_pc_addr)
begin
	case sel1 is
		when '0' => DATA_ADDR <= reg_ptr_addr;
		when '1' => DATA_ADDR <= reg_pc_addr;
		when others =>
	end case;
end process;

-----------------------------
--		Multiplexor 2
----------------------------
mx2: process(CLK, sel2, DATA_RDATA, IN_DATA)
begin
	case sel2 is
		when "00" => DATA_WDATA <= IN_DATA;
		when "01" => DATA_WDATA <= DATA_RDATA + 1;
		when "10" => DATA_WDATA <= DATA_RDATA - 1;
		when "11" => DATA_WDATA <= X"00";
		when others =>
	end case;
end process;

---------------------------
--	Konecny automat pstate
---------------------------
fsm_pstate_proces: process(RESET, CLK)
begin
	if (RESET = '1') then
		pstate <= sidle;
	elsif (CLK'event) and (CLK = '1') then
		if (EN = '1') then
			pstate <= nstate;
		end if;
	end if;
end process;

-------------------------------------------------------------------------------
--								Konecny automat logika nstate
-------------------------------------------------------------------------------
fsm_nstate_proces : process(IN_VLD, IN_DATA, DATA_RDATA, OUT_BUSY, pstate, instruction, reg_cnt_addr, sel1, sel2)
begin
	----------Default----------
	DATA_EN <= '0';
	DATA_RDWR <= '0';
	sel2 <= "11";
	reg_pc_inc <= '0';
	reg_pc_dec <= '0';
	reg_ptr_inc <= '0';
	reg_ptr_dec <= '0';
	reg_cnt_inc <= '0';
	reg_cnt_dec <= '0';
	IN_REQ <= '0';
	OUT_WE <= '0';
	
	case pstate is
		---------SIDLE----------
		when sidle =>
			nstate <= sfetch;
		---------SFETCH---------
		when sfetch =>
			nstate <= sdecode;
			DATA_EN <= '1';
			sel1 <= '1';
		---------SDECODE--------
		when sdecode =>
			case instruction is		
				when halt =>
					nstate <= shalt;				
				when v_inc =>
					nstate <= sv_inc;
				when v_dec =>
					nstate <= sv_dec;				
				when p_inc =>
					nstate <= sp_inc;			
				when p_dec =>
					nstate <= sp_dec;			
				when while_s =>
					nstate <= ss_while1;					
				when while_e =>
					nstate <= se_while1;
				when get =>
					nstate <= sget;
				when put =>
					nstate <= sput;
				when others =>
					nstate <= snop;
			end case;
		------------INSTR >----------
		when sv_inc =>
			nstate <= sfetch;
			reg_ptr_inc <= '1';
			reg_pc_inc <= '1';
		-----------INSTR <-----------
		when sv_dec =>
			nstate <= sfetch;
			reg_ptr_dec <= '1';
			reg_pc_inc <= '1';
		----------INSTR +-----------
		when sp_inc =>
			nstate <= sp_inc_do;
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			sel1 <= '0';		
		when sp_inc_do =>
			nstate <= sfetch;
			DATA_EN <= '1';
			DATA_RDWR <= '1';
			sel1 <= '0';
			sel2 <= "01";
			reg_pc_inc	<= '1';
		-----------INSTR - ----------
		when sp_dec =>
			nstate <= sp_dec_do;
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			sel1 <= '0';
		when sp_dec_do =>
			nstate <= sfetch;
			DATA_EN <= '1';
			DATA_RDWR <= '1';
			sel1 <= '0';
			sel2 <= "10";
			reg_pc_inc <= '1';
		----------SPUT . -------------
		when sput =>
			nstate <= sput_do;
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			sel1 <= '0';
		when sput_do =>
			nstate <= sput_do;
			if (OUT_BUSY = '0') then
				nstate <= sfetch;
				OUT_WE <= '1';
				OUT_DATA <= DATA_RDATA;
				reg_pc_inc <= '1';
			end if;
		----------SGET , --------------
		when sget =>
			nstate <= sget_do;
			IN_REQ <= '1';
		when sget_do =>
			nstate <= sget_do;
			IN_REQ <= '1';
			if (IN_VLD = '1') then
				nstate <= sfetch;
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				sel1 <= '0';
				sel2 <= "00";
				reg_pc_inc <= '1';
			end if;
		-----------While start [------------------
		when ss_while1	=>
			nstate <= ss_while2;
			DATA_EN <= '1';
			reg_pc_inc <= '1';
			sel1 <= '0';
			DATA_RDWR <= '0';
		-- vykonavanie if (ram[PTR] == 0)
		when ss_while2 =>
			nstate <= sfetch;
			if (DATA_RDATA = "00000000") then
				nstate <= ss_while3;
				reg_cnt_inc <= '1';
			end if;

		when ss_while3	=>
			nstate <= ss_while4; --zaciatok cykla
			DATA_EN <= '1';
			sel1 <= '1';
		--vykonavanie while (CNT != 0)
		when ss_while4 =>
			nstate <= ss_while5;
			reg_pc_inc <= '1';	
			if (instruction = while_s) then
				reg_cnt_inc <= '1';
			elsif (instruction = while_e) then
				reg_cnt_dec <= '1';
			end if;
--
		when ss_while5 =>
			nstate <= ss_while3;
		if (reg_cnt_addr = "00000000") then --hop na zaciatok cykla
			nstate <= sfetch;
		end if;

		---------While end ]------------------------
		when se_while1 =>
			nstate <= se_while2;
			DATA_EN <= '1';
			sel1 <= '0';
			DATA_RDWR <= '0';
		--zaciatok vykonavania if (ram[PTR] == 0)
		when se_while2 =>
			nstate <= sfetch;
			if (DATA_RDATA = "00000000") then
				reg_pc_inc <= '1';
			else
				nstate <= se_while3;
				reg_pc_dec <= '1';
				reg_cnt_inc <= '1';
			end if;

		when se_while3 =>
			nstate <= se_while4;
			DATA_EN <= '1';
			sel1 <= '1';
		--zaciatok vzkonavania while (CNT != 0)
		when se_while4 =>
			nstate <= se_while5;
			if (instruction = while_s) then
				reg_cnt_dec <= '1';
			elsif (instruction = while_e) then
				reg_cnt_inc <= '1';
			end if;

		when se_while5 =>
			nstate <= sfetch;
			if (reg_cnt_addr = "00000000") then
				reg_pc_inc <= '1';
			else
				nstate <= se_while3;
				reg_pc_dec <= '1';
			end if;	
	
		---------SHALT---------
		when shalt =>
			nstate <= shalt;
		----------SNOP---------
		when snop =>
			nstate <= sfetch;
			reg_pc_inc <= '1';

		when others =>

	end case;
end process;	
end behavioral;