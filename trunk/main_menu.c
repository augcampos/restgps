/**
 * 		\file main_menu.c
 * 		\brief Implementation file of main menu.
 * 		\author Augusto Campos
 * 
 * 		\par Copyright
 * 		Copyright 2008 Augusto Campos <augcampos@augcampos.pt>\n
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 3 of the License.
 *      \par
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty 
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      \par
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main_menu.h"
#include "restaurant.h"
#include "utils.h"
#include "main.h"

#define MENU_OPTION_00_STR "* 0 - Exit                          *\n"
#define MENU_OPTION_01_STR "* 1 - Insert Restaurant             *\n"
#define MENU_OPTION_02_STR "* 2 - Delete Restaurant             *\n"
#define MENU_OPTION_03_STR "* 3 - Edit Restaurant               *\n"
#define MENU_OPTION_04_STR "* 4 - Import list from File         *\n"
#define MENU_OPTION_05_STR "* 5 - Export List to File           *\n"
#define MENU_OPTION_06_STR "* 6 - Sort list by distance to user *\n"
#define MENU_OPTION_07_STR "* 7 - Find restaurants              *\n"
#define MENU_OPTION_08_STR "* 8 - List Open restaurants         *\n"
#define MENU_OPTION_09_STR "* 9 - List all restaurants          *\n"
#define MENU_OPTION_99_STR "* 99- Load test data                *\n"
#define MENU_OPTION_SEP_STR "*************************************\n"

/** Find a restaurant in the restaurant list 
 * \return 
 * 			- pointer to a restaurante
 * 		   	- NULL if not found
 * \see restaurant_find
 */
prestaurant_t rest_find() {
	int i;
	char *vlt;

	for (i = ID; i <= OBS; i++) {
		printf(" %5i -> %s\n", i, restaurant_get_field_name(i));
	}
	i = kget_int("Select field for search :");
	printf("\n[%s] == ", restaurant_get_field_name(i));
	vlt = kget_char("", 500);

	return restaurant_find(i, vlt);
}

/** Menu option for exit the main programm */
void menu_exit() {
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_00_STR);
	printf(MENU_OPTION_SEP_STR);
	puts("Clean up");
	restaurant_clear();
	puts("Bye!!");
}

/** Menu option for insert a new restaurant in the list */
void menu_insert() {
	int i;
	char mess[80];
	prestaurant_t r = restaurant_new();
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_01_STR);
	printf(MENU_OPTION_SEP_STR);
	for (i = LONGITUDE; i <= OBS; i++) {

		sprintf(mess, " %s\t: ", restaurant_get_field_name(i));
		switch (i) {
		case LONGITUDE:
			r->longitude = kget_float(mess);
			break;
		case LATITUDE:
			r->latitude = kget_float(mess);
			break;
		case NAME:
			strcpy(r->name, kget_char(mess, 255));
			break;
		case STREET:
			strcpy(r->street, kget_char(mess, 255));
			break;
		case TOWN:
			strcpy(r->town, kget_char(mess, 255));
			break;
		case ZIP_CODE:
			r->zip_code = kget_int(mess);
			break;
		case LOCALITY:
			strcpy(r->locality, kget_char(mess, 255));
			break;
		case E_MAIL:
			strcpy(r->e_mail, kget_char(mess, 255));
			break;
		case URL:
			strcpy(r->url, kget_char(mess, 255));
			break;
		case FOOD_TYPE:
			strcpy(r->food_type, kget_char(mess, 100));
			break;
		case WEEKLY_REST:
			r->weekly_rest = kget_int(" Weekly rest (0 -> Sun ... 6 -> Sat):");
			break;
		case VACATION_FROM:
			kget_day_month(" Start Vacation (dd/mm):", &r->vacation_from);
			break;
		case VACATION_TO:
			kget_day_month(" End Vacation (dd/mm):", &r->vacation_to);
			break;
		case PHONE:
			r->phone = kget_int(mess);
			break;
		case OBS:
			strcpy(r->obs, kget_char(mess, 500));
			break;
		}
	}
	restaurant_insert(r);
}

/** Menu option for delete a restaurant of the list 
 * \remarks the delete opration only takes place after the finding the restaurant
 * \see rest_find
 * \see restaurant_delete
 */
