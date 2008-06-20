/**
 *      \file restaurant.c
 * 		\brief Implementation file for Restaurant related actions
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

#include "acdll.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "restaurant.h"
#include "utils.h"
#include "main.h"

/** Pointer type to a restaurant_seeker struct */
typedef struct restaurant_seeker_s restaurant_seeker_t;

/** Struct to be used in the search for a restaurant
 *  \see fn_seeker_restaurant
 */
struct restaurant_seeker_s {
	/** field to be seach */
	eRESTAURANTE_FIELDS field;
	/** Value to shearch for */
	char *value;
};

/** File mane for import and export Restaurants */
#define IMPORT_EXPORT_FILE_NAME "list_restaurants.dat"

/** auxiliar variable to store the next ID for the Restaurant List. */
unsigned int restaurant_index = 0;

/** Array of the fields names of the Restaurante Struct */
const char *restaurant_fields_names[] = { "ID", "LONGITUDE", "LATITUDE", "NAME", "STREET", "TOWN", "ZIP_CODE", "LOCALITY",
		"E_MAIL", "URL", "FOOD_TYPE", "WEEKLY_REST", "VACATIONS_FROM", "VACATIONS_TO", "PHONE", "OBS" };

/** Funtion Comparator for distance to user 
 * \param p1 pointer to Restaurant 1 
 * \param p2 pointer to Restaurant 2
 * \return 
 * 			- 0 	if distance to de user of p1 = p2
 * 			- -1	if distance to de user of p1 < p2
 * 			- 1		if distance to de user of p1 > p2
 */
int fn_comparator_restaurant_distance(const void *p1, const void *p2) {
	prestaurant_t r1 = (prestaurant_t) p1;
	prestaurant_t r2 = (prestaurant_t) p2;
	double d1 = distance(user_latitude, user_longitude, r1->latitude, r1->longitude);
	double d2 = distance(user_latitude, user_longitude, r2->latitude, r2->longitude);

	if (d1 < d2)
		return -1;
	if (d2 < d1)
		return 1;

	return 0;
}

/**
 * Function Seeker for opened restaurants 
 * \param el 		pointer to the element in the Restaurant List
 * \param indicator NOT USED
 * \return 1 if the restaurent is not in vacations or is the the week rest , otherwise 0.
 */
int fn_seeker_restaurant_open(const void *el, const void *indicator) {
	prestaurant_t r = ((prestaurant_t) el);
	struct tm *tmp;
	struct tm today;
	time_t timer;
	timer=time(NULL);
	int dt1, dt2, dt3;

	tmp = localtime(&timer);

	if (r->weekly_rest == today_day_of_week() )
		return 0;

	today.tm_mday = tmp->tm_mday +1;
	today.tm_mon = tmp->tm_mon+1;

	dt1 = r->vacation_from.tm_mon * 100 + r->vacation_from.tm_mday;
	dt2 = r->vacation_to.tm_mon * 100 + r->vacation_to.tm_mday;
	dt3 = today.tm_mon * 100 + today.tm_mday;

	if ( !(dt3 >= dt1 && dt3 <=dt2))
		return 1;

	return 0;
}

/**
 * Function Seeker for restaurants.
 * \param el 		pointer to the element in the Restaurant List.
 * \param indicator pointer to retaurant_seeker_t containing data for the search.
 */
int fn_seeker_restaurant(const void *el, const void *indicator) {

	prestaurant_t r = ((prestaurant_t) el);
	restaurant_seeker_t *value = ((restaurant_seeker_t *) indicator);

	switch (value->field) {
	case ID:
		return (r->id == atoi(value->value));
		break;
	case LONGITUDE:
		return (float_equal(r->longitude, (float) atof(value->value)));
		break;
	case LATITUDE:
		return (float_equal(r->latitude, (float) atof(value->value)));
		break;
	case NAME:
		return (!strcmp(r->name, value->value));
		break;
	case STREET:
		return (!strcmp(r->street, value->value));
		break;
	case TOWN:
		return (!strcmp(r->town, value->value));
		break;
	case ZIP_CODE:
		return (r->zip_code == atoi(value->value));
		break;
	case LOCALITY:
		return (!strcmp(r->locality, value->value));
		break;
	case E_MAIL:
		return (!strcmp(r->e_mail, value->value));
		break;
	case URL:
		return (!strcmp(r->url, value->value));
		break;
	case FOOD_TYPE:
		return (!strcmp(r->food_type, value->value));
		break;
	case WEEKLY_REST:
		return (r->weekly_rest == atoi(value->value));
		break;
	case VACATION_FROM:
		return 0;//(vacation_cmp(&r->vacation_from, (struct tm*)value->value));
		break;
	case VACATION_TO:
		return 0;//(vacation_cmp(&r->vacation_to, (struct tm*)(value->value)));
		break;
	case PHONE:
		return (r->phone == atoi(value->value));
		break;
	case OBS:
		return (!strcmp(r->obs, value->value));
		break;
	}

	return 0;
}

