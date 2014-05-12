/**
 *
 * \file os.h
 *
 * Function declaration for OS.
 *
 * Copyright 2014 John Whitmore <jwhitmore@electronicsoup.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the version 2 of the GNU General Public License
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _OS_H
#define _OS_H

#define APP_INIT_INVALID   0x01
#define APP_MAIN_INVLAID   0x02
#define APP_ISR_INVALID    0x04

extern BOOL application_invalid;

extern void os_init_data(void);

#endif //_OS_H
