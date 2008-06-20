/**
 * 		\file acdll.h
 * 		\brief Heather file for de Double Linked List
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

#ifndef ACDLL_H
#define ACDLL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <sys/types.h>


/**
 * \brief A comparator of elements.
 *
 * A comparator of elements is a function that:
 *      -# receives two references to elements a and b
 *      -# returns {<0, 0, >0} if (a > b), (a == b), (a < b) respectively
 *
 * \remarks It is responsability of the function to handle possible NULL values.
 */
typedef int (*element_comparator)(const void *a, const void *b);

/**
 * \brief A seeker of elements.
 *
 * An element seeker is a function that:
 *      -# receives a reference to an element el
 *      -# receives a reference to some indicator data
 *      -# returns non-0 if the element matches the indicator, 0 otherwise
 *
 * \remarks It is responsibility of the function to handle possible NULL values in any
 * argument.
 */
typedef int (*element_seeker)(const void *el, const void *indicator);

/**
 * \brief An element length meter.
 *
 * An element meter is a function that:
 *      -# receives the reference to an element el
 *      -# returns its size in bytes
 *
 * \remarks It is responsibility of the function to handle possible NULL values.
 */
typedef size_t (*element_meter)(const void *el);

/**
 * \brief A serializer of element.
 *
 * A serializer function is one that gets a reference to an element,
 * and returns a reference to a buffer that contains its serialization
 * along with the length of this buffer.
 * It is responsability of the function to handle possible NULL values,
 * returning a NULL buffer and a 0 buffer length.
 *
 * These functions have 3 goals:
 *  -# "freeze" and "flatten" the memory representation of the element
 *  -# provide a portable (wrt byte order, or type size) representation of the element, if the dump can be used on different sw/hw combinations
 *  -# possibly extract a compressed representation of the element
 *
 * \param el                reference to the element data
 * \param serialize_buffer  reference to fill with the length of the buffer
 * \return                  reference to the buffer with the serialized data
 */
typedef void *(*element_serializer)(const void *el, unsigned int *serializ_len);

/**
 * \brief A un-serializer of element.
 *
 * An unserializer function accomplishes the inverse operation of the
 * serializer function.  An unserializer function is one that gets a
 * serialized representation of an element and turns it backe to the original
 * element. The serialized representation is passed as a reference to a buffer
 * with its data, and the function allocates and returns the buffer containing
 * the original element, and it sets the length of this buffer into the
 * integer passed by reference.
 *
 * \param data              reference to the buffer with the serialized representation of the element
 * \param data_len          reference to the location where to store the length of the data in the buffer returned
 * \return                  reference to a buffer with the original, unserialized representation of the element
 */
typedef void *(*element_unserializer)(const void *data, unsigned int *data_len);

/** 
 * \brief Type defenition for struct list_s 
 * \see list_s
 * */
typedef struct list_s list_t;

/** Double Linked List service references
 * \note [private-use]
 */
struct list_entry_s {
	/** Element list data pointer */
	void *data;
	/** Next element pointer */
	struct list_entry_s *next;
	/** Privious element pointer */
	struct list_entry_s *prev;
};

/** Double Linked List attributes
 * \note [private-use]
 */
struct list_attributes_s {
	/** User-set routine for comparing list elements */
	element_comparator comparator;
	/** User-set routing for seeking elements */
	element_seeker seeker;
	/** User-set routine for determining the length of an element */
	element_meter meter;
	/** User-set routine for serializing an element */
	element_serializer serializer;
	/** User-set routine for unserializing an element */
	element_unserializer unserializer;
};

/** Double Linked List object */
struct list_s {
	/** Pointer to the Head element */
	struct list_entry_s *head_sentinel;
	/** Pointer to the Tail element */
	struct list_entry_s *tail_sentinel;
	/** Pointer to the Midle element */
	struct list_entry_s *mid_sentinel;
	/** Number of elements */
	unsigned int numels;

	/** Pointer to array of spare elements */
	struct list_entry_s **spareels;
	/** Number of elements in spareels */
	unsigned int spareelsnum;

	/** True if an list iteration is active(on-going) */
	int iter_active;
	/** Position of list iteration active*/
	unsigned int iter_pos;
	/** Pointer to the current list iteration */
	struct list_entry_s *iter_curentry;

	/** List attributes */
	struct list_attributes_s attrs;
};

/**
 * Initialize a list object for use.
 *
 * \param l     must point to a user-provided memory location
 * \return      0 for success. -1 for failure
 */
int list_init(list_t *l);

/**
 * Completely remove the list from memory.
 *
 * \remarks This function is the inverse of list_init(). It is meant to be called when
 * the list is no longer going to be used. Elements and possible memory taken
 * for internal use are freed.
 *
 * \param l     list to destroy
 */
void list_destroy(list_t *l);

/**
 * Set the comparator function for list elements.
 *
 * \remarks Comparator functions are used for searching and sorting. \n
 *          If NULL is passed as reference to the function, the comparator is disabled.
 *
 * \param l     list to operate
 * \param comparator_fun    pointer to the actual comparator function
 * \return      0 if the attribute was successfully set; -1 otherwise
 *
 * \see element_comparator()
 */
