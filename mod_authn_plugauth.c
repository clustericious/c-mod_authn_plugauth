#include <strings.h>
#include <stdio.h>
#include "apr_hash.h"
#include "ap_config.h"
#include "ap_provider.h"
#include "httpd.h"
#include "http_core.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"
#include "mod_auth.h"
#include <plugauth.h>

#define DEFAULT_URL "http://127.0.0.1:5001"

typedef struct {
  char url[256];
  int merge_url;
  int ignore_ssl;
  int merge_ignore_ssl;
} plugauth_config;

static const char *plugauth_set_ignore_ssl(cmd_parms *cmd, void *cfg, const char *arg)
{
  plugauth_config *config = (plugauth_config*) cfg;
  
  if(!strcasecmp(arg, "on"))
  {
    config->ignore_ssl = 1;
  }
  else if(!strcasecmp(arg, "off"))
  {
    config->ignore_ssl = 0;
  }
  else
  {
    return "PlugAuthIgnoreSSL muse be on or off";
  }
  config->merge_ignore_ssl = 1;
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
  AP_INIT_TAKE1("PlugAuthURL",       plugauth_set_url,        NULL, RSRC_CONF|OR_AUTHCFG, "The URL to the PlugAuth server"),
  AP_INIT_TAKE1("PlugAuthIgnoreSSL", plugauth_set_ignore_ssl, NULL, RSRC_CONF|OR_AUTHCFG, "Whether to ignore strict SSL checking"),
  { NULL },
};

static authn_status authn_plugauth_password(request_rec *r, const char *user, const char *password);

static void register_hooks(apr_pool_t *pool)
{
  static const authn_provider authn_plugauth_provider = {
    &authn_plugauth_password,
    NULL 
  };
  
  ap_register_auth_provider(
    pool,
    AUTHN_PROVIDER_GROUP,
    "plugauth",
    AUTHN_PROVIDER_VERSION,
    &authn_plugauth_provider,
    AP_AUTH_INTERNAL_PER_CONF
  );
}

static void *create_dir_config(apr_pool_t *pool, char *context)
{
  plugauth_config *config = apr_pcalloc(pool, sizeof(plugauth_config));
  
  if(config)
  {
    config->merge_url     = 0;
    strncpy(config->url, DEFAULT_URL, sizeof(config->url));
    config->ignore_ssl    = 0;
    config->merge_ignore_ssl;
  }
  
  return config;
}

static void *merge_dir_config(apr_pool_t *pool, void *BASE, void *ADD)
{
  plugauth_config *base   = (plugauth_config*) BASE;
  plugauth_config *add    = (plugauth_config*) ADD;
  plugauth_config *config = (plugauth_config*) create_dir_config(pool, "Merged configuration");  
  
  if(config)
  {
    strncpy(config->url, (add->merge_url) ? (add->url) : (base->url), sizeof(config->url));
    config->merge_url     = add->merge_url     || base->merge_url;
    config->ignore_ssl = (add->merge_ignore_ssl) ? (add->ignore_ssl) : (base->ignore_ssl);
    config->merge_ignore_ssl = add->merge_ignore_ssl || base->merge_ignore_ssl;
  }
  
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

static authn_status authn_plugauth_password(request_rec *r, const char *user, const char *password)
{
  plugauth_config *config = (plugauth_config*) ap_get_module_config(r->per_dir_config, &plugauth_module);
  plugauth_client_t *client;
  int res;
  int ret;

  /* return one of: AUTH_GENERAL_ERROR, AUTH_USER_NOT_FOUND, AUTH_DENIED, AUTH_GRANTED */
  client = plugauth_client_new(config->url);
  plugauth_client_set_ignore_ssl(client, config->ignore_ssl);
  
  if(client)
  {
    res = plugauth_client_auth(client, user, password);
    switch(res)
    {
      case PLUGAUTH_AUTHORIZED:
        ret = AUTH_GRANTED;
        break;
      case PLUGAUTH_UNAUTHORIZED:
        ret = AUTH_DENIED;
        break;
      default:
        fprintf(stderr, "libplugauth error: %s", plugauth_client_get_error(client));
        ret = AUTH_GENERAL_ERROR;
        break;
    }
    plugauth_client_free(client);
  }
  else
  {
    fprintf(stderr, "no libplugauth client");
    ret = AUTH_GENERAL_ERROR;
  }
  
  return ret;
}