/** Get the size of elements int the restaurant List
 * \param el	pointer to the element in the Restaurant List
 * \return 		size of the Restaurent list element
 * \remarks 	Since it is user fixes String size all elements have the same size.	
 */
size_t fn_data_size_restaurant(const void *el) {
	return sizeof(struct restaurant_s);
}

/* set initial settings fot the list of restaurants */
void restaurant_init() {
	list_init(&list_restaurants);
	list_attributes_copy(&list_restaurants, fn_data_size_restaurant, 0);
	list_attributes_comparator(&list_restaurants, fn_comparator_restaurant_distance);
}

/* creates a new empty restaurant */
prestaurant_t restaurant_new() {
	prestaurant_t r = (prestaurant_t) malloc(sizeof(struct restaurant_s));
	if (!r) {
		perror("out of memory");
		return NULL;
	}

	/* First clear the result structure.  */
	memset(r, '\0', sizeof(*r));

	return r;

}

int restaurant_insert(prestaurant_t r) {
	r->id = restaurant_index++;
	return list_append(&list_restaurants, r);
}

void restaurant_delete(prestaurant_t r) {
	list_delete_at(&list_restaurants, r->id);
}

void restaurant_clear() {
	list_destroy(&list_restaurants);
}
prestaurant_t restaurant_find(eRESTAURANTE_FIELDS f, const char *v) {
	restaurant_seeker_t vl;
	vl.field = f;
	vl.value = (char*)v;

	list_attributes_seeker(&list_restaurants, fn_seeker_restaurant);

	return (prestaurant_t) list_seek(&list_restaurants, &vl);

}

void restaurant_find_all(eRESTAURANTE_FIELDS f, const char *v) {
	restaurant_seeker_t vl;
	vl.field = f;
	vl.value = (char*)v;

	printf("<START>\n");
	printf("ID   |Distance |Longitude|Latitude |Name                                    |%s\n", restaurant_get_field_name(f));

	restaurant_list_sort();

	list_iterator_start(&list_restaurants);
	while (list_iterator_hasnext(&list_restaurants)) {
		prestaurant_t r = (prestaurant_t) list_iterator_next(&list_restaurants);

		if (fn_seeker_restaurant(r, &vl) ) {

			printf("%5i|%09.4f|%09.4f|%09.4f|%-40s|", r->id, distance(user_latitude, user_longitude, r->latitude,
					r->longitude), r->longitude, r->latitude, (r->name == NULL ? "<null>" : r->name));

			switch (f) {
			case ID:
				printf("\n");
				break;
			case LONGITUDE:
				printf("\n");
				break;
			case LATITUDE:
				printf("\n");
				break;
			case NAME:
				printf("\n");
				break;
			case STREET:
				printf("%s\n", r->street);
				break;
			case TOWN:
				printf("%s\n", r->town);
				break;
			case ZIP_CODE:
				printf("%i\n", r->zip_code);
				break;
			case LOCALITY:
				printf("%s\n", r->locality);
				break;
			case E_MAIL:
				printf("%s\n", r->e_mail);
				break;
			case URL:
				printf("%s\n", r->url);
				break;
			case FOOD_TYPE:
				printf("%s\n", r->food_type);
				break;
			case WEEKLY_REST:
				printf("%s\n", day_of_week_text(r->weekly_rest));
				break;
			case VACATION_FROM:
				printf("%i/%i\n", r->vacation_from.tm_mday, r->vacation_from.tm_mon);
				break;
			case VACATION_TO:
				printf("%i/%i\n", r->vacation_to.tm_mday, r->vacation_to.tm_mon);
				break;
			case PHONE:
				printf("%i\n", r->phone);
				break;
			case OBS:
				printf("%s\n", r->obs);
				break;
			}
		}

	}
	list_iterator_stop(&list_restaurants);

	printf("<END>\n");
}

