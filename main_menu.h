/**
 *      \file main_menu.h
 * 		\brief Heather file of main menu.
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


#ifndef _MAIN_MENU_H
#define	_MAIN_MENU_H

#ifdef	__cplusplus
extern "C" {
#endif

/** 
 * Show the main menu \n
 * This menu has: 
 *  -# the user GPS information
 *  -# the restaurant list sise information
 *  -# the current day description
 *  -# The main menu with the all the actions available from the restaurant list.
 * @return the options selected by the user
 */
int main_menu();

#ifdef	__cplusplus
}
#endif

#endif	/* _MAIN_MENU_H */

