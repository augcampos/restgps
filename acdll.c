/**
 *      \file acdll.c
 *		\brief Implementation file for the Double Linked List
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdint.h>

#include "acdll.h"

#ifndef ACDLL_MAX_SPARE_ELEMS
/** Length of alocator for new elemts */
#define ACDLL_MAX_SPARE_ELEMS        5
#endif



/** Minumum number of elements for sorting with quicksort instead of insertion */
#define ACDLL_MINQUICKSORTELS        24

/** List dump declarations \n
 *  Version of fileformat managed by _dump* and _restore* functions
 */
#define ACDLL_DUMPFORMAT_VERSION     1
/** Length of the header */
#define ACDLL_DUMPFORMAT_HEADERLEN   30

/** Header description for a list dump */
struct list_dump_header_s {
	/** version */
	uint16_t ver;
	/** dump timestamp */
	int64_t timestamp;
	/** random value terminator -- terminates the data sequence */
	int32_t rndterm;

	/** sum of every element' size, bytes */
	uint32_t totlistlen;
	/** number of elements */
	uint32_t numels;
	/** bytes length of an element, for constant-size lists, <= 0 otherwise */
	uint32_t elemlen;
	/** hash of the list at the time of dumping, or 0 if to be ignored */
	int32_t listhash;
};


static int list_drop_elem(list_t *l, struct list_entry_s *tmp, unsigned int pos);

static int list_attributes_setdefaults(list_t *l);

static void list_sort_quicksort(list_t *l, int versus, unsigned int first, struct list_entry_s *fel, unsigned int last,
		struct list_entry_s *lel);



/* list initialization */
int list_init(list_t *l) {
	if (l == NULL)
		return -1;

	srandom((unsigned long)time(NULL));

	l->numels = 0;

	/* head/tail sentinels and mid pointer */
	l->head_sentinel = (struct list_entry_s *)malloc(sizeof(struct list_entry_s));
	l->tail_sentinel = (struct list_entry_s *)malloc(sizeof(struct list_entry_s));
	l->head_sentinel->next = l->tail_sentinel;
	l->tail_sentinel->prev = l->head_sentinel;
	l->head_sentinel->prev = l->tail_sentinel->next = l->mid_sentinel = NULL;
	l->head_sentinel->data = l->tail_sentinel->data = NULL;

	/* iteration attributes */
	l->iter_active = 0;
	l->iter_pos = 0;
	l->iter_curentry = NULL;

	/* free-list attributes */
	l->spareels = (struct list_entry_s **)malloc(ACDLL_MAX_SPARE_ELEMS * sizeof(struct list_entry_s *));
	l->spareelsnum = 0;

	list_attributes_setdefaults(l);

	return 0;
}

void list_destroy(list_t *l) {
	unsigned int i;

	list_clear(l);
	for (i = 0; i < l->spareelsnum; i++) {
		free(l->spareels[i]);
	}
	free(l->spareels);
	free(l->head_sentinel);
	free(l->tail_sentinel);
}

/** set default values for initialized lists 
 * \param l     list to operate
 */
int list_attributes_setdefaults(list_t *l) {
	l->attrs.comparator = NULL;
	l->attrs.seeker = NULL;

	/* also free() element data when removing and element from the list */
	l->attrs.meter = NULL;

	/* serializer/unserializer */
	l->attrs.serializer = NULL;
	l->attrs.unserializer = NULL;

	return 0;
}

/* setting list properties */
int list_attributes_comparator(list_t *l, element_comparator comparator_fun) {
	if (l == NULL)
		return -1;

	l->attrs.comparator = comparator_fun;
	return 0;
}

int list_attributes_seeker(list_t *l, element_seeker seeker_fun) {
	if (l == NULL)
		return -1;

	l->attrs.seeker = seeker_fun;
	return 0;
}

int list_attributes_copy(list_t *l, element_meter metric_fun, int copy_data) {
	if (l == NULL || (metric_fun == NULL && copy_data != 0))
		return -1;

	l->attrs.meter = metric_fun;

	return 0;
}

