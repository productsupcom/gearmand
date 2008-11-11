/* Gearman server and library
 * Copyright (C) 2008 Brian Aker, Eric Day
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __GEARMAN_TYPES_H__
#define __GEARMAN_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gearman_st gearman_st;
typedef struct gearman_client_st gearman_client_st;
typedef struct gearman_command_info_st gearman_command_info_st;
typedef struct gearman_con_st gearman_con_st;
typedef struct gearman_job_st gearman_job_st;
typedef struct gearman_packet_st gearman_packet_st;
typedef struct gearman_worker_st gearman_worker_st;

typedef uint8_t* (*gearman_worker_function)(gearman_worker_st *job,
                                            uint8_t *value,
                                            size_t value_length,
                                            size_t *result_length,
                                            gearman_return *error);

#if 0
typedef struct gearman_result_st gearman_result_st;
typedef struct gearman_byte_array_st gearman_byte_array_st;
typedef struct gearman_server_list_st gearman_server_list_st;
typedef struct gearman_server_st gearman_server_st;
typedef struct giov_st giov_st;

#endif

#ifdef __cplusplus
}
#endif

#endif /* __GEARMAN_TYPES_H__ */
