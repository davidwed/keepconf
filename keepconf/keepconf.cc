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
 *     FILE: keepconf.cc
 *     DATE: jun 2017
 *
 *  DESCRIPCION: xml config file maintainer.
 *
 */

#define _OPEN_SYS_ITOA_EXT

#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

#include "keepconf.h"

//extern "C" char *   itoa(       int value, char * str, int base );
extern "C" char * ulltoa(unsigned long long val,char *buf, int radix);
extern "C" char *   gcvt(   double number, int ndigit, char * buf);
//extern "C" char *   ltoa(      long value, char * buff, int radix );
extern "C" char *  lltoa( long long value, char * buff, int radix );

char *itoa( int n, char * buf, int base )
{ sprintf( buf, "%d", n );
  return   buf;
}

char *ltoa( long n, char * buf, int base )
{ sprintf( buf, "%ld", n );
  return   buf;
}


/* =====================================================[ JACS 2012-06-08 ]== *\
 *                                                                            *
 *   altomaltes@yahoo.es                                                      *
 *                                                                            *
 *  METHOD: ObjConfRec::enter                                                 *
 *                                                                            *
 *  @brief enter a subbranch of XML file                                      *
 *                                                                            *
\* ========================================================================== */
void ObjConfRec::enter( const char * id
                      , const char * name
                      , const char * value
                      , int          idx )
{

/* Overload some values
 */
  xmlData.level= info-levels;
  xmlData.ptr=   (char*)value;

  xmlData.idx=             info->index= idx;
  strcpy( xmlData.buffNme, info->names= name );
  strcpy( xmlData.buffItm, info->objs = id   );

  parserStub( &xmlData );

  if ( !value )
  { info++;      /* Is an object */
} }


/* =====================================================[ JACS 2012-06-08 ]== *\
 *                                                                            *
 *   altomaltes@yahoo.es                                                      *
 *                                                                            *
 *  METHOD: ObjConfRec::leave                                                 *
 *                                                                            *
 *  @brief leaves a subbranch of XML file                                     *
 *                                                                            *
\* ========================================================================== */
const char * ObjConfRec::leave( int idx )
{ info --;

  xmlData.idx  = idx;
  xmlData.level= info-levels;
  xmlData.ptr  = (char*)""; // Mark leave
  strcpy( xmlData.buffNme, info->names );

  parserStub( &xmlData );
  return( info->names );
}


extern "C" const char * stringById( const char * str );

/* =====================================================[ JACS 2012-06-08 ]== *\
 *                                                                            *
 *   altomaltes@yahoo.es                                                      *
 *                                                                            *
 *  CALLBACK: pushElement                                                    *
 *                                                                            *
 *  @brief expat handlers.                                                    *
 *                                                                            *
\* ========================================================================== */
ANSIC const char * pushElement( ObjConfRec & cnf
                              , int   phase
                              , const char * name
                              , const char * type
                              , const char * value )
{ cnf.value= value;
  ObjLevelRec * code= cnf.info-1;   // Current loader
  ObjLevelRec * args= cnf.info;     // Current data holder

//  printf( "push (%d) %10s %10s %10s\n", phase, name, type, value );

  if ( name )                       // Enter object or stamp value
  { if ( *value )
    { args->objs= stringById( value );
    }
    else
    { args->objs= NULL;
      type= "class";
    }

    args->names= stringById( name );

    if ( !strcmp( type, "style" ))
    { cnf.info= cnf.levels+1;  // Start process
      return( 0 );
    }

    if ( !strcmp( type, "class" ))
    { args->index= phase;
      if ( args > cnf.levels )
      { code->loader( cnf, code->holder );
      }

   //   printf( ">>>: %2d: %s >>> %d    [ %10s-%10s ]\n"
     //       , args - cnf.levels
       //     , phase == INTEGER_NAN ? "STALE" : "     "
         //   , phase == INTEGER_NAN ? 0       : args->index
           // , args->names, args->objs );

      cnf.info++;   // a more level
      return( NULL );
    }
    else
    { args->objs= type;
    }

    args->index= phase;
    if ( code->loader ) code->loader( cnf, code->holder );


    args->objs=   NULL;    // Mark arguments as invalid
    args->names=  NULL;
    args->loader= NULL;
    args->holder= NULL;
  }                        // Leave object
  else
  { cnf.info--;
    cnf.value= NULL;

    if ( cnf.info > cnf.levels )
    { code->loader( cnf, code->holder );
    }

    return( cnf.info->names );    // Name of the object needed
  }

  return( NULL );
}


