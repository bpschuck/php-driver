#include <php.h>
#include <zend_exceptions.h>
#include "../php_cassandra.h"
#include "util/inet.h"
#include "inet.h"

extern zend_class_entry *cassandra_ce_InvalidArgumentException;

zend_class_entry *cassandra_ce_Inet = NULL;

/* {{{ Cassandra\Inet::__construct(string) */
PHP_METHOD(CassandraInet, __construct)
{
  char *address;
  int address_len;
  cassandra_inet* inet;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &address, &address_len) == FAILURE) {
    return;
  }

  inet = (cassandra_inet*) zend_object_store_get_object(getThis() TSRMLS_CC);

  char* tmp = strndup(address, address_len);
  if (!php_cassandra_parse_ip_address(tmp, &inet->inet)) {
    free(tmp);
    return;
  }
  free(tmp);

  char* string;
  php_cassandra_format_address(inet->inet, &string);

  zend_update_property_string(cassandra_ce_Inet, getThis(), "address", strlen("address"), string TSRMLS_CC);
  efree(string);
}
/* }}} */

/* {{{ Cassandra\Inet::__toString() */
PHP_METHOD(CassandraInet, __toString)
{
  zval *zode = zend_read_property(cassandra_ce_Inet, getThis(), "address", strlen("address"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), 1);
}
/* }}} */

/* {{{ Cassandra\Inet::address() */
PHP_METHOD(CassandraInet, address)
{
  zval *zode = zend_read_property(cassandra_ce_Inet, getThis(), "address", strlen("address"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), 1);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, address)
ZEND_END_ARG_INFO()

static zend_function_entry CassandraInet_methods[] = {
  PHP_ME(CassandraInet, __construct, arginfo___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraInet, __toString, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraInet, address, NULL, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static void
php_cassandra_inet_free(void *object TSRMLS_DC)
{
  cassandra_inet* inet = (cassandra_inet*) object;

  zend_object_std_dtor(&inet->zval TSRMLS_CC);

  efree(inet);
}

static zend_object_value
php_cassandra_inet_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_inet *inet;

  inet = (cassandra_inet*) emalloc(sizeof(cassandra_inet));
  memset(inet, 0, sizeof(cassandra_inet));

  zend_object_std_init(&inet->zval, class_type TSRMLS_CC);
  object_properties_init(&inet->zval, class_type TSRMLS_CC);

  retval.handle = zend_objects_store_put(inet, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_inet_free, NULL TSRMLS_CC);
  retval.handlers = zend_get_std_object_handlers();

  return retval;
}

void cassandra_define_CassandraInet(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Inet", CassandraInet_methods);
  cassandra_ce_Inet = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_ce_Inet->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Inet->create_object = php_cassandra_inet_new;

  /* fields */
  zend_declare_property_string(cassandra_ce_Inet, "address", strlen("address"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
}
