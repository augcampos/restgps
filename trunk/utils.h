/**
 *      \file utils.h
 *      \brief Heather file for util functions
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

#ifndef _UTILS_H
#define	_UTILS_H

#ifdef	__cplusplus
extern "C"
  {
#endif

#include <math.h>
#include <time.h>
#include <stdbool.h>

/**
 * Compare 2 floats
 * \param a to be compared
 * \param b to be compared
 * \return boolean
 */
#define float_equal(a,b) (fabs(a - b) <= 1.0e-20f)
/**
 * Calculates de distance between two GPS points
 * \param lat1  latitude of the first GPS point.
 * \param lon1  longitude of the first GPS point.
 * \param lat2  latitude of the second GPS point.
 * \param lon2  longitude of the second GPS point.
 * \return      distance in Km.
 * \see http://www.gpsvisualizer.com/calculators#distance_address
 */
double distance(float lat1, float lon1, float lat2, float lon2);

/**
 * Get chars from keyboard.
 * \param mess  message to display to the user.
 * \param max_count     max char that is taken in consideration.
 * \return      chars intruduced by the user.
 */ 
char *kget_char(const char* mess, int max_count);

/**
 * Get integer from keyboard
 * \param mess      message to display to the user.
 * \return      integer intruduced by the user.
 */ 
int kget_int(const char* mess);

/**  
 * Get float from keyboard
 * \param mess      message to display to the user.
 * \return       float intruduced by the user.
*/
float kget_float(const char* mess);

/**
 * Pause the application and wait for a key to be pressed.
 */
void pause();

/**
 * Clear the screen.
 */
void clear_screen();

/**
 * Get the number of the day in the week
 * \param timer Timer from where is to be take the day of week
 */
int day_of_week(const time_t* timer);

/**
 * Get the number of the day of week for current day.
 * \see day_of_week()
 */
int today_day_of_week();

/**
 * Get text for the day of the week
 * \param d     Number of the day of week.
 * \return      Description for the number of the day of week.
 * \see days_of_week
 */
const char *day_of_week_text(int d);

/**
 *  get day and month from keyboard
 * \param mess  message to display to the user.
 * \param date  place where to store the day and month.    
 */
void kget_day_month(const char *mess, struct tm* date);

/**
 * Get random integer
 * \param min   lower limit for the random number
 * \param max   upper limit for the random number
 * \return      randon integer.
 */
int get_random(int min,int max);

#ifdef	__cplusplus
}
#endif

#endif	/* _UTILS_H */