/** ====================================================[ JACS 2012-06-08 ]== *\
 *                                                                            *
 *   altomaltes@yahoo.es                                                      *
 *                                                                            *
 *  BUNCH formString                                                          *
 *                                                                            *
 *  @brief pure type to human readable conversions.                           *
 *                                                                            *
\* ========================================================================= **/
const char * asString( char * buff, const           bool w ) { return(  w ? "TRUE" : "FALSE" );    }
const char * asString( char * buff, const           char w ) { return(   itoa( w, buff, RADIX )); }
const char * asString( char * buff, const   signed  char w ) { return(   itoa( w, buff, RADIX )); }
const char * asString( char * buff, const unsigned  char w ) { return( ulltoa( w, buff, RADIX )); }

const char * asString( char * buff, const   signed short w ) { return(   itoa( w, buff, RADIX )); }
const char * asString( char * buff, const unsigned short w ) { return( ulltoa( w, buff, RADIX )); }

const char * asString( char * buff, const           int  w ) { return(   itoa( w, buff, RADIX )); }
const char * asString( char * buff, const unsigned  int  w ) { return( ulltoa( w, buff, RADIX )); }

const char * asString( char * buff, const          long  w ) { return(   ltoa( w, buff, RADIX )); }
const char * asString( char * buff, const unsigned long  w ) { return( ulltoa( w, buff, RADIX )); }

const char * asString( char * buff, const float          w ) { return( gcvt( w, 10, buff )); }
const char * asString( char * buff, const double         w ) { return( gcvt( w, 10, buff )); }

const char * asString( char * buff, const char         * w ) { return( strcpy( buff, w )); }
const char * asString( char * buff,       char         * w ) { return( strcpy( buff, w )); }

//const char * asString( char * buff, const char         * w ) { return( strcpy( buff, w )); }
//const char * asString( char * buff,       char         * w ) { return( strcpy( buff, w )); }

#ifdef __GNUG__
 const char * asString( char * buff, const qlong w ) { return(  lltoa( w, buff, RADIX )); }
 const char * asString( char * buff, const qword w ) { return( ulltoa( w, buff, RADIX )); }
#endif

/** ====================================================[ JACS 2012-06-08 ]== *\
 *                                                                            *
 *   altomaltes@yahoo.es                                                      *
 *                                                                            *
 *  BUNCH formString                                                          *
 *                                                                            *
 *  @brief human readable to pure type conversions.                           *
 *                                                                            *
\* ========================================================================= **/
const char * fromString(           bool      & w, const char * buff )
{ w= ( *buff == 'T' );
  return( buff );
}

const char * fromString(           char      & w, const char * buff )
{ w= (char)strtol( buff, NULL, RADIX ); return( buff );
}

const char * fromString(  unsigned char      & w, const char * buff )
{ w= (unsigned char)strtoul( buff, NULL, RADIX ); return( buff );
}

const char * fromString(           int       & w, const char * buff )
{ w=  strtol( buff, NULL, RADIX ); return( buff );
}

const char * fromString(          long       & w, const char * buff )
{ w=  strtol( buff, NULL, RADIX ); return( buff );
}

const char * fromString(  unsigned int       & w, const char * buff )
{ w= strtoul( buff, NULL, RADIX ); return( buff );
}

const char * fromString(  unsigned long      & w, const char * buff )
{ w= strtoul( buff, NULL, RADIX ); return( buff );
}

const char * fromString(           short     & w, const char * buff )
{ w=  (short)strtol( buff, NULL, RADIX ); return( buff );
}

const char * fromString(  unsigned short     & w, const char * buff )
{ w= (unsigned short)strtoul( buff, NULL, RADIX ); return( buff );
}

const char * fromString(           qlong     & w, const char * buff )
{ w= (unsigned short)strtoul( buff, NULL, RADIX ); return( buff );
}
const char * fromString(  unsigned  qlong     & w, const char * buff )
{ w= (unsigned short)strtoul( buff, NULL, RADIX ); return( buff );
}

const char * fromString(           double   & w, const char * buff )
{ w= atof( buff ); return( buff );
}

const char * fromString(         float     & w, const char * buff )
{ w= atof( buff ); return( buff );
}

const char * fromString(  unsigned char *   & w, const char * buff ) { return( strcpy( (char*)w, buff )); }
const char * fromString(   const char   *   & w, const char * buff ) { return( w= strdup( buff ) );       }
const char * fromString(         char   *   & w, const char * buff )
{ return( strcpy(   w, buff ));
} /* client gives space enough ??? */


/** ====================================================[ JACS 2012-06-08 ]== *\
 *                                                                            *
 *   altomaltes@yahoo.es                                                      *
 *                                                                            *
 *  TEMPLATE load                                                             *
 *                                                                            *
 *  @brief instantiate pure type loading templates.                           *
 *                                                                            *
\* ========================================================================= **/


void * xmlconfFlexSeed= NULL;
