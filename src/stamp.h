/*
 * stamp.h
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

#ifndef __STAMP_H__
#define __STAMP_H__

#include <SDL.h>

/* Define las posibles categorías para la estampa */
enum {
	STAMP_TYPE_ACTIVITY = 0,
	STAMP_TYPE_GAME,
	STAMP_TYPE_EVENT,
	STAMP_TYPE_PIN,
	
	NUM_STAMP_TYPE
};

enum {
	STAMP_EASY = 0,
	STAMP_NORMAL,
	STAMP_HARD,
	STAMP_EXTREME
};

typedef struct _CPStamp {
	int id;
	char *titulo;
	char *descripcion;
	
	int categoria;
	int dificultad;
	
	int ganada;
	
	SDL_Surface *image;
	
	struct _CPStamp *sig;
} CPStamp;

typedef struct _CPStampCategory {
	char *nombre;
	int categoria;
	
	CPStamp *lista;
	int read_version;
	
	char *l10n_domain;
	char *l10n_dir;
	
	char *resource_dir;
	
	int total_stamps;
	int earned_stamps;
	
	/* Objetos SDL para desplegar la categoría */
	SDL_Surface *titulo;
	SDL_Surface *background_page;
	
	SDL_Surface *icon_big, *icon_small;
	
	struct _CPStampCategory *sig;
} CPStampCategory;

void stamp_read_all_files (CPStampCategory **listas);

#endif /* __STAMP_H__ */

