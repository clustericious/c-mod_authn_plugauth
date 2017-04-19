#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "plugauth.h"
#include "plugauth_private.h"

plugauth_client_t *
plugauth_client_new(const char *url)
{
  size_t size;
  plugauth_client_t *self;

  if(!url)
    return NULL;

  size = strlen(url);
  
  if(size < 5)
    return NULL;
  
  if(curl_global_init(CURL_GLOBAL_ALL))
    return NULL;
  
  self = malloc(sizeof(plugauth_client_t));
  
  if(self)
  {
    self->base_url = strdup(url);
    /* auth url is base_url + '/auth' */
    self->auth_url = malloc(size+6);
    if(url[size-1] == '/')
      snprintf(self->auth_url, size+6, "%sauth", url);
    else
      snprintf(self->auth_url, size+6, "%s/auth", url);
      
    self->my_error  = NULL;
    self->ex_error  = NULL;
    self->http_code = 0L;
    self->auth      = PLUGAUTH_NOTHING;
    
  }
  
  return self;
}

void
plugauth_client_free(plugauth_client_t *self)
{
  free(self->base_url);
  free(self->auth_url);
  free(self);
  curl_global_cleanup();
}

const char *plugauth_client_get_base_url(plugauth_client_t *self)
{
  return self->base_url;
}

const char *plugauth_client_get_auth_url(plugauth_client_t *self)
{
  return self->auth_url;
}
