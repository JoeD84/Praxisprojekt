/*
 * mymenu.h
 *
 *  Created on: 30.11.2011
 *      Author: Dielmann
 */

#ifndef MYMENU_H_
#define MYMENU_H_



menu_t	menu_move_rot = {
   .top_entry = 0,
   .current_entry = 0,
   .entry =  {
      {.flags = 0,
       .select = menu_puts,
       .name = "+90",
       .value = "M 125000\n",
      },
      {.flags = 0,
       .select = menu_puts,
       .name = "-90",
       .value = "M -125000\n",
      },

      {.flags = 0,
       .select = menu_puts,
       .name = "+10.000 Schritte",
       .value = "M 10000\n",
      },
      {.flags = 0,
       .select = menu_puts,
       .name = "-10.000 Schritte",
       .value = "M -10000\n",
      },
      {.flags = 0,
       .select = menu_puts,
       .name = "Gehe zum Ursprung",
       .value = "MA 0\n",
      },
   },
   .num_entries = 5,
   .previous = NULL,
};
menu_t	menu_move_height = {
   .top_entry = 0,
   .current_entry = 0,
   .entry =  {
      {.flags = 0,
       .select = menu_puts,
       .name = "+500.000",
       .value = "2M 500000\n",
      },
      {.flags = 0,
       .select = menu_puts,
       .name = "-500.000",
       .value = "2M -500000\n",
      },

      {.flags = 0,
       .select = menu_puts,
       .name = "+1.000.000 Schritte",
       .value = "2M 1000000\n",
      },
      {.flags = 0,
       .select = menu_puts,
       .name = "-1.000.000 Schritte",
       .value = "2M -1000000\n",
      },
      {.flags = 0,
       .select = menu_puts,
       .name = "Gehe zum Ursprung",
       .value = "2MA 0\n",
      },
   },
   .num_entries = 5,
   .previous = NULL,
};
menu_t	menu_conf = {
   .top_entry = 0,
   .current_entry = 0,
   .entry = {
	  {.flags = 0,
	   .select = menu_puts,
	   .name = "Motorstatus",
	   .value = "A\n",
	  },
	  {.flags = 0,
	   .select = menu_puts,
	   .name = "Setze Ursprung",
	   .value = "D\n",
	  },
	  {.flags = 0,
	   .select = menu_puts,
	   .name = "Write to EEPROM",
	   .value = "Q\n",
	  },
      {.flags = 0,
       .select = menu_puts,
       .name = "Newline 1",
       .value = "N 1\n",
      },
      {.flags = 0,
       .select = menu_puts,
       .name = "Para. auslesen",
       .value = "?\n",
      },
   },
   .num_entries = 4,
   .previous = NULL,
};
menu_t	menu_main = {
   .top_entry = 0,
   .current_entry = 0,
   .entry = {
      {.flags = MENU_FLAG_SUBMENU,
       .select = NULL,
       .name = "Bewegen - Rotation",
       .value = &menu_move_rot,
      },
      {.flags = MENU_FLAG_SUBMENU,
       .select = NULL,
       .name = "Bewegen - Hoehe",
       .value = &menu_move_height,
      },
      {.flags = MENU_FLAG_SUBMENU,
       .select = NULL,
       .name = "Konfigurieren",
       .value = &menu_conf,
      },
   },
   .num_entries = 3,
   .previous = NULL,
};
menu_context_t menu_context = {
   .x_loc = 0,
   .y_loc = 0,
   .height = 4,
   .width = 19,
   .menu = NULL,
};


#endif /* MYMENU_H_ */