int list_attributes_comparator(list_t *l, element_comparator comparator_fun);

/**
 * Set a seeker function for list elements.
 *
 * \remarks Seeker functions are used for finding elements. \n
 * 			If NULL is passed as reference to the function, the seeker is disabled.
 *
 * \param l     list to operate
 * \param seeker_fun    pointer to the actual seeker function
 * \return      0 if the attribute was successfully set; -1 otherwise
 *
 * \see element_seeker()
 */
int list_attributes_seeker(list_t *l, element_seeker seeker_fun);

/**
 * Require to free element data when list entry is removed (default: don't free).
 *
 * \remark
 * -# By default, when an element is removed from the list, it disappears from
 * the list by its actual data is not free()d. With this option, every
 * deletion causes element data to be freed.
 * -# It is responsability of this function to correctly handle NULL values, if
 * NULL elements are inserted into the list.
 *
 * \param l             list to operate
 * \param metric_fun    pointer to the actual metric function
 * \param copy_data     0: do not free element data (default); non-0: do free
 * \return          0 if the attribute was successfully set; -1 otherwise
 *
 * \see element_meter()
 */
int list_attributes_copy(list_t *l, element_meter metric_fun, int copy_data);


/**
 * Append data at the end of the list.
 *
 * \param l     list to operate
 * \param data  pointer to user data to append
 *
 * \return      1 for success. < 0 for failure
 */
int list_append(list_t *l, const void *data);


/**
 * Insert an element at a given position.
 *
 * \param l     list to operate
 * \param data  reference to data to be inserted
 * \param pos   [0,size-1] position index to insert the element at
 * \return      positive value on success. Negative on failure
 */
int list_insert_at(list_t *l, const void *data, unsigned int pos);

/**
 * Delete an element at a given position from the list.
 *
 * \param l     list to operate
 * \param pos   [0,size-1] position index of the element to be deleted
 * \return      0 on success. Negative value on failure
 */
int list_delete_at(list_t *l, unsigned int pos);

/**
 * Clear all the elements off of the list.
 *
 * \remarks The element data will not be freed.
 *
 * \see list_size()
 *
 * \param l     list to operate
 * \return      the number of elements in the list before cleaning
 */
int list_clear(list_t *l);

/**
 * Inspect the number of elements in the list.
 *
 * \param l     list to operate
 * \return      number of elements currently held by the list
 */
unsigned int list_size(const list_t *l);

/**
 * Returns an element given an indicator.
 *
 * \warning Requires a seeker function to be set for the list.
 *
 * Inspect the given list looking with the seeker if an element matches
 * an indicator. If such element is found, the reference to the element
 * is returned.
 *
 * \param l     list to operate
 * \param indicator indicator data to pass to the seeker along with elements
 * \return      reference to the element accepted by the seeker, or NULL if none found
 */
const void *list_seek(const list_t *l, void *indicator);


/**
 * Sort list elements.
 *
 * \warning Requires a comparator function to be set for the list.
 *
 * Sorts the list in ascending or descending order as specified by the versus
 * flag. The algorithm chooses autonomously what algorithm is best suited for
 * sorting the list wrt its current status.
 *
 * \param l     list to operate
 * \param versus 	- positive: order small to big (Asdendent); 
 * 					- negative: order big to small(Descendent)
 * \return      	- 0: sorting went OK     
 * 					- non-0: errors happened
 *
 * \see list_attributes_comparator()
 */
int list_sort(list_t *l, int versus);

/**
 * Start an iteration session.
 *
 * This function prepares the list to be iterated.
 *
 * \param l     list to operate
 * \return 		0 if the list cannot be currently iterated. >0 otherwise
 *
 * \see list_iterator_stop()
 */
int list_iterator_start(list_t *l);

/**
 * Get the next element in the iteration session.
 *
 * \param l     list to operate
 * \return		element datum, or NULL on errors
 */
void *list_iterator_next(list_t *l);

/**
 * Returns true if more elements are available in the iteration session.
 *
 * \param l     list to operate
 * \return      0 if no more elements are available.
 */
int list_iterator_hasnext(const list_t *l);

/**
 * End an iteration session.
 *
 * \param l     list to operate
 * \return      0 if the iteration session cannot be stopped
 */
int list_iterator_stop(list_t *l);

/**
 * Dump the list to a file name.
 * \param l     list to operate
 * \param filename  filename to read data from
 * \return the number of bytes read into memory
 *
 * \remarks This function creates a filename and dumps the current content of the list
 * to it. If the file exists it is overwritten. The number of bytes written to
 * the file are returned.
 *
 * \see element_serializer()
 * \see list_restore_file()
 *
 * \note This function stores a representation of the list
 */
size_t list_dump_file(const list_t *l, const char *filename);

/**
 * Restore the list from a file name.
 *
 * This function restores the content of a list from a file into memory. It is
 * the inverse of list_dump_file().
 *
 * \see element_unserializer()
 * \see list_dump_file()
 * \see list_restore_filedescriptor()
 *
 * \param l         list to restore to
 * \param filename  filename to read data from
 * \return          the number of bytes read into memory
 */
size_t list_restore_file(list_t *l, const char *filename);




#ifdef __cplusplus
}
#endif

#endif
