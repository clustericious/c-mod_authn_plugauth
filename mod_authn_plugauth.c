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

typedef struct {
  int enabled;
  const char *url;
} plugauth_config;

static plugauth_config config;

static const char *plugauth_set_enabled(cmd_parms *cmd, void *cfg, const char *arg)
{
  if(!strcasecmp(arg, "on"))
    config.enabled = 1;
  else
    config.enabled = 0;
  return NULL;
}

static const char *plugauth_set_url(cmd_parms *cmd, void *cfg, const char *arg)
{
  config.url = arg;
  return NULL;
}

static const command_rec plugauth_directives[] =
{
  AP_INIT_TAKE1("PlugAuthEnabled", plugauth_set_enabled, NULL, OR_AUTHCFG, "Enable or disable mod_example"),
  AP_INIT_TAKE1("PlugAuthURL",     plugauth_set_url,     NULL, OR_AUTHCFG, "The URL to the PlugAuth server"),
  { NULL },
};

static int plugauth_handler(request_rec *r)
{
  return DECLINED;
}

static void register_hooks(apr_pool_t *pool)
{
  config.enabled = 0;
  config.url     = "http://127.0.0.1:5001/";
  ap_hook_handler(plugauth_handler, NULL, NULL, APR_HOOK_LAST);
}

module AP_MODULE_DECLARE_DATA plugauth_module =
{
  STANDARD20_MODULE_STUFF,
  NULL,                /* Per-directory configuration handler */
  NULL,                /* Merge handler for per-directory configurations */
  NULL,                /* Per-server configuration handler */
  NULL,                /* Merge handler for per-server configurations */
  plugauth_directives, /* Any directives we may have for httpd */
  register_hooks       /* Our hook registering function */
};
