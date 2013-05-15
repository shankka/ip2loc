/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ip2loc.h"

/* If you declare any globals in php_ip2loc.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(ip2loc)
*/

/* True global resources - no need for thread safety here */
static int le_ip2loc;
#define le_ip2loc_srcname "ip2loc resource type"

static unsigned long ip2long(char* ip TSRMLS_DC);
static int ipcmp(const void*, const void*);

#define IP2LOC_FETCH_RESOURCE(shm_ptr, z_ptr) ZEND_FETCH_RESOURCE(shm_ptr, ip2locshm *, &z_ptr, -1, le_ip2loc_srcname, le_ip2loc)

/* {{{ ip2loc_functions[]
 *
 * Every user visible function must have an entry in ip2loc_functions[].
 */
const zend_function_entry ip2loc_functions[] = {
	PHP_FE(confirm_ip2loc_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(ip2loc, NULL)
	PHP_FE(ip2loc_connect, NULL)
	PHP_FE(ip2loc_get, NULL)
	PHP_FE(ip2loc_close, NULL)
	PHP_FE_END	/* Must be the last line in ip2loc_functions[] */
};
/* }}} */

/* {{{ ip2loc_module_entry
 */
zend_module_entry ip2loc_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"ip2loc",
	ip2loc_functions,
	PHP_MINIT(ip2loc),
	PHP_MSHUTDOWN(ip2loc),
	PHP_RINIT(ip2loc),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(ip2loc),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(ip2loc),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_IP2LOC
ZEND_GET_MODULE(ip2loc)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("ip2loc.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_ip2loc_globals, ip2loc_globals)
    STD_PHP_INI_ENTRY("ip2loc.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_ip2loc_globals, ip2loc_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_ip2loc_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_ip2loc_init_globals(zend_ip2loc_globals *ip2loc_globals)
{
	ip2loc_globals->global_value = 0;
	ip2loc_globals->global_string = NULL;
}
*/

void ip2loc_destruction_handler(zend_rsrc_list_entry *rsrc TSRMLS_DC) 
{
	ip2locshm *shm = (ip2locshm *) rsrc->ptr;
	shmdt(shm->ptr);
	efree(shm);
}


/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ip2loc)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	le_ip2loc = zend_register_list_destructors_ex(ip2loc_destruction_handler, NULL, le_ip2loc_srcname, module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ip2loc)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ip2loc)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ip2loc)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ip2loc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ip2loc support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_ip2loc_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_ip2loc_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;
	
	unsigned long addr;
	iploc * search;
	iploc loc = {0, 0, {0}, {0}, {0}, {0}, {0}};
	int shm_id;
	iploc* shm;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "ip2loc", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

PHP_FUNCTION(ip2loc)
{
	char* ip = NULL;
	int iplen;
	unsigned long addr;
	iploc * search;
	iploc loc = {0, 0, {0}, {0}, {0}, {0}, {0}};
	int shm_id;
	key_t key;
	iploc* shm;
	int argc = ZEND_NUM_ARGS();
	
	if (zend_parse_parameters(argc TSRMLS_CC, "s", &ip, &iplen) == FAILURE)
		return;
	
	if (iplen <= 0)
		return;
		
	addr = ip2long(ip TSRMLS_CC);
	if (addr == 0)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ip invalid!\n");
		RETURN_FALSE;	
	}
	
	loc.start = addr;
	loc.end = addr;
	
	key = ftok(SHMFILENAME, 0);
	if (key == -1)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Get shm key failed!\n");
        RETURN_FALSE;
	}

	shm_id = shmget(key, sizeof(iploc)*IPCNT, IPC_CREAT);
	if (shm_id == -1)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Get shared memory failed!\n");
		RETURN_FALSE;	
	}
	
	shm = (iploc*)shmat(shm_id, 0, 0);
	search = (iploc*)bsearch(&loc, shm, IPCNT, sizeof(iploc), ipcmp);
	if (search == NULL)
	{
		RETURN_FALSE;	
	}
	
	zval *country, *province, *detail;
	MAKE_STD_ZVAL(country);
	MAKE_STD_ZVAL(province);
	MAKE_STD_ZVAL(detail);
	array_init(return_value);
	ZVAL_STRING(country, search->country, 1);
	ZVAL_STRING(province, search->province, 1);
	ZVAL_STRING(detail, search->detail, 1);
	shmdt(shm_id);

	zend_hash_index_update(HASH_OF(return_value), 0, (void *)&country, sizeof(zval *), NULL);
	zend_hash_index_update(HASH_OF(return_value), 1, (void *)&province, sizeof(zval *), NULL);
	zend_hash_index_update(HASH_OF(return_value), 2, (void *)&detail, sizeof(zval *), NULL);
}

