#include "MySQL.h"

int ProcessMYSQL(MYSQL * result)
{
  MYSQL_ROW row;
  MYSQL_FIELD *fields;

  unsigned int num_fields = mysql_num_fields(result);
  int i;
  row = mysql_fetch_row(result);
  //If now rows, then there is no user and don't open the door
  //return false
  if(!row)
    return 0;
  
  fields = mysql_fetch_fields(result);  
  unsigned long *lengths;
  lengths = mysql_fetch_lengths(result);

  //Calculate the size of the string we need to allocate
  ssize_t JSONsize = 3; //open and close bracket and '\n'
  for(i = 0; i < num_fields; i++)
    {
      JSONsize += 5; //four quotes and a ':'
      JSONsize += strlen(fields[i].name); //length of field name
      JSONsize += lengths[i]; //length of field contents
      if(i != (num_fields-1)) //ending comma for all but the last entry
	JSONsize++;
    }
  //allocate the string we are going to send out
  char * JSONbuffer = malloc(JSONsize+1);
  if(JSONbuffer != NULL)
    {
      JSONbuffer[JSONsize] = NULL;
      char * ptr = JSONbuffer;
      *ptr = '{'; ptr++;      //starting curly
      for(i = 0; i < num_fields; i++)
	{
	  sprintf(ptr,"\"%s\":\"%*s\"",
		  fields[i].name,
		  (int) lengths[i],
		  row[i]);
	  ptr+=5; //two quotes and a ':'
	  ptr+=lengths[i];  //length of field contents
	  ptr+=strlen(fields[i].name); //length of field name
	  if(i != (num_fields-1))
	    {
	      *ptr = ',';
	      ptr++;
	    }
	} 
      *ptr = '}'; //ending curly     
      ptr++; 
      *ptr = '\n';
      ptr++;
      SendOut(JSONbuffer,JSONsize);
      //      printf("%s",JSONbuffer);
      free(JSONbuffer);
    }
  return 1;
}

int RunSQLquery(MYSQL * conn,
		char * query,
		ssize_t querySize)
{
  int opendoor = 0;
  MYSQL_RES *result;
  int query_state = mysql_real_query(conn,query,querySize);  
  if (query_state != 0)
    { 	  
      printf("MySQL Query error:\t%s\n",mysql_error(conn));
      return;
    }
  else                                                                                                  
    {
      result = mysql_store_result(conn);  
      opendoor = ProcessMYSQL(result);
      mysql_free_result(result);
    }
  return opendoor;
}
