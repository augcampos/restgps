/**
 *      \file utils.c
 *      \brief Implementation file for util functions
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

#include "utils.h"

#define LINE_CHARS 128

/**
 * Description of the days of week
 */
const char days_of_week[7][4]= { "Sun", "Mon", "Thu", "Wed", "Thu", "Fri", "Sat" };

double distance(float lat1, float lon1, float lat2, float lon2) {
	double d_lat1 = (double) (lat1 / 57.29578);
	double d_lon1 = (double) (lon1 / 57.29578);
	double d_lat2 = (double) (lat2 / 57.29578);
	double d_lon2 = (double) (lon2 / 57.29578);

	return acos(sin(d_lat1) * sin(d_lat2) + cos(d_lat1) * cos(d_lat2) * cos(d_lon2 - d_lon1)) * 6371;
}

char *kget_char(const char* mess, int max_count) {
	char buffer[LINE_CHARS];
	char *readline = (char *) calloc(max_count, sizeof(char));
	if (!readline) {
		perror("Out of memory");
		return NULL;
	}
	
	printf(mess);
	bzero(buffer, sizeof(buffer));
	fgets(buffer, sizeof(buffer), stdin);
	sscanf(buffer, "%[^\n]", buffer);

	return strncpy(readline,buffer,max_count);
}

int kget_int(const char* mess) {
	char buffer[LINE_CHARS];
	int rt = 0;
	int somenum;

	do {
		printf(mess);
		bzero(buffer, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		rt = sscanf(buffer, "%i", &somenum);
		if (rt <= 0)
			printf("Error: Invalid Int!\n");
		else
			break;
		
	} while(true);

	return somenum;


}

/** Replace chars in a string 
 * \param str	string where to reolace the chars
 * \param from	char to find
 * \param to	char to be replaced with
 */
void replace_char(char *str, char from, char to) {
	for (; *str; ++str)
		if (*str == from)
			*str = to;
}

float kget_float(const char* mess) {
	char buffer[LINE_CHARS];
	int rt = 0;
	float somenum;

	do {
		printf(mess);
		bzero(buffer, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		rt = sscanf(buffer, "%f", &somenum);
		if (rt <= 0)
			printf("Error: Invalid Float point number!\n");
		else
			break;
		
	} while(true);

	return somenum;
}

void pause() {
	puts("Press ENTER key to continue...");
	getchar();
}

void clear_screen() {
	system("clear");
}

int day_of_week(const time_t* timer) {
	struct tm *tmp = localtime(timer);

	return tmp->tm_wday;
}

int today_day_of_week() {
	time_t timer = time(NULL);
	return day_of_week(&timer);
}

const char *day_of_week_text(int d) {
	return days_of_week[d];
}

void kget_day_month(const char *mess, struct tm* date) {
	char buffer[LINE_CHARS];
	int rt = 0;
	int day;
	int month;

	do {
		printf(mess);
		bzero(buffer, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		rt = sscanf(buffer, "%i/%i", &day,&month);
		if (rt <= 0)
			printf("Error: Invalid Day/Month!\n");
		else
			break;
		
	} while(true);

	date->tm_mday = day;
	date->tm_mon = month;
}

int get_random(int min,int max){
	return  (rand() % max + min);
}
