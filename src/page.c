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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "path.h"

#include "book.h"
#include "stamp.h"
#include "page.h"

/* TODO: Agregar el gettext_noop */
const char *stamp_category_names[NUM_STAMP_TYPE] = {
	gettext_noop ("Activities"),
	gettext_noop ("Games"),
	gettext_noop ("Events"),
	gettext_noop ("Pins")
};

static BookPage *pages_list = NULL;

static void load_resources_from_pages (void) {
	BookPage *p;
	CPStamp *stamp;
	SDL_Surface * image;
	char buffer_file[8192];
	IndexItem *item;
	SDL_Color gris, gris2;
	
	gris.r = gris.g = gris.b = 0x5e;
	gris2.r = gris2.g = gris2.b = 0x66;
	
	p = pages_list;
	
	while (p != NULL) {
		if (p->tipo == PAGE_INDEX) {
			/* Cargar el fondo */
			sprintf (buffer_file, "%s/images/index/background_%i.png", systemdata_path, p->categoria);
			image = IMG_Load (buffer_file);
			
			p->index->background = image;
			
			sprintf (buffer_file, "%s/images/index/category_%i_big.png", systemdata_path, p->categoria);
			image = IMG_Load (buffer_file);
			
			p->index->icon = image;
			
			p->index->img_titulo = TTF_RenderUTF8_Blended (ttf30_burbank_bold, p->index->titulo, gris);
			
			/* Recorrer cada item del indice y cargar su pequeña miniatura */
			item = p->index->items;
			
			while (item != NULL) {
				sprintf (buffer_file, "%s/category_small.png", item->link->group_stamps->resource_dir);
				image = IMG_Load (buffer_file);
				
				item->icon = image;
				item->img_titulo = TTF_RenderUTF8_Blended (ttf12_burbank_bold, item->titulo, gris2);
				item = item->next;
			}
		} else if (p->tipo == PAGE_GAME) {
			/* Cargar el fondo */
			sprintf (buffer_file, "%s/background.png", p->group_stamps->resource_dir);
			image = IMG_Load (buffer_file);
			
			p->group_stamps->background = image;
			
			sprintf (buffer_file, "%s/category_big.png", p->group_stamps->resource_dir);
			image = IMG_Load (buffer_file);
			
			p->group_stamps->icon = image;
			
			/* Renderizar el título */
			p->group_stamps->img_titulo = TTF_RenderUTF8_Blended (ttf30_burbank_bold, p->group_stamps->titulo, gris);
			
			/* Cargar los archivos de cada estampa */
			stamp = p->group_stamps->lista;
			
			while (stamp != NULL) {
				sprintf (buffer_file, "%s/%i.png", p->group_stamps->resource_dir, stamp->id);
				image = IMG_Load (buffer_file);
				stamp->image = image;
				
				stamp = stamp->sig;
			}
		}
		
		p = p->next;
	}
}

static void append_page_to_current_index (BookPage *page_index, BookPage *page, CPStampGroup *group) {
	IndexItem *last_index_item;
	IndexItem *new;
	
	new = (IndexItem *) malloc (sizeof (IndexItem));
	
	/* Usar el locale domain de la categoria */
	new->titulo = dgettext (group->l10n_domain, group->titulo);
	
	new->link = page;
	new->next = NULL;
	
	if (page_index->index->items == NULL) {
		page_index->index->items = new;
	} else {
		last_index_item = page_index->index->items;
		
		while (last_index_item->next != NULL) {
			last_index_item = last_index_item->next;
		}
		
		last_index_item->next = new;
	}
}

static BookPage * agregar_index (int category, BookPage *last_page) {
	BookPage *new;
	
	new = (BookPage *) malloc (sizeof (BookPage));
	
	new->next = NULL;
	new->prev = last_page;
	
	if (last_page != NULL) {
		last_page->next = new;
	}
	
	new->tipo = PAGE_INDEX;
	new->categoria = category;
	new->group_stamps = NULL;
	
	new->index = (BookIndex *) malloc (sizeof (BookIndex));
	new->index->titulo = gettext (stamp_category_names[category]);
	
	new->index->items = NULL;
	
	return new;
}

void generate_pages (void) {
	CPStampGroup *listas[NUM_STAMP_TYPE];
	BookPage *last_page = NULL;
	BookPage *p;
	CPStampGroup *group;
	BookPage *full_index;
	BookPage *current_index;
	int g;
	
	memset (listas, 0, sizeof (listas));
	stamp_read_all_files (listas);
	
	/* Generar aquí el FULL INDEX */
	
	/* Por cada categoria que sí exista, generar índices */
	for (g = 0; g < NUM_STAMP_TYPE; g++) {
		if (listas[g] == NULL) continue;
		
		/* Primero agregar el índice */
		current_index = p = agregar_index (g, last_page);
		
		last_page = p;
		
		/* Ahora recorrer todas las categorias de esta lista */
		group = listas[g];
		
		while (group != NULL) {
			p = (BookPage *) malloc (sizeof (BookPage));
			
			p->next = NULL;
			p->prev = last_page;
			
			last_page->next = p;
			last_page = p;
			
			p->tipo = PAGE_GAME;
			p->group_stamps = group;
			p->categoria = g;
			
			append_page_to_current_index (current_index, p, group);
			
			group = group->sig;
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
