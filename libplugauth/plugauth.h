#ifndef PLUGAUTH_H
#define PLUGAUTH_H

struct _plugauth_client_t;
typedef struct _plugauth_client_t plugauth_client_t;

plugauth_client_t *plugauth_client_new(const char *url);
void plugauth_client_free(plugauth_client_t *self);

const char *plugauth_client_get_base_url(plugauth_client_t *self);
const char *plugauth_client_get_auth_url(plugauth_client_t *self);
int plugauth_client_get_auth(plugauth_client_t *self);
const char *plugauth_client_get_error(plugauth_client_t *self);
int plugauth_client_get_ignore_ssl(plugauth_client_t *self);
void plugauth_client_set_ignore_ssl(plugauth_client_t *self, int value);

int plugauth_client_auth(plugauth_client_t *self, const char *user, const char *password);

enum plugauth_client_auth_t {
  PLUGAUTH_NOTHING      = 000,
  PLUGAUTH_AUTHORIZED   = 200,
  PLUGAUTH_UNAUTHORIZED = 403,
  PLUGAUTH_ERROR        = 600
};

#endif