int list_append(list_t *l, const void *data) {
	return list_insert_at(l, data, l->numels);
}

/** 
 * Set tmp to point to element at index posstart in l 
 * \param l     	list to operate
 * \param posstart  start position
 * */
static inline struct list_entry_s *list_findpos(const list_t *l, int posstart) {
	struct list_entry_s *ptr;
	float x;
	int i;

	/* accept 1 slot overflow for fetching head and tail sentinels */
	if (posstart < -1 || posstart > (int)l->numels)
		return NULL;

	x = (float)(posstart+1) / l->numels;
	if (x <= 0.25) {
		/* first quarter: get to posstart from head */
		for (i = -1, ptr = l->head_sentinel; i < posstart; ptr = ptr->next, i++)
			;
	} else if (x < 0.5) {
		/* second quarter: get to posstart from mid */
		for (i = (l->numels-1)/2, ptr = l->mid_sentinel; i > posstart; ptr = ptr->prev, i--)
			;
	} else if (x <= 0.75) {
		/* third quarter: get to posstart from mid */
		for (i = (l->numels-1)/2, ptr = l->mid_sentinel; i < posstart; ptr = ptr->next, i++)
			;
	} else {
		/* fourth quarter: get to posstart from tail */
		for (i = l->numels, ptr = l->tail_sentinel; i > posstart; ptr = ptr->prev, i--)
			;
	}

	return ptr;
}

int list_insert_at(list_t *l, const void *data, unsigned int pos) {
	struct list_entry_s *lent, *succ, *prec;

	if (l->iter_active || pos > l->numels)
		return -1;

	/* this code optimizes malloc() with a free-list */
	if (l->spareelsnum > 0) {
		lent = l->spareels[l->spareelsnum-1];
		l->spareelsnum--;
	} else {
		lent = (struct list_entry_s *)malloc(sizeof(struct list_entry_s));
		if (lent == NULL)
			return -1;
	}

	lent->data = (void*)data;

	/* actually append element */
	prec = list_findpos(l, pos-1);
	succ = prec->next;

	prec->next = lent;
	lent->prev = prec;
	lent->next = succ;
	succ->prev = lent;

	l->numels++;

	/* fix mid pointer */
	if (l->numels == 1) { /* first element, set pointer */
		l->mid_sentinel = lent;
	} else if (l->numels % 2) { /* now odd */
		if (pos >= (l->numels-1)/2)
			l->mid_sentinel = l->mid_sentinel->next;
	} else { /* now even */
		if (pos <= (l->numels-1)/2)
			l->mid_sentinel = l->mid_sentinel->prev;
	}

	return 1;
}

int list_delete_at(list_t *l, unsigned int pos) {
	struct list_entry_s *delendo;

	if (l->iter_active || pos >= l->numels)
		return -1;

	delendo = list_findpos(l, pos);

	list_drop_elem(l, delendo, pos);

	l->numels--;

	return 0;
}

int list_clear(list_t *l) {
	struct list_entry_s *s;

	if (l->iter_active)
		return -1;

	/* spare a loop conditional with two loops: spareing elems and freeing elems */
	for (s = l->head_sentinel->next; l->spareelsnum < ACDLL_MAX_SPARE_ELEMS && s != l->tail_sentinel; s = s->next) {
		/* move elements as spares as long as there is room */
		l->spareels[l->spareelsnum++] = s;
	}
	while (s != l->tail_sentinel) {
		/* free the remaining elems */
		s = s->next;
		free(s->prev);
	}
	l->head_sentinel->next = l->tail_sentinel;
	l->tail_sentinel->prev = l->head_sentinel;

	l->numels = 0;
	l->mid_sentinel = NULL;

	return 0;
}

unsigned int list_size(const list_t *l) {
	return l->numels;
}

const void *list_seek(const list_t *l, void *indicator) {
	const struct list_entry_s *iter;

	if (l->attrs.seeker == NULL)
		return NULL;

	for (iter = l->head_sentinel->next; iter != l->tail_sentinel; iter = iter->next) {
		if (l->attrs.seeker(iter->data, indicator) != 0)
			return iter->data;
	}

	return NULL;
}

