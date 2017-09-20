/*
 * book.c
 * This file is part of stampbook
 *
 * Copyright (C) 2011 - Félix Arreola Rodríguez
 *
 * stampbook is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * stampbook is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with stampbook; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "path.h"

#include "draw-text.h"

#define FPS (1000/24)

/* Enumerar las imágenes */
enum {
	IMG_PORTADA_NEGRA,
	
	IMG_CLASP,
	
	NUM_IMAGES
};

/* Los nombres de archivos */
const char *images_names[NUM_IMAGES] = {
	"images/portada_negra.png",
	"images/clasp.png"
};

enum {
	//SND_NONE,
	
	NUM_SOUNDS
};

const char *sound_names[NUM_SOUNDS] = {
	//"sounds/none.wav",
};

/* Codigos de salida */
enum {
	GAME_NONE = 0, /* No usado */
	GAME_CONTINUE,
	GAME_QUIT
};

/* Prototipos de función */
int book_portada (void);
int game_loop (void);
int game_finish (void);
void setup (void);
SDL_Surface * set_video_mode(unsigned flags);

/* Variables globales */
SDL_Surface * screen;
SDL_Surface * images[NUM_IMAGES];
int use_sound;

Mix_Chunk * sounds[NUM_SOUNDS];
Mix_Music * mus_carnie;

TTF_Font *ttf54_burbank_bold, *ttf14_burbank_bold;

int main (int argc, char *argv[]) {
	/* Recuperar las rutas del sistema */
	initSystemPaths (argv[0]);
	
	/* Inicializar l18n */
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, l10n_path);
	
	textdomain (PACKAGE);
	
	setup ();
	
	read_all_files ();
	
	do {
		if (book_portada () == GAME_QUIT) break;
		/*if (game_loop () == GAME_QUIT) break;
		if (game_finish () == GAME_QUIT) break;*/
	} while (1 == 0);
	
	SDL_Quit ();
	return EXIT_SUCCESS;
}

