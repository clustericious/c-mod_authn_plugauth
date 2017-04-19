#include "plugauth.h"
#include "plugauth_private.h"
#include <curl/curl.h>

int
plugauth_client_auth(plugauth_client_t *self, const char *user, const char *password)
{
  CURL *curl;
  CURLcode res;
  
  self->my_error   = NULL;
  self->ex_error   = NULL;
  self->http_code  = 0L;
  self->auth       = PLUGAUTH_NOTHING;

  if(!user)
    user = "";
  if(!password)
    password = "";

  curl = curl_easy_init();
  
  if(curl)
  {
    /*
    curl_easy_setopt(curl, CURLOPT_VERBOSE,        1L);
    */

    curl_easy_setopt(curl, CURLOPT_URL,            self->auth_url);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_NOBODY,         1L);
    curl_easy_setopt(curl, CURLOPT_USERNAME,       user);
    curl_easy_setopt(curl, CURLOPT_PASSWORD,       password);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,      "libplugauth");
    
    res = curl_easy_perform(curl);
    
    if(res == CURLE_OK)
    {
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &self->http_code);
      switch(self->http_code)
      {
        case 200:
          self->auth = PLUGAUTH_AUTHORIZED;
          break;
        case 403:
          self->auth = PLUGAUTH_UNAUTHORIZED;
          break;
        default:
          self->auth = PLUGAUTH_ERROR;
          self->my_error = "unexpected status code";
          break;
      }
    }
    else
    {
      self->my_error   = "curl_easy_perform() failed";
      self->ex_error   = curl_easy_strerror(res);
      self->auth       = PLUGAUTH_ERROR;
    }
  }
  else
  {
    self->my_error = "curl_easy_init() failed";
    self->auth     = PLUGAUTH_ERROR;
  }
  
  return self->auth;
}

int
plugauth_client_get_auth(plugauth_client_t *self)
{
  return self->auth;
}

const char *
plugauth_client_get_error(plugauth_client_t *self)
{
  if(self->my_error && self->ex_error)
  {
    snprintf(self->error, sizeof(self->error), "%03d %s: %s", self->http_code, self->my_error, self->ex_error);
  }
  else if(self->my_error)
  {
    snprintf(self->error, sizeof(self->error), "%03d %s", self->http_code, self->my_error);
  }
  else
  {
    const char *type;
    switch(self->auth)
    {
      case PLUGAUTH_UNAUTHORIZED:
        type = "unauthorized";
        break;
      case PLUGAUTH_AUTHORIZED:
        type = "authorized";
        break;
      case PLUGAUTH_NOTHING:
        type = "nothing";
        break;
      default:
        type = "unknown";
        break;
    }
    snprintf(self->error, sizeof(self->error), "%03d %s", self->auth, type);
  }
  return self->error;
}