int list_sort(list_t *l, int versus) {
	if (l->iter_active || l->attrs.comparator == NULL) /* cannot modify list in the middle of an iteration */
		return -1;

	if (l->numels <= 1)
		return 0;
	list_sort_quicksort(l, versus, 0, l->head_sentinel->next, l->numels-1, l->tail_sentinel->prev);
	return 0;
}

static inline void list_sort_selectionsort(list_t *l, int versus, unsigned int first, struct list_entry_s *fel,
		unsigned int last, struct list_entry_s *lel) {
	struct list_entry_s *cursor, *toswap, *firstunsorted;
	void *tmpdata;

	if (last <= first) /* <= 1-element lists are always sorted */
		return;

	for (firstunsorted = fel; firstunsorted != lel; firstunsorted = firstunsorted->next) {
		/* find min or max in the remainder of the list */
		for (toswap = firstunsorted, cursor = firstunsorted->next; cursor != lel->next; cursor = cursor->next)
			if (l->attrs.comparator(toswap->data, cursor->data) * -versus > 0)
				toswap = cursor;
		if (toswap != firstunsorted) { /* swap firstunsorted with toswap */
			tmpdata = firstunsorted->data;
			firstunsorted->data = toswap->data;
			toswap->data = tmpdata;
		}
	}
}

/**
 * start an iteration session.
 *
 * This function prepares the list to be iterated.
 *
 * \param l     	list to operate
 * \param versus 	- positive: order small to big(Ascendent); 
 * 					- negative: order big to small(Descendent)
 * \param first		fist index position
 * \param fel		first element
 * \param last		last index position
 * \param lel		last element
 *
 * \see list_sort
 */
static void list_sort_quicksort(list_t *l, int versus, unsigned int first, struct list_entry_s *fel, unsigned int last,
		struct list_entry_s *lel) {
	unsigned int pivotid;
	unsigned int i;
	register struct list_entry_s *pivot;
	struct list_entry_s *left, *right;
	void *tmpdata;

	if (last <= first) /* <= 1-element lists are always sorted */
		return;

	if (last - first+1 <= ACDLL_MINQUICKSORTELS) {
		list_sort_selectionsort(l, versus, first, fel, last, lel);
		return;
	}

	/* base of iteration: one element list */
	if (! (last > first))
		return;

	pivotid = (random() % (last - first + 1));
	/* pivotid = (last - first + 1) / 2; */

	/* find pivot */
	if (pivotid < (last - first + 1)/2) {
		for (i = 0, pivot = fel; i < pivotid; pivot = pivot->next, i++)
			;
	} else {
		for (i = last - first, pivot = lel; i > pivotid; pivot = pivot->prev, i--)
			;
	}

	/* smaller PIVOT bigger */
	left = fel;
	right = lel;
	/* iterate     --- left ---> PIV <--- right --- */
	while (left != pivot && right != pivot) {
		for (; left != pivot && (l->attrs.comparator(left->data, pivot->data) * -versus <= 0); left = left->next)
			;
		/* left points to a smaller element, or to pivot */
		for (; right != pivot && (l->attrs.comparator(right->data, pivot->data) * -versus >= 0); right = right->prev)
			;
		/* right points to a bigger element, or to pivot */
		if (left != pivot && right != pivot) {
			/* swap, then move iterators */
			tmpdata = left->data;
			left->data = right->data;
			right->data = tmpdata;

			left = left->next;
			right = right->prev;
		}
	}

	/* now either left points to pivot (end run), or right */
	if (right == pivot) { /* left part longer */
		while (left != pivot) {
			if (l->attrs.comparator(left->data, pivot->data) * -versus > 0) {
				tmpdata = left->data;
				left->data = pivot->prev->data;
				pivot->prev->data = pivot->data;
				pivot->data = tmpdata;
				pivot = pivot->prev;
				pivotid--;
				if (pivot == left)
					break;
			} else {
				left = left->next;
			}
		}
	} else { /* right part longer */
		while (right != pivot) {
			if (l->attrs.comparator(right->data, pivot->data) * -versus < 0) {
				/* move current right before pivot */
				tmpdata = right->data;
				right->data = pivot->next->data;
				pivot->next->data = pivot->data;
				pivot->data = tmpdata;
				pivot = pivot->next;
				pivotid++;
				if (pivot == right)
					break;
			} else {
				right = right->prev;
			}
		}
	}

	/* sort sublists A and B :       |---A---| pivot |---B---| */

	if (pivotid > 0)
		list_sort_quicksort(l, versus, first, fel, first+pivotid-1, pivot->prev);
	if (first + pivotid < last)
		list_sort_quicksort(l, versus, first+pivotid+1, pivot->next, last, lel);

}

