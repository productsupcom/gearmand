/* Gearman server and library
 * Copyright (C) 2008 Brian Aker, Eric Day
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/**
 * @file
 * @brief libmemcached Queue Storage Definitions
 */

#include <libgearman-server/common.h>

#include <libgearman-server/plugins/queue/libmemcached/queue.h>
#include <libmemcached/memcached.h>

/**
 * @addtogroup gearman_queue_libmemcached_static Static libmemcached Queue Storage Functions
 * @ingroup gearman_queue_libmemcached
 * @{
 */

/**
 * Default values.
 */
#define GEARMAN_QUEUE_LIBMEMCACHED_DEFAULT_PREFIX "gear_"

/**
 * Structure for libmemcached specific data.
 */
typedef struct
{
  memcached_st memc;
} gearman_queue_libmemcached_st;

/* Queue callback functions. */
static gearmand_error_t _libmemcached_add(gearman_server_st *server,
                                          void *context,
                                          const char *unique, size_t unique_size,
                                          const char *function_name, size_t function_name_size,
                                          const void *data, size_t data_size,
                                          gearmand_job_priority_t priority);
static gearmand_error_t _libmemcached_flush(gearman_server_st *server,
                                            void *context);
static gearmand_error_t _libmemcached_done(gearman_server_st *server,
                                           void *context,
                                           const char *unique, size_t unique_size,
                                           const char *function_name, size_t function_name_size);
static gearmand_error_t _libmemcached_replay(gearman_server_st *server,
                                             void *context,
                                             gearman_queue_add_fn *add_fn,
                                             void *add_context);

/** @} */

/*
 * Public definitions
 */

gearmand_error_t gearman_server_queue_libmemcached_conf(gearman_conf_st *conf)
{
  gearman_conf_module_st *module;

  module= gearman_conf_module_create(conf, NULL, "libmemcached");
  if (module == NULL)
    return GEARMAN_MEMORY_ALLOCATION_FAILURE;

  gearman_conf_module_add_option(module, "servers", 0, "SERVER_LIST",
                                 "List of Memcached servers to use.");
  return gearman_conf_return(conf);
}

gearmand_error_t
gearman_server_queue_libmemcached_init(gearman_server_st *server,
                                       gearman_conf_st *conf)
{
  gearman_queue_libmemcached_st *queue;
  gearman_conf_module_st *module;
  const char *name;
  const char *value;
  memcached_server_st *servers;
  const char *opt_servers= NULL;

  gearmand_info("Initializing libmemcached module");

  queue= calloc(1, sizeof(gearman_queue_libmemcached_st));
  if (queue == NULL)
  {
    gearmand_log_error("gearman_queue_libmemcached_init", "malloc");

    return GEARMAN_MEMORY_ALLOCATION_FAILURE;
  }

  if (memcached_create(&(queue->memc)) == NULL)
  {
    free(queue);
    gearmand_log_error("gearman_queue_libmemcached_init", "memcached_create");
    return GEARMAN_QUEUE_ERROR;
  }

  /* Get module and parse the option values that were given. */
  module= gearman_conf_module_find(conf, "libmemcached");
  if (module == NULL)
  {
    gearmand_log_error("gearman_queue_libmemcached_init", "gearman_conf_module_find:NULL");

    return GEARMAN_QUEUE_ERROR;
  }

  while (gearman_conf_module_value(module, &name, &value))
  {
    if (!strcmp(name, "servers"))
    {
      opt_servers= value;
    }
    else
    {
      memcached_free(&(queue->memc));
      free(queue);
      gearmand_log_error("gearman_queue_libmemcached_init", "Unknown argument: %s", name);
      return GEARMAN_QUEUE_ERROR;
    }
  }

  if (opt_servers == NULL)
  {
    gearmand_log_error("gearman_queue_libmemcached_init", "No --servers given");
    return GEARMAN_QUEUE_ERROR;
  }

  servers= memcached_servers_parse(opt_servers);

  if (servers == NULL)
  {
    memcached_free(&(queue->memc));
    free(queue);

    gearmand_log_error("gearman_queue_libmemcached_init", "memcached_servers_parse");

    return GEARMAN_QUEUE_ERROR;
  }

  memcached_server_push(&queue->memc, servers);
  memcached_server_list_free(servers);

  gearman_server_set_queue(server, queue, _libmemcached_add, _libmemcached_flush, _libmemcached_done, _libmemcached_replay);

  return GEARMAN_SUCCESS;
}

gearmand_error_t
gearman_server_queue_libmemcached_deinit(gearman_server_st *server)
{
  gearman_queue_libmemcached_st *queue;

  gearmand_info("Shutting down libmemcached queue module");

  queue= (gearman_queue_libmemcached_st *)gearman_server_queue_context(server);
  gearman_server_set_queue(server, NULL, NULL, NULL, NULL, NULL);
  memcached_free(&(queue->memc));

  free(queue);

  return GEARMAN_SUCCESS;
}

gearmand_error_t gearmand_queue_libmemcached_init(gearmand_st *gearmand,
                                                  gearman_conf_st *conf)
{
  return gearman_server_queue_libmemcached_init(&(gearmand->server), conf);
}

gearmand_error_t gearmand_queue_libmemcached_deinit(gearmand_st *gearmand)
{
  return gearman_server_queue_libmemcached_deinit(&(gearmand->server));
}

/*
 * Static definitions
 */

