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
 *     FILE: parse.h
 *     DATE: ene 2017
 *
 *  DESCRIPCION: parsers definitions
 *
 */

#include <stdio.h>
#include <malloc.h>

#ifdef _MSC_VER
   // #define alloca _malloca
  #define qlong __int64
  #define qword __uint64
  #define ulltoa _ui64toa
  #define ltoa   _ltoa
  #define itoa   _itoa
  #define gcvt   _gcvt
  #define strdup _strdup
#else
//  #include <alloca.h>
  #define qlong long long
  #define qword unsigned long long
#endif



#ifdef __cplusplus // Allow mix witth c code
  #define ANSIC extern "C"
#else
  #define ANSIC

  ANSIC const char * pushElement( void      * args
                                , int phase
                                , const char * item
                                , const char * name
                                , const char * value );

#endif

#define STREAM_OBJECT  'o'
#define INTEGER_NAN  (int)0x80000000

#define BITRESET( w, r ) w &= ~( r )

#define STRIP_BLANKS (int)0x40000000
#define EXPECT_QUOTE (int)0x20000000
#define EXPECT_EQSGN (int)0x10000000
#define ON_ESCAPE    (int)0x08000000

#define ERR_PERSIST_NOFILE   -259
#define JSON_ERR_END         -258
#define JSON_ERR_NAME        -256
#define JSON_ERR_VALUE       -257

typedef struct
{ void * data;

  char buffItm[ 256  ];
  char buffNme[ 256  ];
  char buffVal[ 4096 ];

  int array;
  int phase;

  char * ptr;

  const char * object;
  FILE       * file;

  int token;
  int level;
  int idx;

/* For XML
 */
  int items;
  int errors;

} KeeperRec;

typedef void (*ObjectKeeper)( KeeperRec *  );



typedef ObjectKeeper (*KeepSvr )( KeeperRec *
                             , const char * name
                             , void * data  );

ANSIC ObjectKeeper xmlSave( KeeperRec * jsn
                          , const char * name
                          , void * data  );

ANSIC ObjectKeeper jsnSave( KeeperRec *
                          , const char * name
                          , void * data  );


typedef KeepSvr (*KeepLdr )( const char * name
                           , void       * data  );

ANSIC KeepSvr xmlLoad( const char * name
                     , void * data  );

ANSIC KeepSvr jsnLoad( const char * name
                     , void       * data  );

ANSIC KeeperRec  *  jsonInit( KeeperRec *
                            , void      * autom );

ANSIC          int jsonParse( KeeperRec  *
                            , const char * dat, int sz  );
ANSIC       int jsonParseStr( KeeperRec  *
                            , const char * str  );
ANSIC      size_t jsonDigest( char    *ptr, size_t size
                            , size_t nmemb, void * userdata );

ANSIC KeepSvr jsnInit( KeeperRec  * keep
                     , void       * autom
                     , const char * name );

#define JSNINIT( k, w ) jsnInit( k, w, #w )

ANSIC KeepSvr jsnDone( KeeperRec  * keep );