int list_iterator_start(list_t *l) {
	if (l->iter_active)
		return 0;
	l->iter_pos = 0;
	l->iter_active = 1;
	l->iter_curentry = l->head_sentinel->next;
	return 1;
}

void *list_iterator_next(list_t *l) {
	void *toret;

	if (!l->iter_active)
		return NULL;

	toret = l->iter_curentry->data;
	l->iter_curentry = l->iter_curentry->next;
	l->iter_pos++;

	return toret;
}

int list_iterator_hasnext(const list_t *l) {
	if (!l->iter_active)
		return 0;
	return (l->iter_pos < l->numels);
}

int list_iterator_stop(list_t *l) {
	if (!l->iter_active)
		return 0;
	l->iter_pos = 0;
	l->iter_active = 0;
	return 1;
}


/**
 * Dump the heather descriptor to a file name.
 * \param l     list to operate
 * \param fd    file discriptor
 * \return the number of bytes read into memory
 *
 * \par Dump Format
 * [ ver   timestamp   |  totlen   numels  elemlen     hash    |   DATA ] \n
 * where DATA can be: \n
 * -# for constant-size list (element size is constant; elemlen > 0) \n
 * [ elem    elem    ...    elem ] \n
 * -# for other lists (element size dictated by element_meter each time; elemlen <= 0) \n
 * [ size elem     size elem       ...     size elem ]
 *
 * \remarks all integers are encoded in long long
 */
size_t list_dump_filedescriptor(const list_t *l, int fd) {
	struct list_entry_s *x;
	void *ser_buf;
	uint32_t bufsize;
	struct timeval timeofday;
	struct list_dump_header_s header;

	if (l->attrs.meter == NULL && l->attrs.serializer == NULL)
		return 0;


	/* prepare HEADER */
	/* version */
	header.ver = htons( ACDLL_DUMPFORMAT_VERSION);

	/* timestamp */
	gettimeofday(&timeofday, NULL);
	header.timestamp = (int64_t)timeofday.tv_sec * 1000000 + (int64_t)timeofday.tv_usec;
	header.timestamp = (long long)(header.timestamp);

	header.rndterm = htonl((int32_t)random());

	/* total list size is postprocessed afterwards */

	/* number of elements */
	header.numels = htonl(l->numels);

	header.listhash = htonl(0);

	header.totlistlen = header.elemlen = 0;

	/* leave room for the header at the beginning of the file */
	lseek(fd, ACDLL_DUMPFORMAT_HEADERLEN, SEEK_SET);

	/* write CONTENT */
	if (l->numels > 0) {
		/* SPECULATE that the list has constant element size */

		if (l->attrs.serializer != NULL) { /* user user-specified serializer */
			ser_buf = l->attrs.serializer(l->head_sentinel->next->data, &header.elemlen);
			free(ser_buf);
			/* request custom serialization of each element */
			for (x = l->head_sentinel->next; x != l->tail_sentinel; x = x->next) {
				ser_buf = l->attrs.serializer(x->data, &bufsize);
				header.totlistlen += bufsize;
				if (header.elemlen != 0) { /* continue on speculation */
					if (header.elemlen != bufsize) {
						free(ser_buf);
						/* constant element length speculation broken! */
						header.elemlen = 0;
						header.totlistlen = 0;
						x = l->head_sentinel;
						/* restart from the beginning */
						continue;
					}
					/* speculation confirmed */
					write(fd, ser_buf, bufsize);
				} else { /* speculation found broken */
					write(fd, &bufsize, sizeof(size_t));
					write(fd, ser_buf, bufsize);
				}
				free(ser_buf);
			}
		} else if (l->attrs.meter != NULL) {
			header.elemlen = (uint32_t)l->attrs.meter(l->head_sentinel->next->data);

			/* serialize the element straight from its data */
			for (x = l->head_sentinel->next; x != l->tail_sentinel; x = x->next) {
				bufsize = l->attrs.meter(x->data);
				header.totlistlen += bufsize;
				if (header.elemlen != 0) {
					if (header.elemlen != bufsize) {
						/* constant element length speculation broken! */
						header.elemlen = 0;
						header.totlistlen = 0;
						x = l->head_sentinel;
						/* restart from the beginning */
						continue;
					}
					write(fd, x->data, bufsize);
				} else {
					write(fd, &bufsize, sizeof(size_t));
					write(fd, x->data, bufsize);
				}
			}
		}
		/* adjust endianness */
		header.elemlen = htonl(header.elemlen);
		header.totlistlen = htonl(header.totlistlen);
	}

	/* write random terminator */
	write(fd, &header.rndterm, sizeof(header.rndterm)); /* list terminator */

	/* write header */
	lseek(fd, 0, SEEK_SET);

	write(fd, &header.ver, sizeof(header.ver)); /* version */
	write(fd, &header.timestamp, sizeof(header.timestamp)); /* timestamp */
	write(fd, &header.rndterm, sizeof(header.rndterm)); /* random terminator */

	write(fd, &header.totlistlen, sizeof(header.totlistlen)); /* total length of elements */
	write(fd, &header.numels, sizeof(header.numels)); /* number of elements */
	write(fd, &header.elemlen, sizeof(header.elemlen)); /* size of each element, or 0 for independent */
	write(fd, &header.listhash, sizeof(header.listhash)); /* list hash, or 0 for "ignore" */

	return ntohl(header.totlistlen);
}

