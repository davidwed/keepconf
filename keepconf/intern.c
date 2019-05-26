/**
 *     AUTHOR: Jose Angel Caso Sanchez, 2013   ( altomaltes@yahoo.es )
 *                                             ( altomaltes@gmail.com )
 *
 *     Copyright (C) 2006, 2013 JACS
 *
 * This program is free software; you can redisgibute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is disgibuted in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *     FILE: intern.c
 *     DATE: jun 2013
 *
 *  DESCRIPCION: static data collection
 *
 */


#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>



#define POOL_START_SIZE 2   /* First data pool */



/* Min block size 16
 * Max blocl size 1Mb
 */
#define MIN_BLOCK  16
#define MAX_BLOCKS 32

typedef struct InternPtr
{ struct InternPtr * next;  /* pointer to data */
} InternPtr;

static InternPtr * blocks= NULL;  /* Memory lists vector, also init flag */

/* ================================================== [ JACS, 2009/12/10 ] == *\
 *                                                                            *
 *   JACS 2009 (altomaltes@yahoo.es)                                          *
 *             (altomaltes@gmail.com)                                         *
 *                                                                            *
 *  FUNCTION freeResources                                                    *
 *                                                                            *
 *  @brief Free allocated blocks. It is no strictly neccessary, since OS      *
 *         frees resources on exit yet, and a corrutted heap may hang process *
 *                                                                            *
\* ========================================================================== */
static void freeResources()
{ InternPtr * ptr, * next;
  int idx;

  for( idx= 0
     ; idx< MAX_BLOCKS
     ; idx ++ )
  { next= blocks[ idx ].next;

    while(( ptr= next ))
    { next= ptr->next;    /* Save next, as become undefined on free */
      free( ptr );
  } }

 free( blocks ); blocks= NULL;
}

/** ================================================= [ JACS, 2009/12/10 ] == *\
 *                                                                            *
 *   JACS 2009 (altomaltes@yahoo.es)                                          *
 *             (altomaltes@gmail.com)                                         *
 *                                                                            *
 *  FUNCTION structById                                                       *
 *           stringById                                                       *
 *                                                                            *
 *  @brief Very abstract. stores a general structure, and gets an ID for it.  *
 *                                                                            *
\* ========================================================================== */
void * structById( const void * data
                 , size_t sz )
{ InternPtr * ptr;
  int type= MIN_BLOCK;

  if ( ! blocks )
  { blocks= (InternPtr*)calloc( MAX_BLOCKS, sizeof( InternPtr ));
    atexit( freeResources );
  }
  ptr= blocks;

/* fit size on quantized value vector
 */
  for( sz += sizeof( void * )
     ; type < sz
     ; ptr++, type += type >> 1 );
  sz -= sizeof( void *);

/* step list till end
 */
  while( ptr->next )
  { ptr= ptr->next;
    if ( !memcmp( ptr+1, data, sz ))              /* Is suitable, but can vary in size */
    { return( ptr+1 );                            /* Found, return data                */
  } }

  ptr->next= calloc( type, 1 ); ptr= ptr->next;   /* ask for a new value */

  memcpy( ptr+1, data, sz );
  return( ptr+1 );                                /* Found, return data                */
}

/* Very handy for const string comparations.
 */

const char * stringById( const char * str )
{ return( str                                             /* Include trailing NULL   */
        ? (const char *)structById( str, strlen(str)+1 )
        : NULL );                             /* Make void strings equal */
}


int strcasecmp(const char *s1, const char *s2)
{ const unsigned char * us1 = (const unsigned char *)s1,
                      * us2 = (const unsigned char *)s2;

  while ( tolower( *us1 ) == tolower(*us2++) )
    if (*us1++ == '\0')
      return (0);

  return (tolower(*us1) - tolower(*--us2));
}

#define MAXL 32

char *lltoa ( long long val, char * buf, int base )
{ int i = MAXL;
  int sign = (val < 0);
  if (sign) val = -val;

  if ( val == 0 ) return "0";

  for(
     ; val && i
     ; --i, val /= base)
  { buf[i] = "0123456789ABCDEF"[val % base];
  }

  if(sign)
  { buf[ i-- ] = '-';
  }

  memcpy( buf, buf+i+1, MAXL-i ); buf[MAXL-i]= 0;

  return( buf );
}

char *ulltoa ( unsigned long long val, char * buf, int base )
{ int i = MAXL;

  if ( val == 0 ) return "0";

  for(
     ; val && i
     ; --i, val /= base)
  { buf[ i ]= "0123456789ABCDEF"[ val % base ];
  }

  memcpy( buf, buf+i+1, MAXL-i ); buf[MAXL-i]= 0;

  return( buf );
}