void menu_delete() {
	prestaurant_t r;
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_02_STR);
	printf(MENU_OPTION_SEP_STR);

	r = rest_find();
	if (r == NULL) {
		printf("\nNOT FOUND!!\n");
	} else {
		char *s;
		restaurant_print(r);
		s = kget_char("Do you realy whant to delete this restaurant (y/n)?", 2);
		if (strcmp(s, "y") == 0) {
			restaurant_delete(r);
		}

	}
}

/** Menu option for edit a restaurant in the list 
 * \remarks it allow the user to find a especific restaurant an them edit his data
 * \see rest_find
 */
void menu_edit() {
	prestaurant_t r;
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_03_STR);
	printf(MENU_OPTION_SEP_STR);

	r = rest_find();
	if (r == NULL) {
		printf("\nNOT FOUND!!\n");
	} else {
		char *s;
		restaurant_print(r);
		s = kget_char("Edit this restaurant (y/n)?", 2);
		if (strcmp(s, "y") == 0) {
			int i;
			char mess[80];

			do {
				for (i = LONGITUDE; i <= OBS; i++) {
					printf(" %5i -> %s\n", i, restaurant_get_field_name(i));
				}
				printf("    99 -> For exit \n");

				i = kget_int("Select the field to edit: ");
				sprintf(mess, "\n%s == ", restaurant_get_field_name(i));
				switch (i) {
				case LONGITUDE:
					r->longitude = kget_float(mess);
					break;
				case LATITUDE:
					r->latitude = kget_float(mess);
					break;
				case NAME:
					strcpy(r->name, kget_char(mess, 255));
					break;
				case STREET:
					strcpy(r->street, kget_char(mess, 255));
					break;
				case TOWN:
					strcpy(r->town, kget_char(mess, 255));
					break;
				case ZIP_CODE:
					r->zip_code = kget_int(mess);
					break;
				case LOCALITY:
					strcpy(r->locality, kget_char(mess, 255));
					break;
				case E_MAIL:
					strcpy(r->e_mail, kget_char(mess, 255));
					break;
				case URL:
					strcpy(r->url, kget_char(mess, 255));
					break;
				case FOOD_TYPE:
					strcpy(r->food_type, kget_char(mess, 100));
					break;
				case WEEKLY_REST:
					r->weekly_rest = kget_int(mess);
					break;
				case VACATION_FROM:
					kget_day_month("(dd/mm) ", &r->vacation_from);
					break;
				case VACATION_TO:
					kget_day_month("(dd/mm) ", &r->vacation_to);
					break;
				case PHONE:
					r->phone = kget_int(mess);
					break;
				case OBS:
					strcpy(r->obs, kget_char(mess, 500));
					break;
				}
			} while (i != 99);
		}
	}

}

/** Menu option for import from file a previous saved list of restaurants to the list 
 * \remarks 
 * 			-# the file [list_restaurants.dat] must exist in the executable file path \n
 *			-# the element will be added in the current list
 * \see restaurant_load
 */
void menu_import() {
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_04_STR);
	printf(MENU_OPTION_SEP_STR);
	puts("<Start>");
	restaurant_load();
	puts("<Done>");
}

/** Menu option for export to file the current list of restaurants
 * \remarks the file [list_restaurants.dat] will be overrided in the executable file path
 * \see restaurant_save
 */
void menu_export() {
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_05_STR);
	printf(MENU_OPTION_SEP_STR);
	puts("<Start>");
	restaurant_save();
	puts("<Done>");
}

/** Menu option to sort the current list of restaurants 
 * \see restaurant_list_sort
 */
void menu_sort() {
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_06_STR);
	printf(MENU_OPTION_SEP_STR);
	puts("<Start>");
	restaurant_list_sort();
	puts("<Done>");
}

/** Menu option to find restaurants in the list 
 * \see restaurant_find_all
 */
void menu_find() {
	int i;
	char *vlt;
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_07_STR);
	printf(MENU_OPTION_SEP_STR);

	for (i = ID; i <= OBS; i++) {
		printf(" %5i -> %s\n", i, restaurant_get_field_name(i));
	}
	i = kget_int("Select field to search :");
	printf("\n[%s] == ", restaurant_get_field_name(i));
	vlt = kget_char("", 500);

	restaurant_find_all(i, vlt);

}