/**
 * Read the heather descriptor to a file name.
 * \param l     list to operate
 * \param fd    file discriptor
 * \return the number of bytes read into memory
 *
 * \see list_dump_filedescriptor
 */
size_t list_restore_filedescriptor(list_t *l, int fd) {
	struct list_dump_header_s header;
	unsigned long cnt;
	void *buf;
	uint32_t elsize, totreadlen, totmemorylen;

	memset(&header, 0, sizeof(header));

	/* read header */

	/* version */
	if (read(fd, &header.ver, sizeof(header.ver)) != sizeof(header.ver))
		return 0;
	header.ver = ntohs(header.ver);
	if (header.ver != ACDLL_DUMPFORMAT_VERSION) {
		errno = EILSEQ;
		return 0;
	}

	/* timestamp */
	if (read(fd, &header.timestamp, sizeof(header.timestamp)) != sizeof(header.timestamp))
		return 0;

	/* list terminator */
	if (read(fd, &header.rndterm, sizeof(header.rndterm)) != sizeof(header.rndterm))
		return 0;

	header.rndterm = ntohl(header.rndterm);

	/* total list size */
	if (read(fd, &header.totlistlen, sizeof(header.totlistlen)) != sizeof(header.totlistlen))
		return 0;
	header.totlistlen = ntohl(header.totlistlen);

	/* number of elements */
	if (read(fd, &header.numels, sizeof(header.numels)) != sizeof(header.numels))
		return 0;
	header.numels = ntohl(header.numels);

	/* length of every element, or '0' = variable */
	if (read(fd, &header.elemlen, sizeof(header.elemlen)) != sizeof(header.elemlen))
		return 0;
	header.elemlen = ntohl(header.elemlen);

	/* list hash, or 0 = 'ignore' */
	if (read(fd, &header.listhash, sizeof(header.listhash)) != sizeof(header.listhash))
		return 0;
	header.listhash = ntohl(header.listhash);

	/* read content */
	totreadlen = totmemorylen = 0;
	if (header.elemlen > 0) {
		/* elements have constant size = header.elemlen */
		if (l->attrs.unserializer != NULL) {
			/* use unserializer */
			buf = malloc(header.elemlen);
			for (cnt = 0; cnt < header.numels; cnt++) {
				if (read(fd, buf, header.elemlen) != (ssize_t)header.elemlen) {
					errno = EPROTO;
					return 0;
				}
				list_append(l, l->attrs.unserializer(buf, &elsize));
				totmemorylen += elsize;
			}
		} else {
			/* copy verbatim into memory */
			for (cnt = 0; cnt < header.numels; cnt++) {
				buf = malloc(header.elemlen);
				if (read(fd, buf, header.elemlen) != (ssize_t)header.elemlen) {
					errno = EPROTO;
					return 0;
				}
				list_append(l, buf);
			}
			totmemorylen = header.numels * header.elemlen;
		}
		totreadlen = header.numels * header.elemlen;
	} else {
		/* elements have variable size. Each element is preceded by its size */
		if (l->attrs.unserializer != NULL) {
			/* use unserializer */
			for (cnt = 0; cnt < header.numels; cnt++) {
				read(fd, &elsize, sizeof(elsize));
				buf = malloc((size_t)elsize);
				if (read(fd, buf, elsize) != (ssize_t)elsize) {
					errno = EPROTO;
					return 0;
				}
				totreadlen += elsize;
				list_append(l, l->attrs.unserializer(buf, &elsize));
				totmemorylen += elsize;
			}
		} else {
			/* copy verbatim into memory */
			for (cnt = 0; cnt < header.numels; cnt++) {
				read(fd, &elsize, sizeof(elsize));
				buf = malloc(elsize);
				if (read(fd, buf, elsize) != (ssize_t)elsize) {
					errno = EPROTO;
					return 0;
				}
				totreadlen += elsize;
				list_append(l, buf);
			}
			totmemorylen = totreadlen;
		}
	}

	read(fd, &elsize, sizeof(elsize)); /* read list terminator */
	elsize = ntohl(elsize);

	/* possibly verify the list consistency */
	/* wrt hash */
	/* don't do that
	 if (header.listhash != 0 && header.listhash != list_hash(l)) {
	 errno = ECANCELED;
	 return 0;
	 }
	 */

	/* wrt header */
	if (totreadlen != header.totlistlen && (int32_t)elsize == header.rndterm) {
		errno = EPROTO;
		return 0;
	}

	/* wrt file */
	if (lseek(fd, 0, SEEK_CUR) != lseek(fd, 0, SEEK_END)) {
		errno = EPROTO;
		return 0;
	}

	return totmemorylen;
}

