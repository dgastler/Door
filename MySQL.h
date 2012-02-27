#ifndef __MYSQL_H__
#define __MYSQL_H__

#include <mysql/mysql.h>
#include <stdlib.h>


int ProcessMYSQL(MYSQL * result);
int RunSQLquery(MYSQL * conn,char * query,ssize_t querySize);

#endif