int book_portada (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	SDL_Rect rect;
	Uint32 last_time, now_time;
	SDL_Surface *nick_color, *nick_negro, *nick_trans;
	SDL_Color negro, color_nick;
	
	/* Predibujar todo */
	SDL_FillRect (screen, NULL, SDL_MapRGB (screen->format, 255, 255, 255));
	
	/* Copiar el el color de fondo */
	rect.x = 47;
	rect.y = 68;
	rect.w = images[IMG_PORTADA_NEGRA]->w;
	rect.h = images[IMG_PORTADA_NEGRA]->h;
	
	SDL_BlitSurface (images[IMG_PORTADA_NEGRA], NULL, screen, &rect);
	
	/* Dibujar el botón */
	rect.x = 625;
	rect.y = 223;
	rect.w = images[IMG_CLASP]->w;
	rect.h = images[IMG_CLASP]->h;
	SDL_BlitSurface (images[IMG_CLASP], NULL, screen, &rect);
	
	const char *username = "Penguin888888";
	
	/* Base 127.05, 334.20
	 * 1.90, 2.9
	 * 3.80, 5.95 A = 30
	 * 1.90, 5.4  A = 30
	 * 0.15, 1.50 A = 20 Negro
	 * 0,    0    A = 20 Negro
	 * 1.4,  0.6  A = 20 Negro
	 * 2.75, 4.3  A = 30
	 */
	
	/* Rederizar el nombre 3 veces */
	negro.r = negro.g = negro.b = 0;
	color_nick.r = 0x38; color_nick.g = 0xA8; color_nick.b = 0x7D;
	
	nick_negro = TTF_RenderUTF8_Blended (ttf54_burbank_bold, username, negro);
	nick_color = TTF_RenderUTF8_Blended (ttf54_burbank_bold, username, color_nick);
	nick_trans = TTF_RenderUTF8_Blended (ttf54_burbank_bold, username, color_nick);
	
	apply_alpha (nick_color, 76);
	apply_alpha (nick_negro, 51);
	
	/* Renderizar todas las capas, de abajo hacia arriba */
	rect.x = 130;
	rect.y = 338;
	rect.w = nick_trans->w;
	rect.h = nick_trans->h;
	SDL_BlitSurface (nick_trans, NULL, screen, &rect);
	
	rect.x = 128;
	rect.y = 335;
	rect.w = nick_negro->w;
	rect.h = nick_negro->h;
	SDL_BlitSurface (nick_negro, NULL, screen, &rect);
	
	rect.x = 127;
	rect.y = 334;
	rect.w = nick_negro->w;
	rect.h = nick_negro->h;
	SDL_BlitSurface (nick_negro, NULL, screen, &rect);
	
	rect.x = 127;
	rect.y = 336;
	rect.w = nick_negro->w;
	rect.h = nick_negro->h;
	SDL_BlitSurface (nick_negro, NULL, screen, &rect);
	
	rect.x = 129;
	rect.y = 340;
	rect.w = nick_trans->w;
	rect.h = nick_trans->h;
	SDL_BlitSurface (nick_trans, NULL, screen, &rect);
	
	rect.x = 131;
	rect.y = 340;
	rect.w = nick_trans->w;
	rect.h = nick_trans->h;
	SDL_BlitSurface (nick_trans, NULL, screen, &rect);
	
	rect.x = 129;
	rect.y = 337;
	rect.w = nick_color->w;
	rect.h = nick_color->h;
	SDL_BlitSurface (nick_color, NULL, screen, &rect);
	
	SDL_FreeSurface (nick_color);
	SDL_FreeSurface (nick_negro);
	SDL_FreeSurface (nick_trans);
	
	const char *total_stamps = "Total Stamps 8888/8888";
	
	nick_negro = TTF_RenderUTF8_Blended (ttf14_burbank_bold, total_stamps, negro);
	nick_color = TTF_RenderUTF8_Blended (ttf14_burbank_bold, total_stamps, color_nick);
	nick_trans = TTF_RenderUTF8_Blended (ttf14_burbank_bold, total_stamps, color_nick);
	
	apply_alpha (nick_color, 76);
	apply_alpha (nick_negro, 51);
	
	/* B = 486.65, 401
	 * -184.9,  0.65
	 * -185.25, 1.3  A = 30 color
	 * -185.25, 0.65 A = 30 color
	 * -185.25, 1.3  A = 30 color
	 * -187.05, 0    A = 20 negro
	 * -185,75, 0    A = 20 negro
	 */
	
	rect.x = 301;
	rect.y = 401;
	rect.w = nick_negro->w;
	rect.h = nick_negro->h;
	SDL_BlitSurface (nick_negro, NULL, screen, &rect);
	
	rect.x = 300;
	rect.y = 401;
	rect.w = nick_negro->w;
	rect.h = nick_negro->h;
	SDL_BlitSurface (nick_negro, NULL, screen, &rect);
	
	rect.x = 301;
	rect.y = 402;
	rect.w = nick_trans->w;
	rect.h = nick_trans->h;
	SDL_BlitSurface (nick_trans, NULL, screen, &rect);
	
	rect.x = 301;
	rect.y = 401;
	rect.w = nick_trans->w;
	rect.h = nick_trans->h;
	SDL_BlitSurface (nick_trans, NULL, screen, &rect);
	
	rect.x = 301;
	rect.y = 402;
	rect.w = nick_trans->w;
	rect.h = nick_trans->h;
	SDL_BlitSurface (nick_trans, NULL, screen, &rect);
	
	rect.x = 302;
	rect.y = 402;
	rect.w = nick_color->w;
	rect.h = nick_color->h;
	SDL_BlitSurface (nick_color, NULL, screen, &rect);
	
	SDL_Flip (screen);
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					if (event.button.x >= 625 && event.button.x < 735 && event.button.y >= 223 && event.button.y < 312) {
						done = GAME_CONTINUE;
					}
					break;
				case SDL_KEYDOWN:
					/* Tengo una tecla presionada */
					key = event.key.keysym.sym;
					
					if (key == SDLK_F11 || (key == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))) {
						SDL_WM_ToggleFullScreen (screen);
					}
					if (key == SDLK_ESCAPE) {
						done = GAME_QUIT;
					}
					break;
			}
		}
		
		SDL_Flip (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	return done;
}

int game_finish (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	SDL_Rect rect;
	Uint32 last_time, now_time;
	
	/* Predibujar todo */
	SDL_FillRect (screen, NULL, 0);
	SDL_Flip (screen);
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			/* fprintf (stdout, "Evento: %i\n", event.type);*/
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					break;
				case SDL_KEYDOWN:
					/* Tengo una tecla presionada */
					key = event.key.keysym.sym;
					
					if (key == SDLK_F11 || (key == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))) {
						SDL_WM_ToggleFullScreen (screen);
					}
					if (key == SDLK_ESCAPE) {
						done = GAME_QUIT;
					}
					break;
			}
		}
		
		SDL_Flip (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	return done;
}

int game_loop (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	Uint32 last_time, now_time;
	SDL_Rect rect;
	
	/* Predibujar todo */
	SDL_FillRect (screen, NULL, 0);
	SDL_Flip (screen);
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEBUTTONDOWN:
					/* Tengo un Mouse Down */
					break;
				case SDL_MOUSEBUTTONUP:
					/* Tengo un mouse Up */
					break;
				case SDL_KEYDOWN:
					/* Tengo una tecla presionada */
					key = event.key.keysym.sym;
					
					if (key == SDLK_F11 || (key == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))) {
						SDL_WM_ToggleFullScreen (screen);
					}
					if (key == SDLK_ESCAPE) {
						done = GAME_QUIT;
					}
					break;
			}
		}
		
		SDL_Flip (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	return done;
}
/* Set video mode: */
/* Mattias Engdegard <f91-men@nada.kth.se> */
SDL_Surface * set_video_mode (unsigned flags) {
	/* Prefer 16bpp, but also prefer native modes to emulated 16bpp. */

	int depth;

	depth = SDL_VideoModeOK (760, 480, 16, flags);
	return depth ? SDL_SetVideoMode (760, 480, depth, flags) : NULL;
}