static gearmand_error_t _libmemcached_add(gearman_server_st *server,
                                          void *context,
                                          const char *unique,
                                          size_t unique_size,
                                          const char *function_name,
                                          size_t function_name_size,
                                          const void *data, size_t data_size,
                                          gearmand_job_priority_t priority)
{
  gearman_queue_libmemcached_st *queue= (gearman_queue_libmemcached_st *)context;
  memcached_return rc;
  char key[MEMCACHED_MAX_KEY];
  size_t key_length;

  (void)server;

  gearmand_log_debug("libmemcached add: %.*s", (uint32_t)unique_size, (char *)unique);

  key_length= (size_t)snprintf(key, MEMCACHED_MAX_KEY, "%s%.*s-%.*s",
                               GEARMAN_QUEUE_LIBMEMCACHED_DEFAULT_PREFIX,
                               (int)function_name_size,
                               (const char *)function_name, (int)unique_size,
                               (const char *)unique);

  rc= memcached_set(&queue->memc, (const char *)key, key_length,
                    (const char *)data, data_size, 0, (uint32_t)priority);

  if (rc != MEMCACHED_SUCCESS)
    return GEARMAN_QUEUE_ERROR;

  return GEARMAN_SUCCESS;
}

static gearmand_error_t _libmemcached_flush(gearman_server_st *server,
                                            void *context)
{
  gearmand_debug("libmemcached flush");
  (void)server;
  (void)context;

  return GEARMAN_SUCCESS;
}

static gearmand_error_t _libmemcached_done(gearman_server_st *server,
                                           void *context,
                                           const char *unique, size_t unique_size,
                                           const char *function_name, size_t function_name_size)
{
  size_t key_length;
  char key[MEMCACHED_MAX_KEY];
  memcached_return rc;
  gearman_queue_libmemcached_st *queue= (gearman_queue_libmemcached_st *)context;
  (void)server;

  gearmand_log_debug("libmemcached done: %.*s", (uint32_t)unique_size, (char *)unique);

  key_length= (size_t)snprintf(key, MEMCACHED_MAX_KEY, "%s%.*s-%.*s",
                               GEARMAN_QUEUE_LIBMEMCACHED_DEFAULT_PREFIX,
                               (int)function_name_size,
                               (const char *)function_name, (int)unique_size,
                               (const char *)unique);

  /* For the moment we will assume it happened */
  rc= memcached_delete(&queue->memc, (const char *)key, key_length, 0);

  if (rc != MEMCACHED_SUCCESS)
    return GEARMAN_QUEUE_ERROR;

  return GEARMAN_SUCCESS;
}

struct replay_context
{
  memcached_st clone;
  gearman_server_st *server;
  gearman_queue_add_fn *add_fn;
  void *add_context;
};

static memcached_return callback_loader(const memcached_st *ptr __attribute__((unused)),
                                        memcached_result_st *result __attribute__((unused)),
                                        void *context)
{
  struct replay_context *container= (struct replay_context *)context;
  const char *key;
  char *unique;
  const char *function;
  size_t function_len;
  char *data;
  size_t data_size;

  key= memcached_result_key_value(result);
  if (strncmp(key, GEARMAN_QUEUE_LIBMEMCACHED_DEFAULT_PREFIX, strlen(GEARMAN_QUEUE_LIBMEMCACHED_DEFAULT_PREFIX)))
    return MEMCACHED_SUCCESS;

  function= key + strlen(GEARMAN_QUEUE_LIBMEMCACHED_DEFAULT_PREFIX);

  unique= index(function, '-');

  if (! unique)
    return MEMCACHED_SUCCESS;

  function_len = (size_t) (unique-function);
  unique++;

  assert(unique);
  assert(strlen(unique));
  assert(function);
  assert(function_len);

  data_size= (size_t) memcached_result_length(result);
  /* need to make a copy here ... gearman_server_job_free will free it later */
  data= (char *)malloc(data_size);
  if (data == NULL)
  {
    gearmand_perror("malloc");
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;
  } 
  memcpy(data, memcached_result_value(result), data_size);

  /* Currently not looking at failure cases */
  (void)(*container->add_fn)(container->server, container->add_context,
                             unique, strlen(unique),
                             function, function_len,
                             data, data_size,
                             memcached_result_flags(result));


  return MEMCACHED_SUCCESS;
}

/* Grab the object and load it into the loader */
static memcached_return callback_for_key(const memcached_st *ptr __attribute__((unused)),
                                         const char *key, size_t key_length,
                                         void *context)
{
  memcached_return rc;
  struct replay_context *container= (struct replay_context *)context;
  memcached_execute_function callbacks[1];
  char *passable[1];

  callbacks[0]= (memcached_execute_fn)&callback_loader;

  passable[0]= (char *)key;
  rc= memcached_mget(&container->clone, (void *)passable, &key_length, 1);

  /* Just void errors for the moment, since other treads might have picked up the object. */
  (void)memcached_fetch_execute(&container->clone, callbacks, context, 1);

  return MEMCACHED_SUCCESS;
}

/*
  If we have any failures for loading values back into replay we just ignore them.
*/
static gearmand_error_t _libmemcached_replay(gearman_server_st *server, void *context,
                                             gearman_queue_add_fn *add_fn,
                                             void *add_context)
{
  gearman_queue_libmemcached_st *queue= (gearman_queue_libmemcached_st *)context;
  struct replay_context container;
  memcached_st *check_for_failure;
  memcached_dump_func callbacks[1];

  callbacks[0]= (memcached_dump_fn)&callback_for_key;

  gearmand_info("libmemcached replay start");

  memset(&container, 0, sizeof(struct replay_context));
  check_for_failure= memcached_clone(&container.clone, &queue->memc);
  container.server= server;
  container.add_fn= add_fn;
  container.add_context= add_context;

  assert(check_for_failure);


  (void)memcached_dump(&queue->memc, callbacks, (void *)&container, 1);

  memcached_free(&container.clone);

  return GEARMAN_SUCCESS;
}
