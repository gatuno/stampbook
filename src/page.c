/*
 * page.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_image.h>

#include "path.h"

#include "stamp.h"
#include "page.h"

static BookPage *pages_list = NULL;

static void load_resources_from_pages (void) {
	BookPage *p;
	CPStamp *stamp;
	SDL_Surface * image;
	char buffer_file[8192];
	
	p = pages_list;
	
	while (p != NULL) {
		if (p->tipo == PAGE_GAME) {
			/* Cargar los archivos de cada estampa */
			stamp = p->category_obj->lista;
			
			while (stamp != NULL) {
				sprintf (buffer_file, "%s/%i.png", systemdata_path, stamp->id);
				image = IMG_Load (buffer_file);
				stamp->image = image;
				
				stamp = stamp->sig;
			}
		}
		
		p = p->next;
	}
}

void generate_pages (void) {
	CPStampCategory *listas[NUM_STAMP_TYPE];
	BookPage *last_page = NULL;
	BookPage *p;
	CPStampCategory *cat;
	
	int g;
	
	memset (listas, 0, sizeof (listas));
	stamp_read_all_files (listas);
	
	/* Por cada categoria, generar índices */
	for (g = 0; g < NUM_STAMP_TYPE; g++) {
		if (listas[g] == NULL) continue;
		
		/* Primero agregar el índice */
		p = (BookPage *) malloc (sizeof (BookPage));
		
		p->next = NULL;
		p->prev = last_page;
		
		if (last_page != NULL) {
			last_page->next = p;
		}
		
		last_page = p;
		
		p->tipo = PAGE_INDEX;
		p->categoria = g;
		
		/* Ahora recorrer todas las categorias de esta lista */
		cat = listas[g];
		
		while (cat != NULL) {
			p = (BookPage *) malloc (sizeof (BookPage));
			
			p->next = NULL;
			p->prev = last_page;
			
			last_page->next = p;
			last_page = p;
			
			p->tipo = PAGE_GAME;
			p->category_obj = cat;
			
			cat = cat->sig;
		}
	}
	
	p = NULL;
	if (last_page != NULL) {
		p = last_page;
		while (p->prev != NULL) {
			p = p->prev;
		}
	}
	
	pages_list = p;
	
	load_resources_from_pages ();
}

BookPage *get_pages (void) {
	return pages_list;
}

/*
-355.45, -115.0
419.45,  214.25
-39.4    24.45
-380.6,  -241.45
-0.05,   -0.05
375.95,  236.00


-----------------
19.9     118.2


Para 54:
-0.45, -101.35
*/
