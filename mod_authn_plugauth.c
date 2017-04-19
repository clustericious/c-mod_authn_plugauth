#include <strings.h>
#include "apr_hash.h"
#include "ap_config.h"
#include "ap_provider.h"
#include "httpd.h"
#include "http_core.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"

#define DEFAULT_URL "http://127.0.0.1:5001"

typedef struct {
  int enabled;
  char url[256];
  int merge_enabled;
  int merge_url;
} plugauth_config;

static const char *plugauth_set_enabled(cmd_parms *cmd, void *cfg, const char *arg)
{
  plugauth_config *config = (plugauth_config*) cfg;

  if(!strcasecmp(arg, "on"))
    config->enabled = 1;
  else if(!strcasecmp(arg, "off"))
    config->enabled = 0;
  else
    return "PlugAuthEnabled must be either on or off";
  config->merge_enabled = 1;
  return NULL;
}

static const char *plugauth_set_url(cmd_parms *cmd, void *cfg, const char *arg)
{
  plugauth_config *config = (plugauth_config*) cfg;

  strncpy(config->url, arg, sizeof(config->url));
  config->merge_url = 1;
  return NULL;
}

static const command_rec plugauth_directives[] =
{
  AP_INIT_TAKE1("PlugAuthEnabled", plugauth_set_enabled, NULL, RSRC_CONF|OR_AUTHCFG, "Enable or disable mod_example"),
  AP_INIT_TAKE1("PlugAuthURL",     plugauth_set_url,     NULL, RSRC_CONF|OR_AUTHCFG, "The URL to the PlugAuth server"),
  { NULL },
};

static int plugauth_check_authn(request_rec *r);

static void register_hooks(apr_pool_t *pool)
{
  ap_hook_handler(plugauth_check_authn, NULL, NULL, APR_HOOK_FIRST);
}

static void *create_dir_config(apr_pool_t *pool, char *context)
{
  plugauth_config *config = apr_pcalloc(pool, sizeof(plugauth_config));
  
  if(config)
  {
    config->enabled       = 0;
    config->merge_enabled = 0;
    config->merge_url     = 0;
    strncpy(config->url, DEFAULT_URL, sizeof(config->url));
  }
  
  return config;
}

static void *merge_dir_config(apr_pool_t *pool, void *BASE, void *ADD)
{
  plugauth_config *base   = (plugauth_config*) BASE;
  plugauth_config *add    = (plugauth_config*) ADD;
  plugauth_config *config = (plugauth_config*) create_dir_config(pool, "Merged configuration");  
  
  config->enabled = (add->merge_enabled) ? (add->enabled) : (base->enabled);
  strncpy(config->url, (add->merge_url) ? (add->url) : (base->url), sizeof(config->url));

  config->merge_enabled = add->merge_enabled || base->merge_enabled;
  config->merge_url     = add->merge_url     || base->merge_url;
  
  return config;
}

module AP_MODULE_DECLARE_DATA plugauth_module =
{
  STANDARD20_MODULE_STUFF,
  create_dir_config,   /* Per-directory configuration handler */
  merge_dir_config,    /* Merge handler for per-directory configurations */
  NULL,                /* Per-server configuration handler */
  NULL,                /* Merge handler for per-server configurations */
  plugauth_directives, /* Any directives we may have for httpd */
  register_hooks       /* Our hook registering function */
};

static int plugauth_check_authn(request_rec *r)
{
  plugauth_config *config = (plugauth_config*) ap_get_module_config(r->per_dir_config, &plugauth_module);

  if(config->enabled)
  {
    ap_set_content_type(r, "text/plain");
    ap_rprintf(r, "401 Unauthorized");
    return HTTP_UNAUTHORIZED;
  }

  return DECLINED;
}