const char * restaurant_get_field_name(eRESTAURANTE_FIELDS f) {
	return restaurant_fields_names[f];
}

void restaurant_print(prestaurant_t r) {
	if (!r) {
		perror("Null Restaurant");
		return;
	}

	printf("\n");
	printf("ID		: %i\n", r->id);
	printf("LONGITUDE	: %f\n", r->longitude);
	printf("LATITUDE	: %f\n", r->latitude);
	printf("NAME		: %s\n", r->name);
	printf("ADRESS		: %s\n", r->street);
	printf("		: %s\n", r->town);
	printf("		: %i %s\n", r->zip_code, r->locality);
	printf("EMAIL		: %s\n", r->e_mail);
	printf("URL		: %s\n", r->url);
	printf("FOOD TYPE	: %s\n", r->food_type);
	printf("WEEKLY REST	: %s\n", day_of_week_text(r->weekly_rest));
	printf("VACATIONS	: %i/%i -> %i/%i\n", r->vacation_from.tm_mday, r->vacation_from.tm_mon, r->vacation_to.tm_mday,
			r->vacation_to.tm_mon);
	printf("PHONE		: %i\n", r->phone);
	printf("OBS		: %s\n", r->obs);
	printf("\n");
}

/**
 * Prints a restaurant in a tabular form.
 */
void restaurant_list_one(prestaurant_t r) {
	printf("%5i|%09.4f|%09.4f|%09.4f|%-40s|%-40s|%07i-%s\n", r->id, distance(user_latitude, user_longitude, r->latitude,
			r->longitude), r->longitude, r->latitude, (r->name == NULL ? "<null>" : r->name), (r->street == NULL ? "<null>"
			: r->street), r->zip_code, (r->locality == NULL ? "<null>" : r->locality));

}

void restaurant_list_all() {
	printf("<START>\n");
	printf("ID  |Distance|Longitude|Latitude|Name      |Street    |Zip-Code\n");

	list_attributes_seeker(&list_restaurants, NULL);

	list_iterator_start(&list_restaurants);
	while (list_iterator_hasnext(&list_restaurants)) {
		restaurant_list_one((prestaurant_t) list_iterator_next(&list_restaurants));
	}
	list_iterator_stop(&list_restaurants);

	printf("<END>\n");
}

void restaurant_list_all_open() {
	printf("<START>\n");
	printf("ID  |Distance|Longitude|Latitude|Name      |WR    |Vacation\n");

	restaurant_list_sort();

	list_iterator_start(&list_restaurants);
	while (list_iterator_hasnext(&list_restaurants)) {
		prestaurant_t r = (prestaurant_t) list_iterator_next(&list_restaurants);

		if (fn_seeker_restaurant_open(r, NULL) ) {

			printf("%5i|%09.4f|%09.4f|%09.4f|%-40s|%-4s|%i/%i -> %i/%i\n", r->id, distance(user_latitude, user_longitude,
					r->latitude, r->longitude), r->longitude, r->latitude, (r->name == NULL ? "<null>" : r->name),
					day_of_week_text(r->weekly_rest), r->vacation_from.tm_mday, r->vacation_from.tm_mon,
					r->vacation_to.tm_mday, r->vacation_to.tm_mon);
		}

	}
	list_iterator_stop(&list_restaurants);

	printf("<END>\n");
}

void restaurant_save() {
	list_dump_file(&list_restaurants, IMPORT_EXPORT_FILE_NAME);
}

void restaurant_load() {
	list_restore_file(&list_restaurants, IMPORT_EXPORT_FILE_NAME);
}

void restaurant_list_sort() {
	/* setting the custom comparator */
	list_attributes_comparator(&list_restaurants, fn_comparator_restaurant_distance);
	list_sort(&list_restaurants, -1);

}