size_t list_dump_file(const list_t *l, const char *filename) {
	int fd;
	size_t sizetoret;

	fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0) {
		/* preserve the errno value left by fopen() */
		return (size_t)0;
	}

	sizetoret = list_dump_filedescriptor(l, fd);
	close(fd);

	return sizetoret;
}

size_t list_restore_file(list_t *l, const char *filename) {
	int fd;
	size_t totdata;

	fd = open(filename, O_RDONLY, 0);
	if (fd < 0)
		return 0;

	totdata = list_restore_filedescriptor(l, fd);
	close(fd);

	return totdata;
}

/** 
 * Deletes tmp from list, with care write its position (head, tail, middle) 
 * \param l     list to operate
 * \param tmp   pointer to temporary element
 * \param pos   [0,size-1] position index
 */
static int list_drop_elem(list_t *l, struct list_entry_s *tmp, unsigned int pos) {
	if (tmp == NULL)
		return -1;

	/* fix mid pointer */
	if (l->numels % 2) { /* now odd */
		if (pos >= l->numels/2)
			l->mid_sentinel = l->mid_sentinel->prev;
	} else { /* now even */
		if (pos < l->numels/2)
			l->mid_sentinel = l->mid_sentinel->next;
	}

	tmp->prev->next = tmp->next;
	tmp->next->prev = tmp->prev;

	if (l->spareelsnum < ACDLL_MAX_SPARE_ELEMS) {
		l->spareels[l->spareelsnum++] = tmp;
	} else {
		free(tmp);
	}

	return 0;
}

