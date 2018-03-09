/*
 * stamp.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "stamp.h"
#include "path.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

static int compare_stamp (const void *a, const void *b) {
	const CPStamp *left = (const CPStamp *) a;
	const CPStamp *right = (const CPStamp *) b;
	
	if (left->dificultad < right->dificultad) {
		return -1;
	} else if (left->dificultad > right->dificultad) {
		return 1;
	} else {
		if (left->id < right->id) {
			return -1;
		} else if (left->id > right->id) {
			return 1;
		} else {
			return 0;
		}
	}
}
static void sort_stamps (CPStampGroup *group) {
	int total = 0, g;
	CPStamp **lista, *p;
	
	p = group->lista;
	
	while (p != NULL) {
		total++;
		p = p->sig;
	}
	
	if (total <= 1) {
		return;
	}
	
	lista = (CPStamp **) malloc (sizeof (CPStamp *) * total);
	
	p = group->lista;
	
	g = 0;
	while (p != NULL) {
		lista[g] = p;
		p = p->sig;
		g++;
	}
	
	qsort (lista, total, sizeof (CPStamp *), compare_stamp);
	
	/* Religar la lista */
	group->lista = lista[0];
	
	g = 0;
	p = group->lista;
	while (g < total - 1) {
		p->sig = lista[g + 1];
		p = lista[g + 1];
		g++;
	}
	
	p->sig = NULL;
}