PHP_FUNCTION(ip2loc_connect)
{
	ip2locshm* shm_ptr;
	shm_ptr = (ip2locshm*)emalloc(sizeof(ip2locshm));
	
	shm_ptr->key = ftok(SHMFILENAME, 0);
	if (shm_ptr->key == -1)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Get shm key failed!\n");
		efree(shm_ptr);
    RETURN_FALSE;
	}
	
	shm_ptr->id = shmget(shm_ptr->key, sizeof(iploc)*IPCNT, IPC_CREAT);
	if (shm_ptr->id == -1)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Get shared memory failed!\n");
		efree(shm_ptr);
		RETURN_FALSE;	
	}
	
	shm_ptr->ptr = (iploc*)shmat(shm_ptr->id, 0, 0);
	
	ZEND_REGISTER_RESOURCE(return_value, shm_ptr, le_ip2loc);
}

PHP_FUNCTION(ip2loc_get)
{
	zval* z_val;
	char* ip = NULL;
	int iplen;
	ip2locshm* shm_ptr;
	unsigned long addr;
	iploc * search;
	iploc loc = {0, 0, {0}, {0}, {0}, {0}, {0}};
	
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sr", &ip, &iplen, &z_val)) {
		return;
	}
	
	if (iplen <= 0)
		return;
	
	addr = ip2long(ip TSRMLS_CC);
	if (addr == 0)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ip[%s] invalid!\n", ip);
		RETURN_FALSE;	
	}
	
	loc.start = addr;
	loc.end = addr;
	
	IP2LOC_FETCH_RESOURCE(shm_ptr, z_val);
	
	search = (iploc*)bsearch(&loc, shm_ptr->ptr, IPCNT, sizeof(iploc), ipcmp);
	if (search == NULL)
	{
		RETURN_FALSE;	
	}
	
	zval *country, *province, *detail;
	MAKE_STD_ZVAL(country);
	MAKE_STD_ZVAL(province);
	MAKE_STD_ZVAL(detail);
	array_init(return_value);
	ZVAL_STRING(country, search->country, 1);
	ZVAL_STRING(province, search->province, 1);
	ZVAL_STRING(detail, search->detail, 1);

	zend_hash_index_update(HASH_OF(return_value), 0, (void *)&country, sizeof(zval *), NULL);
	zend_hash_index_update(HASH_OF(return_value), 1, (void *)&province, sizeof(zval *), NULL);
	zend_hash_index_update(HASH_OF(return_value), 2, (void *)&detail, sizeof(zval *), NULL);
}

PHP_FUNCTION(ip2loc_close)
{
	zval* z_val;
	ip2locshm* shm_ptr;
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_val)) {
		return;
	}
	
	IP2LOC_FETCH_RESOURCE(shm_ptr, z_val);
	RETURN_BOOL(SUCCESS == zend_list_delete(Z_LVAL_P(z_val)));
}

static
unsigned long ip2long(char* ip TSRMLS_DC)
{
	struct in_addr addr;
	int ret = inet_aton(ip, &addr);
	if (ret == 0)
		return 0;

	return (unsigned long) ntohl(addr.s_addr);
}

static
int ipcmp(const void* a, const void* b)
{
	iploc* va = (iploc*)a;
	iploc* vb = (iploc*) b;
	if (va->start > vb->end)
		return 1;
	else if (va->end < vb->start)
		return -1;
	else
		return 0;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
