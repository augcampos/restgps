/**
 *      \file restaurant.h
 * 		\brief Heather file for Restaurant related actions
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

#ifndef _RESTAURANT_H
#define	_RESTAURANT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "acdll.h"
#include <time.h>

/** Pointer to Structure Restaurant */
typedef struct restaurant_s* prestaurant_t;
/**Structure Restaurant */
struct restaurant_s {
	/** Indentification Number */
	unsigned int id;
	/** GPS Longitude */
	float longitude;
	/** GPS Latitude */
	float latitude;
	/** Name of the restaurant */
	char name[255];
	/** Street of the restaurant address*/
	char street[255];
	/** Town of the restaurant address*/
	char town[255];
	/** Zip Code of the restaurant address*/
	int zip_code;
	/** Locality of the restaurant address*/
	char locality[255];
	/** Email address of the restaurant*/
	char e_mail[255];
	/** URL for the site of the restaurant */
	char url[255];
	/** Brief description of the foof type for the restaurant */
	char food_type[100];
	/** Day of the week that the restaurant is close
	 * \see days_of_week 
	 */
	int weekly_rest;
	/** Day and Month of the start vacation of the restarurant 
	 * \remarks Only used the tm_mday and tm_mon of the struct tm.
	 */ 
	struct tm vacation_from;
	/** Day and Month of the end vacation of the restaurant 
	* \remarks Only used the tm_mday and tm_mon of the struct tm.
	*/ 
	struct tm vacation_to;
	/** Phone number of the restaurant */
	int phone;
	/** Observations for the restaurant */
	char obs[500];
};

/**  Enumerator for all the fields in the restaurant Struct */
typedef enum{
	ID,
	LONGITUDE,
	LATITUDE,
	NAME,
	STREET,
	TOWN,
	ZIP_CODE,
	LOCALITY,
	E_MAIL,
	URL,
	FOOD_TYPE,
	WEEKLY_REST,
	VACATION_FROM,
	VACATION_TO,
	PHONE,
	OBS
} eRESTAURANTE_FIELDS;

//globals
/** \brief Glogal Restaurant List 
 *  Double linked List were is storage, in memory all Restaurant.
 * \see list_t
 */
list_t list_restaurants;

//extern function
/**
 *  Initializes the Restaurant list.
 * \see list_init
 */
void restaurant_init();

/**
 * Creates a new Restaurant with a clean and initialized data.
 * \return pointer to the created restaurant
 * \note this functions DO NOT include the restaurant in the global Restaurants list
*/
prestaurant_t restaurant_new();

/** Get the field name for de Enum value.
 * \param f field position.
 * \return the field name
 * \see eRESTAURANTE_FIELDS
 * \see restaurant_fields_names
 */
const char *restaurant_get_field_name(eRESTAURANTE_FIELDS f);

/**
 * Print all information about the restaurant.
 * \param r pointer to the restaurant.
 */
void restaurant_print(prestaurant_t r);

/**
 * Prints a list with all restaurants from the Restaurant List
 * \see list_restaurants
 */
void restaurant_list_all();

/**
 * Put all elements from the Restaurant List in a especific order.
 */
void restaurant_list_sort();

/**
 * List all restaurants, from the Restaurant List that are not in vacations or in his weekly rest today.
 */
void restaurant_list_all_open();

/**
 * Insert a restaurant in the Restaurant List.
 * \param r pointer to the restaurant to be include in the list.
 * \return  0 if the attribute was successfully set; -1 otherwise
 * \pre the restaurant must be created before insert
 * \remarks the ID is automatic fill by this function.
 * \see list_append
*/ 
int restaurant_insert(prestaurant_t r);

/**
 * Removes a restaurant from the Restaurant List.
 * \param r pointer to the restaurant to be removed from the list.
 * \see list_delete_at
*/ 
void restaurant_delete(prestaurant_t r);

/**
 * Clear all restaurants from the Restaurant List
 * \see list_destroy
 */
void restaurant_clear();

/**
 * Finds a especific restaurant in the Restaurant List.
 * \param f field where to look for
 * \param v Value to look for
 * \return 	Pointer to the first found restaurant.
 */
prestaurant_t restaurant_find(eRESTAURANTE_FIELDS f, const char *v);

/**
 * Prints a list with all restaurants in the Restaurant List that have the field f iquals to v.
 * \param f field where to look for
 * \param v Value to look for
 */
void restaurant_find_all(eRESTAURANTE_FIELDS f, const char *v);

/**
 * Imports from file restaurants into the Restaurant List
 * \see IMPORT_EXPORT_FILE_NAME
 * \see list_restore_file
 */
void restaurant_load();

/**
 * Exports to file the Restaurant List
 * \see IMPORT_EXPORT_FILE_NAME
 * \see list_dump_file
 */
void restaurant_save();

#ifdef	__cplusplus
}
#endif

#endif	/* _RESTAURANT_H */