void setup (void) {
	SDL_Surface * image;
	TTF_Font *ttf10, *ttf14, *ttf16, *ttf26, *temp_font;
	SDL_Color color;
	SDL_Rect rect, rect2;
	int g;
	char buffer_file[8192];
	
	/* Inicializar el Video SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf (stderr,
			_("Error: Can't initialize the video subsystem\n"
			"The error returned by SDL is:\n"
			"%s\n"), SDL_GetError());
		exit (1);
	}
	sprintf (buffer_file, "%simages/icon.png", systemdata_path);
	image = IMG_Load (buffer_file);
	if (image) {
		SDL_WM_SetIcon (image, NULL);
		SDL_FreeSurface (image);
	}
	SDL_WM_SetCaption (_("stampbook"), _("stampbook"));
	
	/* Crear la pantalla de dibujado */
	screen = set_video_mode (0);
	
	if (screen == NULL) {
		fprintf (stderr,
			_("Error: Can't setup 760x480 video mode.\n"
			"The error returned by SDL is:\n"
			"%s\n"), SDL_GetError());
		exit (1);
	}
	
	use_sound = 1;
	if (SDL_InitSubSystem (SDL_INIT_AUDIO) < 0) {
		fprintf (stdout,
			_("Warning: Can't initialize the audio subsystem\n"
			"Continuing...\n"));
		use_sound = 0;
	}
	
	if (use_sound) {
		/* Inicializar el sonido */
		if (Mix_OpenAudio (22050, AUDIO_S16, 2, 4096) < 0) {
			fprintf (stdout,
				_("Warning: Can't initialize the SDL Mixer library\n"));
			use_sound = 0;
		}
	}
	
	for (g = 0; g < NUM_IMAGES; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, images_names[g]);
		image = IMG_Load (buffer_file);
		
		if (image == NULL) {
			fprintf (stderr,
				_("Failed to load data file:\n"
				"%s\n"
				"The error returned by SDL is:\n"
				"%s\n"), buffer_file, SDL_GetError());
			SDL_Quit ();
			exit (1);
		}
		
		images[g] = image;
		/* TODO: Mostrar la carga de porcentaje */
	}
	
	if (use_sound) {
		for (g = 0; g < NUM_SOUNDS; g++) {
			sprintf (buffer_file, "%s%s", systemdata_path, sound_names[g]);
			sounds[g] = Mix_LoadWAV (buffer_file);
			
			if (sounds[g] == NULL) {
				fprintf (stderr,
					_("Failed to load data file:\n"
					"%s\n"
					"The error returned by SDL is:\n"
					"%s\n"), buffer_file, SDL_GetError ());
				SDL_Quit ();
				exit (1);
			}
			Mix_VolumeChunk (sounds[g], MIX_MAX_VOLUME / 2);
		}
		
		/* Cargar la música */
		//sprintf (buffer_file, "%s%s", systemdata_path, MUS_CARNIE);
		//mus_carnie = Mix_LoadMUS (buffer_file);
		
		/*if (mus_carnie == NULL) {
			fprintf (stderr,
				_("Failed to load data file:\n"
				"%s\n"
				"The error returned by SDL is:\n"
				"%s\n"), buffer_file, SDL_GetError ());
			SDL_Quit ();
			exit (1);
		}*/
	}
	
	if (TTF_Init () < 0) {
		fprintf (stderr,
			_("Error: Can't initialize the SDL TTF library\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	sprintf (buffer_file, "%s%s", systemdata_path, "burbanksb.ttf");
	ttf54_burbank_bold = TTF_OpenFont (buffer_file, 54);
	
	if (!ttf54_burbank_bold) {
		fprintf (stderr,
			_("Failed to load font file 'Burbank Small Bold'\n"
			"The error returned by SDL is:\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	sprintf (buffer_file, "%s%s", systemdata_path, "burbanksb.ttf");
	ttf14_burbank_bold = TTF_OpenFont (buffer_file, 14);
	
	if (!ttf14_burbank_bold) {
		fprintf (stderr,
			_("Failed to load font file 'Burbank Small Bold'\n"
			"The error returned by SDL is:\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	/* Generador de números aleatorios */
	srand (SDL_GetTicks ());
}

