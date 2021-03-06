/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Gearmand client and server library.
 *
 *  Copyright (C) 2011-2013 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2008 Brian Aker, Eric Day
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

/**
 * Error codes.
 */
enum gearmand_error_t
{
  GEARMAND_SUCCESS,
  GEARMAND_IO_WAIT,
  GEARMAND_SHUTDOWN,
  GEARMAND_SHUTDOWN_GRACEFUL,
  GEARMAND_ERRNO,
  GEARMAND_EVENT,
  GEARMAND_TOO_MANY_ARGS,
  GEARMAND_NO_ACTIVE_FDS,
  GEARMAND_INVALID_MAGIC,
  GEARMAND_INVALID_COMMAND,
  GEARMAND_INVALID_PACKET,
  GEARMAND_UNEXPECTED_PACKET,
  GEARMAND_GETADDRINFO,
  GEARMAND_NO_SERVERS,
  GEARMAND_LOST_CONNECTION, // Valid for worker return
  GEARMAND_MEMORY_ALLOCATION_FAILURE,
  GEARMAND_JOB_EXISTS,
  GEARMAND_JOB_QUEUE_FULL,
  GEARMAND_SERVER_ERROR,
  GEARMAND_WORK_ERROR,
  GEARMAND_WORK_DATA,
  GEARMAND_WORK_WARNING,
  GEARMAND_WORK_STATUS,
  GEARMAND_WORK_EXCEPTION,
  GEARMAND_WORK_FAIL,
  GEARMAND_NOT_CONNECTED,
  GEARMAND_COULD_NOT_CONNECT,
  GEARMAND_SEND_IN_PROGRESS,
  GEARMAND_RECV_IN_PROGRESS,
  GEARMAND_NOT_FLUSHING,
  GEARMAND_DATA_TOO_LARGE,
  GEARMAND_INVALID_FUNCTION_NAME,
  GEARMAND_INVALID_WORKER_FUNCTION,
  GEARMAND_NO_REGISTERED_FUNCTION,
  GEARMAND_NO_REGISTERED_FUNCTIONS,
  GEARMAND_NO_JOBS,
  GEARMAND_ECHO_DATA_CORRUPTION,
  GEARMAND_NEED_WORKLOAD_FN,
  GEARMAND_PAUSE,
  GEARMAND_UNKNOWN_STATE,
  GEARMAND_PTHREAD,
  GEARMAND_PIPE_EOF,
  GEARMAND_QUEUE_ERROR,
  GEARMAND_FLUSH_DATA,
  GEARMAND_SEND_BUFFER_TOO_SMALL,
  GEARMAND_IGNORE_PACKET,
  GEARMAND_UNKNOWN_OPTION,
  GEARMAND_TIMEOUT,
  GEARMAND_ARGUMENT_TOO_LARGE,
  GEARMAND_INVALID_ARGUMENT,
  GEARMAND_MAX_RETURN /* Always add new error code before */
};

#define gearmand_success(X) ((X) == GEARMAND_SUCCESS)
#define gearmand_failed(X) ((X) != GEARMAND_SUCCESS)
