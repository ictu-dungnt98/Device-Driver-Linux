#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lcd_lib.h"

int main()
{
	char c;

	while(1) {
		c = getchar();
		lcd_clear_screen();
		switch(c) {
			case 0:
				return 0;
			case 1:
				lcd_draw_circle(12, 5, 4, Pixel_Set);
				break;
			case 2:
				lcd_gotoxy(20, 24);
				draw_string("Dungnt98 Linux", Pixel_Set, FontSize_5x7);
				break;
			case 3:
				lcd_draw_rect(24, 5, 48,10 , Pixel_Set);
				break;
			default:
				lcd_gotoxy(0, 0);
				draw_string("Dungnt98 Linux", Pixel_Set, FontSize_5x7);
				break;
		}
		
		lcd_send_buff();
	}
	return 0;
}

