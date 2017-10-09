/*
 * page.h
 * This file is part of Stampbook
 *
 * Copyright (C) 2017 - Félix Arreola Rodríguez
 *
 * Stampbook is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Stampbook is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stampbook; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef __PAGE_H__
#define __PAGE_H__

#include "stamp.h"

enum {
	PAGE_FULL_INDEX = 1,
	PAGE_INDEX,
	PAGE_GAME
};

typedef struct _BookPage {
	int tipo;
	
	int categoria;
	CPStampCategory *category_obj;
	
	struct _BookPage *prev, *next;
} BookPage;

void generate_pages (void);
BookPage *get_pages (void);


#endif /* __PAGE_H__ */

