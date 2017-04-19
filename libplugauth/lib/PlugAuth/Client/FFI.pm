package PlugAuth::Client::FFI;

use strict;
use warnings;
use FFI::Platypus;

my $ffi = FFI::Platypus->new;
$ffi->lib('./libplugauth.so');

$ffi->attach( [ 'plugauth_client_new' => '_new' ] => ['string'] => 'opaque');

sub new
{
  my($class, $url) = @_;
  my $ptr = _new($url);
  die "invalid url: @{[ $url || 'undef' ]}" unless $ptr;
  bless \$ptr, $class;
}

$ffi->attach( [ 'plugauth_client_free' => 'DESTROY' ] => ['opaque'] => 'void' => sub {
  my($sub, $self) = @_;
  $sub->($$self);
});

$ffi->attach( [ 'plugauth_client_get_base_url' => 'get_base_url' ] => ['opaque'] => 'string' => sub {
  my($sub, $self) = @_;
  $sub->($$self);
});

$ffi->attach( [ 'plugauth_client_get_auth_url' => 'get_auth_url' ] => ['opaque'] => 'string' => sub {
  my($sub, $self) = @_;
  $sub->($$self);
});

$ffi->attach( [ 'plugauth_client_get_auth' => 'get_auth' ] => ['opaque'] => 'int' => sub {
  my($sub, $self) = @_;
  $sub->($$self);
});

$ffi->attach( [ 'plugauth_client_get_error' => 'get_error' ] => ['opaque'] => 'string' => sub {
  my($sub, $self) = @_;
  $sub->($$self);
});

use constant PLUGAUTH_NOTHING      => 000;
use constant PLUGAUTH_AUTHORIZED   => 200;
use constant PLUGAUTH_UNAUTHORIZED => 403;
use constant PLUGAUTH_ERROR        => 600;

$ffi->attach( [ 'plugauth_client_auth' => 'auth' ] => ['opaque', 'string', 'string'] => 'int' => sub {
  my($sub, $self, $user, $pass) = @_;
  my $ret = $sub->($$self, $user, $pass);
  $ret == PLUGAUTH_AUTHORIZED
    ? 1 : $ret == PLUGAUTH_UNAUTHORIZED ? 0 : die $self->get_error;
});

1;
