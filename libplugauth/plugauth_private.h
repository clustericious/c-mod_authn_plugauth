#ifndef PLUGAUTH_PRIVATE_H
#define PLUGAUTH_PRIVATE_H

struct _plugauth_client_t
{
  char *base_url;
  char *auth_url;
  const char *my_error;
  const char *ex_error;
  long http_code;
  int  auth;
  char error[255];
};

#endif
