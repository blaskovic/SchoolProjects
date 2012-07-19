library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;
use IEEE.std_logic_arith.all;

entity ledc8x8 is
port(
	RESET: in std_logic;
	SMCLK: in std_logic;
	ROW: out std_logic_vector(7 downto 0); 
	LED: out std_logic_vector(0 to 7)
); end ledc8x8;

architecture behv of ledc8x8 is

signal ctrl_counter: std_logic_vector(21 downto 0);
signal row_counter: std_logic_vector(7 downto 0);
signal led_light: std_logic_vector(7 downto 0);
signal ce: std_logic;
signal switch: std_logic;
begin
	
	-- hlavne pocitadlo a nastavovadlo 'ce'
	ctrl_cnt: process(SMCLK, RESET, switch, ce, ctrl_counter)
	begin
		
		-- ked chceme premazat citac - asynchronne
		if RESET = '1' then
			ctrl_counter <= "0000000000000000000000";		
		-- pripocitavame k hlavnemu citacu
		elsif SMCLK'event and SMCLK = '1' then
			
			ctrl_counter <=  ctrl_counter + "1";
			
			if ctrl_counter(7 downto 0) = "11111111" then
				ce <= '1';
			else
				ce <= '0';
			end if;
			
			switch <= ctrl_counter(21);

			
		end if;
		
	end process ctrl_cnt;
	
	-- posuvac riadkov displeja
	row_cnt: process(SMCLK, RESET, ce, row_counter)
	begin
		
		-- ked chcem asynchronne nastavit po resete prvy riadok
		if RESET = '1' then
			row_counter <= "11111110";
		-- posuvam riadky, ak ce je v 1
		elsif SMCLK'event and ce = '1' and SMCLK = '1' then --  ce'event and ce = '1'
			row_counter <= row_counter(6 downto 0) & row_counter(7);
		end if;
		
	end process row_cnt;

	-- vykreslenie riadkov
	dec: process(SMCLK, row_counter, led_light )
	begin
	
		if SMCLK'event and SMCLK = '1' then
			case row_counter is
				when "11111110" => led_light <= "11100000"; -- 1
				when "11111101" => led_light <= "10011110"; -- 2
				when "11111011" => led_light <= "10011001"; -- 3
				when "11110111" => led_light <= "11101001"; -- 4
				when "11101111" => led_light <= "10011110"; -- 5
				when "11011111" => led_light <= "10011001"; -- 6
				when "10111111" => led_light <= "11101001"; -- 7
				when "01111111" => led_light <= "00001110"; -- 8
				when others => led_light <= "00000000";
			end case;
		end if;
	
	end process dec;
	
	-- magic box
	magic_box: process(SMCLK, led_light, switch, row_counter)
	begin
	
		if SMCLK'event and SMCLK = '1' then
			ROW <= row_counter;
			-- ked mame switch v 1, zhasne displej
			if switch = '1' then
				LED <= "00000000";
			else
				LED <= led_light;
			end if;
			
		end if;
	
	end process magic_box;

end architecture;