CPStampGroup *read_file (char *file) {
	int fd;
	CPStampGroup *abierta;
	uint32_t temp, version;
	int n_stampas;
	int res;
	char buf[4096];
	CPStamp *s, *last;
	int g;
	
	fd = open (file, O_RDONLY);
	
	if (fd < 0) {
		return NULL;
	}
	
	abierta = (CPStampGroup *) malloc (sizeof (CPStampGroup));
	abierta->total_stamps = 0;
	abierta->earned_stamps = 0;
	
	if (abierta == NULL) {
		goto error_close;
	}
	
	/* Leer el número de versión */
	if (read (fd, &version, sizeof (uint32_t)) == 0) {
		/* El archivo no existe, así que se ignora y se crea la estructura */
		goto error_free;
	}
	
	if (version == 0) {
		/* No podemos trabajar con la versión 0 de las estampas */
		goto error_free;
	}
	
	abierta->read_version = version;
	
	if (read (fd, &temp, sizeof (uint32_t)) == 0) {
		/* Se llegó al fin de archivo, ignorar y retornar la estructura */
		goto error_free;
	}
	
	n_stampas = temp;
	
	/* Leer la categoria general */
	res = read (fd, &temp, sizeof (uint32_t));
	if (res < 0 || temp >= NUM_STAMP_TYPE) {
		/* Ignorar, poner la categoría por default */
		goto error_free;
	}
	
	abierta->categoria = temp;
	
	/* Leer el nombre de las estampas */
	res = read (fd, &temp, sizeof (uint32_t));
	if (res <= 0) {
		/* Error en la lectura del archivo, ignorar */
		goto error_free;
	}
	
	if (temp < sizeof (buf)) { /* Leer solo si tenemos espacio */
		res = read (fd, buf, temp * sizeof (char));
		
		if (buf[0] != 0) {
			abierta->titulo = strdup (buf);
		} else {
			abierta->titulo = strdup ("");
		}
	} else {
		/* Brincar el nombre, de igual forma, no debería ser tan largo */
		lseek (fd, temp, SEEK_CUR);
		abierta->titulo = strdup ("");
	}
	
	/* Leer el dominio de traducción */
	res = read (fd, &temp, sizeof (uint32_t));
	if (res <= 0) {
		/* Error en la lectura del archivo, ignorar */
		goto error_free1;
	}
	
	if (temp < sizeof (buf)) { /* Leer solo si tenemos espacio */
		res = read (fd, buf, temp * sizeof (char));
		
		if (buf[0] != 0) {
			abierta->l10n_domain = strdup (buf);
		} else {
			abierta->l10n_domain = strdup ("");
		}
	} else {
		/* Brincar el nombre del dominio, de igual forma, no debería ser tan largo */
		lseek (fd, temp, SEEK_CUR);
		abierta->l10n_domain = strdup ("");
	}
	
	/* Leer el nombre de directorio de l10n */
	res = read (fd, &temp, sizeof (uint32_t));
	if (res <= 0) {
		/* Error en la lectura del archivo, ignorar */
		goto error_free2;
	}
	
	if (temp < sizeof (buf)) { /* Leer solo si tenemos espacio */
		res = read (fd, buf, temp * sizeof (char));
		
		if (buf[0] != 0) {
			abierta->l10n_dir = strdup (buf);
		} else {
			abierta->l10n_dir = strdup ("");
		}
	} else {
		/* Brincar el nombre del l10n_dir, de igual forma, no debería ser tan largo */
		lseek (fd, temp, SEEK_CUR);
		abierta->l10n_dir = strdup ("");
	}
	
	/* Leer el nombre de directorio de recursos de estampas */
	res = read (fd, &temp, sizeof (uint32_t));
	if (res <= 0) {
		/* Error en la lectura del archivo, ignorar */
		goto error_free3;
	}
	
	if (temp < sizeof (buf)) { /* Leer solo si tenemos espacio */
		res = read (fd, buf, temp * sizeof (char));
		
		if (buf[0] != 0) {
			abierta->resource_dir = strdup (buf);
		} else {
			goto error_free3;
		}
	} else {
		/* Si no logramos leer el directorio de recursos, abortar */
		goto error_free3;
	}
	
	/* Ahora leer todas las estampas */
	for (g = 0; g < n_stampas; g++) {
		s = (CPStamp *) malloc (sizeof (CPStamp));
		
		if (s == NULL) {
			return abierta;
		}
		s->sig = NULL;
		s->descripcion = NULL;
		
		/* Leer el id de la estampa */
		res = read (fd, &temp, sizeof (uint32_t));
		if (res <= 0) {
			/* Error en la lectura del archivo, ignorar la estampa y salir */
			free (s);
			return abierta;
		}
		
		s->id = temp;
		
		temp = 0;
		res = read (fd, &temp, sizeof (uint32_t));
		if (res <= 0 || temp > 255) {
			/* Error en la lectura del archivo, ignorar la estampa y salir 
			 * o Cadena de texto demasiado larga */
			free (s);
			return abierta;
		}
		
		res = read (fd, buf, temp * sizeof (char));
		
		if (res < temp) {
			/* Hemos leído menos bytes que los esperados */
			free (s);
			return abierta;
		}
		
		buf[temp] = 0; /* Fin de cadena */
		
		s->titulo = strdup (buf);
		
		temp = 0;
		res = read (fd, &temp, sizeof (uint32_t));
		
		if (res <= 0) {
			/* Error en la lectura, ignorar la estampa y salir */
			free (s->titulo);
			free (s);
			return abierta;
		}
		
		if (temp < sizeof (buf)) {
			res = read (fd, buf, temp * sizeof (char));
			
			if (res < temp) {
				/* Hemos leido menos bytes que los esperados */
				free (s->titulo);
				free (s);
				return abierta;
			}
			
			buf[temp] = 0;
			
			s->descripcion = strdup (buf);
		} else {
			/* No tengo espacio para leer la descripción, es muy larga */
			lseek (fd, temp, SEEK_CUR);
			s->descripcion = strdup ("");
		}
		
		res = read (fd, &temp, sizeof (uint32_t));
		if (res < 0 || temp >= NUM_STAMP_TYPE) {
			/* Error de lectura 
			 * o dato inválido */
			free (s->titulo);
			free (s->descripcion);
			free (s);
			return abierta;
		}
		
		s->categoria = temp;
		
		res = read (fd, &temp, sizeof (uint32_t));
		if (res < 0 || temp > STAMP_EXTREME) {
			/* Error de lectura 
			 * o dato inválido */
			free (s->titulo);
			free (s->descripcion);
			free (s);
			return abierta;
		}
		
		s->dificultad = temp;
		
		res = read (fd, &temp, sizeof (uint32_t));
		if (res < 0) {
			/* Error de lectura */
			free (s->titulo);
			free (s->descripcion);
			free (s);
			return abierta;
		}
		
		s->ganada = (temp != FALSE) ? TRUE : FALSE;
		
		if (g == 0) {
			abierta->lista = s;
		} else {
			last->sig = s;
		}
		
		abierta->total_stamps++;
		if (s->ganada) abierta->earned_stamps++;
		
		last = s;
	}
	
	return abierta;

error_free3:
	free (abierta->l10n_dir);
	
error_free2:
	free (abierta->l10n_domain);
	
error_free1:
	free (abierta->titulo);
	
error_free:
	free (abierta);
	
error_close:
	close (fd);
	
	return NULL;
}

