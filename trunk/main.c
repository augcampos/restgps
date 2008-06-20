/**
 * 		\file main.c
 * 		\brief Implementation file of entry point of the executable.
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
#include "main.h"
#include "restaurant.h"
#include "main_menu.h"
#include "utils.h"

/** Get user GPS position */
void get_user_gps_pos() {
	printf("User GSP Location\n");
	user_longitude = kget_float("Longitude :");
	user_latitude = kget_float("Latitude :");
}
/** Get user GPS position from file  */
int get_user_gps_pos_from_file() {
	FILE *fp;
	int n = 0;
	float vl;
	char str[80];

	fp = fopen("user.txt", "rt");
	if (!fp) {
		printf("File not fount :%s/%s .\n", exe_path, "user.txt");
		return -1;
	}
	while (!feof(fp)) {
		fgets(str, 80, fp);

		if (sscanf(str, "longitude=%f\n5", &vl) != 0) {
			user_longitude = vl;
			n++;
		}
		if (sscanf(str, "latitude=%f\n", &vl) != 0) {
			user_latitude = vl;
			n++;
		}
	}
	fclose(fp);
	return n;
}

/** Main entry function
 * \param argc	number of parameters inserted in command line
 * \param argv 	array of all parameters inserted in command line
 * \return 		0 in case of success; errorcode in case of an error
 */
int main(int argc, char** argv) {
	exe_path = argv[0];

	if (get_user_gps_pos_from_file() < 2)
		get_user_gps_pos();

	restaurant_init();
	while (main_menu() > 0) {
	}

	return (0);

}