/** Menu option to list all open restaurants 
 * \remarks a open restaurant is a restaurant that are not in vacations or week rest from the list
 * \see restaurant_list_all_open
 */
void menu_list_open() {
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_08_STR);
	printf(MENU_OPTION_SEP_STR);
	restaurant_list_all_open();

}

/** Menu option to list all restaurants from the list
 * \see restaurant_list_all
 */
void menu_list() {
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_09_STR);
	printf(MENU_OPTION_SEP_STR);
	restaurant_list_all();
}

/** Menu option to load from a GPS Points of interest file more than 10000 restaurants 
 * \note some data is random but the GPS, name and adress are real.\n
 * The POI(Points Of Interest) was from GIS Sapo Services in http://services.sapo.pt/Metadata/Service/GIS
 */
void menu_test() {
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_99_STR);
	printf(MENU_OPTION_SEP_STR);
	puts("<Start>");
	{
		const char filename[] = "poi.csv";
		int itm[4];
		FILE *file = fopen(filename, "r");
		if (file != NULL) {
			char line[9999];
			while (fgets(line, sizeof line, file) != NULL) {
				prestaurant_t r = restaurant_new();
				sscanf(line, "%f;%f;%[^;];%[^;];%[^;];%i;%[^;];%[^;];%[^;];%[^;];%[^;];\n", &r->longitude, &r->latitude,
						r->name, r->street, r->town, &r->zip_code, r->locality, r->e_mail, r->url, r->food_type, r->obs);

				if (strcmp(r->town, r->locality) == 0)
					strcpy(r->town, "");

				r->weekly_rest = get_random(0, 6);

				itm[0] = get_random(1, 28);
				itm[1] = get_random(1, 28);
				itm[2] = get_random(1, 12);
				itm[3] = get_random(1, 12);

				if (itm[0] < itm[1]) {
					r->vacation_from.tm_mday = itm[0];
					r->vacation_to.tm_mday = itm[1];
				} else {
					r->vacation_from.tm_mday = itm[1];
					r->vacation_to.tm_mday = itm[0];
				}
				if (itm[2] < itm[3]) {
					r->vacation_from.tm_mon = itm[2];
					r->vacation_to.tm_mon = itm[3];
				} else {
					r->vacation_from.tm_mon = itm[3];
					r->vacation_to.tm_mon = itm[2];
				}
				r->phone = get_random(12345678, 99999999);
				restaurant_insert(r);

				//restaurant_print(r);

				//if (ln++ > 5)
				//    break;


			}
			fclose(file);
		} else {
			perror(filename);
		}

	}
	puts("<Done>");
}


int main_menu() {
	int op;
	clear_screen();
	printf("REST GPS V0.1 - Augusto Campos\n");
	printf("************* Information ************\n");
	printf("  Longitude 	: %08.5f\n" ,user_longitude);
	printf("  Latitude  	: %08.5f\n", user_latitude);
	printf("  List Size 	: [%04i]\n", list_size(&list_restaurants));
	printf("  Week Day      : %s\n", day_of_week_text(today_day_of_week()));
	printf("*************** MENU ****************\n");
	printf(MENU_OPTION_01_STR);
	printf(MENU_OPTION_02_STR);
	printf(MENU_OPTION_03_STR);
	printf(MENU_OPTION_04_STR);
	printf(MENU_OPTION_05_STR);
	printf(MENU_OPTION_06_STR);
	printf(MENU_OPTION_07_STR);
	printf(MENU_OPTION_08_STR);
	printf(MENU_OPTION_09_STR);
	printf(MENU_OPTION_99_STR);
	printf(MENU_OPTION_SEP_STR);
	printf(MENU_OPTION_00_STR);
	printf(MENU_OPTION_SEP_STR);

	op = kget_int("Option: ");

	switch (op) {
	case 0:
		menu_exit();
		break;
	case 1:
		menu_insert();
		break;
	case 2:
		menu_delete();
		break;
	case 3:
		menu_edit();
		break;
	case 4:
		menu_import();
		break;
	case 5:
		menu_export();
		break;
	case 6:
		menu_sort();
		break;
	case 7:
		menu_find();
		break;
	case 8:
		menu_list_open();
		break;
	case 9:
		menu_list();
		break;
	case 99:
		menu_test();
		break;
	default:
		break;
	}
	pause();
	return op;
}
