/**
 *      \file main.h
 * 		\brief Heather file of entry point of the executable.
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

#ifndef _MAIN_H
#define	_MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

/** Current user GPS longitude */
float user_longitude;
/** Current user GPS latitude */
float user_latitude;
/** Executable file path
 * \remarks usefull for file not found errors display
 */
char *exe_path;

#ifdef	__cplusplus
}
#endif

#endif	/* _MAIN_H */
