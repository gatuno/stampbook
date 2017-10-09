/*
 * cp-button.h
 * This file is part of Thin Ice
 *
 * Copyright (C) 2017 - Félix Arreola Rodríguez
 *
 * Thin Ice is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Thin Ice is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Thin Ice; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef __CP_BUTTON_H__
#define __CP_BUTTON_H__

extern int *cp_button_frames;
extern int *cp_button_refresh;

void cp_registrar_botones (int n);
void cp_registrar_boton (int n, int frame);
void cp_button_start (void);
void cp_button_motion (int map);
void cp_button_down (int map);
int cp_button_up (int map);

#endif /* __CP_BUTTON_H__ */

