-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2011 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Zdenek Vasicek <vasicek AT fit.vutbr.cz>
--            Branislav Blaskovic <xblask00 AT stud.fit.vutbr.cz>
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
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
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

	type stavy is (idle, fetch, decode, halt, plus2, minus2, vypis2, nacitaj2, while_doprava, while_doprava_pauza, while_dolava, while_dolava_pauza, while_over_nula, while_over_koniec);
	type instrukcia is (inkrementuje, dekrementuje, plus, minus, zacni_while, konci_while, vypis, nacitaj, preskoc, nic_nul);
                   
	-- zde dopiste potrebne deklarace signalu
	signal PC		: std_logic_vector(11 downto 0);
	signal PC_INC	: std_logic;
	signal PC_DEC	: std_logic;
	signal CNT		: std_logic_vector(7 downto 0);
	signal CNT_INC	: std_logic;
	signal CNT_DEC	: std_logic;
	signal CNT_SET1	: std_logic;
	signal PTR		: std_logic_vector(9 downto 0);
	signal PTR_INC	: std_logic;
	signal PTR_DEC	: std_logic;
	
	signal pstate : stavy;
	signal nstate : stavy;
	signal sinst : instrukcia;

begin
	
	CODE_ADDR <= PC;
	DATA_ADDR <= PTR;
	
	-- Citace, inkrementacie, dekremantacie, atd
	pc_cntr: process(RESET, CLK, PC_INC, PC_DEC)
	begin
		if (RESET='1') then
			PC <= "000000000000";
		elsif (CLK'event) and (CLK='1') then
			if(PC_INC='1') then
				PC <= PC + 1;
			elsif (PC_DEC='1') then
				PC <= PC - 1;
			end if;
		end if;
	end process;
	
	ptr_cntr: process(RESET, CLK, PTR_INC, PTR_DEC)
	begin
		if (RESET='1') then
			PTR <= "0000000000";
		elsif (CLK'event) and (CLK='1') then
			if(PTR_INC='1') then
				PTR <= PTR + 1;
			elsif (PTR_DEC='1') then
				PTR <= PTR - 1;
			end if;
		end if;
	end process;
	
	cnt_cntr: process(RESET, CLK)
	begin
		if (RESET='1') then
			CNT <= "00000000";
		elsif (CLK'event) and (CLK='1') then
			if (CNT_INC='1') then
				CNT <= CNT + 1;
			elsif (CNT_DEC='1') then
				CNT <= CNT - 1;
			elsif (CNT_SET1='1') then
				CNT <= "00000001";
			end if;
		end if;
	end process;
	
	--FSM present state
   fsm_pstate: process(RESET, CLK)
   begin
      if (RESET='1') then
         pstate <= idle;
      elsif (CLK'event) and (CLK='1') then
         if (EN = '1') then
            pstate <= nstate;
         end if;
      end if;
   end process;
	
	nacitaj_inst: process(CODE_DATA)
    begin
		case (CODE_DATA) is
			when X"3E"   =>
				sinst <= inkrementuje;	-- >
			when X"3C"   =>
				sinst <= dekrementuje;	-- <
			when X"2B"   =>
				sinst <= plus;			-- +
			when X"2D"   =>
				sinst <= minus;			-- -
			when X"5B"   => 
				sinst <= zacni_while;		-- [
			when X"5D"   => 
				sinst <= konci_while;		-- ]
			when X"2E"   => 
				sinst <= vypis;			-- .
			when X"2C"   => 
				sinst <= nacitaj;			-- ,
			when X"00"   => 
				sinst <= nic_nul;
			when others => sinst <= preskoc;
		end case;
		
	end process;


	instrukcia_proc: process(CODE_DATA, DATA_RDATA, EN, IN_DATA, IN_VLD, OUT_BUSY, pstate, sinst)
	begin
		
		nstate <= idle;
		
		DATA_EN <= '0';
		
		PC_INC <= '0';
		PC_DEC <= '0';
		CNT_INC <= '0';
		CNT_DEC <= '0';
		CNT_SET1 <= '0';
		IN_REQ <= '0';
		PTR_INC <= '0';
		PTR_DEC <= '0';
		CODE_EN <= '0';
		OUT_WE <= '0';

		case(pstate) is
			when idle =>
				nstate <= fetch;
				
				
			when fetch =>
				nstate <= decode;
				CODE_EN <= '1';
				
			when decode =>
				case(sinst) is
					when inkrementuje =>
						PTR_INC <= '1';
						PC_INC <= '1';
						nstate <= idle;
						
					when dekrementuje =>
						PTR_DEC <= '1';
						PC_INC <= '1';
						nstate <= idle;
					
					when plus =>
						DATA_EN <= '1';
						DATA_RDWR <= '0';
						
						nstate <= plus2;
					
					when minus =>
						DATA_EN <= '1';
						DATA_RDWR <= '0';
						
						nstate <= minus2;
					
					when vypis =>
						DATA_RDWR <= '0';
						DATA_EN <= '1';

						nstate <= vypis2;
					
					when nacitaj =>
						IN_REQ <= '1';
						
						nstate <= nacitaj2;
					
					when zacni_while =>
						DATA_EN <= '1';
						DATA_RDWR <= '0';
						nstate <= while_over_nula;
					
					when konci_while =>
						DATA_EN <= '1';
						DATA_RDWR <= '0';
						nstate <= while_over_koniec;
					
					when preskoc =>
						nstate <= idle;
						PC_INC <= '1';
					
					when nic_nul =>
						nstate <= halt;
					
					when others =>
						PC_INC <= '1';
					
				end case;
				-- end Spracuj instrukciu
			
			when while_over_nula =>
				if(DATA_RDATA=0) then
					PC_INC <= '1';
					CNT_INC <= '1';
					nstate <= while_doprava_pauza;
				else
					nstate <= fetch;
					PC_INC <= '1';
				end if;
			
			when while_over_koniec =>
				if(DATA_RDATA=0) then
					nstate <= fetch;
					PC_INC <= '1';
				else
					CNT_SET1 <= '1';
					PC_DEC <= '1';
					nstate <= while_dolava_pauza;
				end if;
			
			when while_doprava =>
				if (CNT=0) then
					nstate <= fetch;
				elsif (sinst=konci_while) then
					CNT_DEC <= '1';
					PC_INC <= '1';
					nstate <= while_doprava_pauza;
				elsif (sinst=zacni_while) then
					CNT_INC <= '1';
					PC_INC <= '1';
					nstate <= while_doprava_pauza;
				else
					PC_INC <= '1';
					nstate <= while_doprava_pauza;
				end if;
				
			when while_dolava =>
				if(sinst=konci_while) then
					PC_DEC <= '1';
					CNT_INC <= '1';
					nstate <= while_dolava_pauza;
				elsif(sinst=zacni_while) and (CNT=1) then
					CNT_DEC <= '1';
					nstate <= fetch;
				elsif(sinst=zacni_while) and (CNT/=1) then
					PC_DEC <= '1';
					CNT_DEC <= '1';
					nstate <= while_dolava_pauza;
				else
					nstate <= while_dolava_pauza;
					PC_DEC <= '1';
					CODE_EN <= '1';
				end if;
			
			when while_dolava_pauza =>
				CODE_EN <= '1';
				nstate <= while_dolava;
				
			when while_doprava_pauza =>
				CODE_EN <= '1';
				nstate <= while_doprava;
			
			when plus2 =>
				DATA_WDATA <= DATA_RDATA + 1;
				DATA_RDWR <= '1';
				DATA_EN <= '1';
				
				PC_INC <= '1';
				nstate <= fetch;
				
			when minus2 =>
				DATA_WDATA <= DATA_RDATA - 1;
				DATA_RDWR <= '1';
				DATA_EN <= '1';
				
				PC_INC <= '1';
				nstate <= fetch;
			
			when vypis2 =>
				if (OUT_BUSY='0') then
					OUT_DATA <= DATA_RDATA;
					OUT_WE <= '1';
					
					PC_INC <= '1';
					nstate <= fetch;
				end if;
			
			when nacitaj2 =>
				if (IN_VLD='1') then
					DATA_RDWR <= '1';
					DATA_WDATA <= IN_DATA;
					DATA_EN <= '1';
					PC_INC <= '1';
					nstate <= fetch;
				else
					IN_REQ <= '1';
					nstate <= nacitaj2;
				end if;
			
			when halt =>
				nstate <= halt;
			
			when others =>
		end case;
	
	end process;

 

end behavioral;