CPStampGroup *append_group_to_list (CPStampGroup *inicio, CPStampGroup *elemento) {
	CPStampGroup *p;
	
	elemento->sig = NULL;
	
	if (inicio == NULL) {
		/* Nueva lista */
		return elemento;
	}
	
	p = inicio;
	while (inicio->sig != NULL) {
		inicio = inicio->sig;
	}
	
	inicio->sig = elemento;
	
	return inicio;
}

static int compare_stamp_group (const void *a, const void *b) {
	const CPStampGroup *left = (const CPStampGroup *) a;
	const CPStampGroup *right = (const CPStampGroup *) b;
	
	return strcmp (left->titulo, right->titulo);
}

static void sort_group_list (CPStampGroup **lista) {
	int total, g;
	CPStampGroup **arr, *p;
	
	total = 0;
	p = *lista;
	while (p != NULL) {
		total++;
		p = p->sig;
	}
	
	if (total <= 1) return;
	
	arr = (CPStampGroup **) malloc (sizeof (CPStampGroup *) * total);
	
	p = *lista;
	g = 0;
	while (p != NULL) {
		arr[g] = p;
		p = p->sig;
		g++;
	}
	
	qsort (arr, total, sizeof (CPStampGroup *), compare_stamp_group);
	
	/* Religar toda la lista */
	*lista = arr[0];
	
	g = 0;
	p = arr[0];
	while (g < total - 1) {
		p->sig = arr[g + 1];
		p = arr[g + 1];
		g++;
	}
	
	p->sig = NULL;
}

void stamp_read_all_files (CPStampGroup **listas) {
	/* Crear listas ligadas por cada "tipo" */
	CPStampGroup *read;
	char buf[4096];
	DIR *stamp_dir;
	struct dirent *objeto;
	int g;
	
	memset (listas, 0, sizeof (listas));
	
	sprintf (buf, "%s/.cpstamps/", userdata_path);
	stamp_dir = opendir (buf);
	
	if (stamp_dir == NULL) {
		/* Error al abrir el directorio */
		return;
	}
	
	while (objeto = readdir (stamp_dir), objeto != NULL) {
		sprintf (buf, "%s/.cpstamps/%s", userdata_path, objeto->d_name);
		
		if (file_exists (buf)) {
			read = read_file (buf);
			
			if (read == NULL) {
				continue;
			}
			
			/* Configurar el bindtextdomain para estas estampas */
			if (read->l10n_domain != NULL &&
			    read->l10n_domain[0] != 0 &&
			    read->l10n_dir != NULL &&
			    read->l10n_dir[0] != 0) {
				bindtextdomain (read->l10n_domain, read->l10n_dir);
			}
			
			/* Reacomodar el título para que se ordene bien */
			read->titulo = dgettext (read->l10n_domain, read->titulo);
			
			sort_stamps (read);
			
			/* Agregar a la lista ligada local */
			listas[read->categoria] = append_group_to_list (listas[read->categoria], read);
		}
	}
	
	closedir (stamp_dir);
	
	/* Reordenar cada lista de categorías */
	g = 0;
	while (g < NUM_STAMP_TYPE) {
		if (listas[g] != NULL) {
			sort_group_list (&listas[g]);
		}
		g++;
	}
}

