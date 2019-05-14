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
 *     FILE: json.c
 *     DATE: ene 2017
 *
 *  DESCRIPCION: ligthweigth json parser
 *               is event driven
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parse.h"



/*  Supported formats
 */
  const char * JSNTYPE= ".json";


enum
{ PHASE_IDLE

, PHASE_NAME
, PHASE_ENDN

, PHASE_HUNTVAL
, PHASE_NUMBER   = 'N'
, PHASE_TRUE     = 'T'
, PHASE_FALSE    = 'F'
, PHASE_STRING   = 'S'

, PHASE_ENDVALUE = '}'
, PHASE_HUNT     = ','


};



int jsonParse( KeeperRec * jsn, const char * dat, int sz  )
{ while( sz-- )
  { char itm= *dat++;

//    fputc( itm, stderr ); fflush( stderr );

/* Build pooible values
 */
    switch( jsn->phase )
    { case PHASE_NAME:
        if ( itm == '"' )
        { *jsn->ptr= 0;
          jsn->phase= PHASE_ENDN;
        }
        else
        { *jsn->ptr++= itm;
        }
      continue;

      case PHASE_STRING:
        if ( itm == '"' )
        { if ( jsn->ptr == jsn->buffVal )
          { *jsn->ptr++= " ";             
          }
          *jsn->ptr= 0;
          jsn->phase= PHASE_ENDVALUE;
        }
        else
        { *jsn->ptr++= itm;
        }
      continue;

      case PHASE_TRUE: 
      case PHASE_FALSE:
        if ( itm > 32 )
        { continue;
        }
        jsn->phase= PHASE_ENDVALUE;
      break;

      case PHASE_NUMBER: switch( itm )
      { case '.': case '-': 
        case '0': case '1': case '2': case '3': case '4':   
	       case '5': case '6': case '7': case '8': case '9':
          *jsn->ptr++= itm;
        continue;

        default: 
          jsn->phase= PHASE_ENDVALUE;
        break;
    } }

/* Remove laying chars
 */
    if ( itm <= 32 )
    { continue;
    }

/* Take paths
 */
    switch( jsn->phase )
    { case PHASE_IDLE: switch( itm )
      { case '{': jsn->phase= PHASE_HUNT; break;
        default: return( -3 );
      }
      break;

      case PHASE_HUNT: switch( itm )
      { case '"':
          jsn->ptr= jsn->buffNme; *jsn->ptr= 0;
          jsn->phase= PHASE_NAME;
        break;

        case '{':   /* Object coming from an array */
          pushElement( jsn->data, jsn->items++
                     , jsn->buffItm // "@"
                     , jsn->buffNme
                     , jsn->buffVal ); /* Tell enter object  */
        break;

        default: return( -2 );
      }
      break;

      case PHASE_ENDN: switch( itm )
      { case ':':
          jsn->array= INTEGER_NAN;
          jsn->phase= PHASE_HUNTVAL;
        break;

        default: return( JSON_ERR_NAME );
      }
      break;

/* Capture values
 */
      case PHASE_HUNTVAL: switch( itm )
      { case '[':                          /* Catch an array */
          *jsn->buffVal= 0;
           jsn->array= INTEGER_NAN;
        break;

        case '{':                          /* Enter object */
          jsn->phase= PHASE_HUNT;
 
          if ( jsn->array == INTEGER_NAN ) /* Compromised solution. Non array objects can omit typename */
          { pushElement( jsn->data, INTEGER_NAN /* Start of object */
                       , jsn->buffNme
                       , "class"         /* Tell enter object  */
                       , "" );           /* Tell enter object  */
            jsn->items= 1;               /* Mark first */
          }
          else
          { strcpy( jsn->buffItm         /* Save object name   */
                  , jsn->buffNme );
            jsn->items= jsn->array;      /* Save index         */
          }

          jsn->ptr= NULL;                /* Yet delivered      */
        break;      

//        case '{':                   /* Enter object */
  //        jsn->phase= PHASE_HUNT;
    //      pushElement( jsn->data, jsn->array == INTEGER_NAN ? INTEGER_NAN : *jsn->buffNme
      ///             , jsn->buffNme
         //          , "class"             /* Tell enter object  */
           //        , "" );     /* Tell enter object  */
    //      jsn->ptr= NULL;  /* Yet delivered */
      //  break;



        case '"':
          jsn->ptr= jsn->buffVal;
          jsn->phase= PHASE_STRING;
        break;

        case '.': case '-' :
        case '0' ... '9':
          jsn->ptr= jsn->buffVal; *jsn->ptr++= itm;
          jsn->phase= PHASE_NUMBER;
        continue;

        case 't': case 'T':
          jsn->ptr= jsn->buffVal; *jsn->ptr++= '1'; *jsn->ptr= 0;
          jsn->phase= PHASE_TRUE;
        continue;

        case 'f': case 'F':
          jsn->ptr= jsn->buffVal; *jsn->ptr++= '0'; *jsn->ptr= 0;
          jsn->phase= PHASE_FALSE;
        continue;

        default: 
         puts("err");
         return( JSON_ERR_VALUE );
      }
      break;

/*  Tree goes back
 */

      case PHASE_ENDVALUE: switch( itm )
      { case ']':
          if ( jsn->ptr )
          { pushElement( jsn->data, jsn->array /* Pending value */
                       , jsn->buffNme
                       , jsn->buffItm
                       , jsn->buffVal );       /* Stamp value */
            jsn->ptr= NULL;                    /* Mark as delivered */
            jsn->array= INTEGER_NAN;
          }
        break;

        case ',': case '}':
           if ( jsn->ptr )
           { *jsn->ptr=  0;

             if (*jsn->buffNme== '$')                  /* Metadata */
             { if ( !strcmp( jsn->buffNme, "$type" ))
               { pushElement( jsn->data
                            , jsn->items               /* Saved index */
                            , jsn->buffItm             /* Saved name */
                            , "class"
                            , jsn->buffVal );          /* Tell enter object  */

             } }
             else
             { pushElement( jsn->data, jsn->array++  /* Pending value           */
                          , jsn->buffNme
                          , ""                      /* No type on leaves       */
                          , jsn->buffVal );         /* Stamp value             */
             }

             if ( jsn->array > 0 )                 /* Iterating a leave array */
             { jsn->phase= PHASE_HUNTVAL;          /* Close object            */
               jsn->ptr= jsn->buffVal;             /* Mark as delivered       */
               break;
             }
             jsn->ptr= NULL;                       /* Mark as delivered */
           }

           if ( itm == '}' )
           { const char * name= pushElement( jsn->data, 0
                                           , NULL , NULL, NULL );
             if ( name )                           /* Recover object */
             { strcpy( jsn->buffNme, "class" );
               strcpy( jsn->buffItm, name );
               *jsn->buffVal= 0;
           } }
           jsn->phase= itm;             /* Close object */
        break;

        default: return( JSON_ERR_END );
  } } }

  return( 0 );
}

/*
 */
static void jsnEnter( KeeperRec * jsn )
{ int loop; int brck= ' ';

  if ( jsn->level < 0 )
  { if ( jsn->file )
    { fclose( jsn->file );
      jsn->file= NULL;
      return;
  } }

  if ( jsn->token != INTEGER_NAN )
  { for( loop= 0                   /* Indent */
       ; loop< jsn->level
       ; loop++ )
    { fputs( "  ", jsn->file );
  } }

  int bunch= ( jsn->level - jsn->token ) ?  '{' : ',' ;

  jsn->token= jsn->level;

  if ( !strcmp( jsn->buffItm, "str" ) )
  { brck= '"';
  }

  if ( jsn->ptr )                  /* Leave */
  { if ( !*jsn->ptr )
    { fprintf( jsn->file, jsn->idx ? "  }\n"  : "  } ]\n" );

      if ( jsn->idx  )        // JASON has not global set
      { if ( jsn->idx == INTEGER_NAN  )        // JASON has not global set
        { fprintf( jsn->file, "}\n" );
      } }
      else
      { fprintf( jsn->file, "}\n" );
      }

      return;
    }

    if ( jsn->idx == INTEGER_NAN ) /* Pure value */
    { fprintf( jsn->file
             , "%c \"%s\": %c%s%c\n"
             , bunch
             , jsn->buffNme
             , brck, jsn->ptr
             , brck );

      if ( !jsn->level ) // JASON has not global set
      { fprintf( jsn->file, "}\n" );
      }

      return;
    }

    fprintf( jsn->file
           , "%c \"%s\": [%c%s%c]\n"
           , bunch
           , jsn->buffNme
           , brck, jsn->ptr
           , brck );
    return;
  }

/* Is an object
 */

   if ( jsn->idx > 0 )
   { fprintf( jsn->file
            , "%c "
            , bunch );
     fflush(jsn->file) ;
     jsn->token= INTEGER_NAN;
   }
   else
   { if ( jsn->idx == INTEGER_NAN  )
     { fprintf( jsn->file
              , "%c \"%s\":\n"
              , bunch
              , jsn->buffNme );
     }
     else
     { fprintf( jsn->file
              , "%c \"%s\": [\n"
              , bunch
              , jsn->buffNme );
       strcpy( jsn->buffVal, jsn->buffItm );  /* Type must be stamped */
} } }

/** Returns the write driver, must be called on token serializing
 */
ANSIC ObjectKeeper jsnSave( KeeperRec  * jsn
                          , const char * name
                          , void       * data  )
{ char * fname= alloca( strlen( name ) + 16 );

  jsn->ptr= NULL;
  jsn->token= 1;
  jsn->phase= PHASE_IDLE;

  strcpy( fname, name   );
  strcat( fname, ".jsn" );

  if (( jsn->file= fopen( fname, "wt" ) ))
  {
  }

  return( jsnEnter );
}

ANSIC KeepSvr jsnDone( KeeperRec * keep )
{ //jsonParse( &keep, "}",  1 );

  return( jsnSave );
}

ANSIC KeepSvr jsnInit( KeeperRec  * keep
                     , void       * autom 
                     , const char * name )
{ char buff[ 256 ]; int sz;
  
  keep->phase= PHASE_IDLE;
  keep->ptr  = NULL;
  keep->data = autom;

  pushElement( keep->data
             , INTEGER_NAN
             , "config"
             , "style"
             , "json" );  /* Tell enter object  */
 // sz= sprintf( buff, "{ \"%s\": \n", name ); 
 // jsonParse( keep, buff,  sz );

  return( jsnSave );
}


/** calls pushElement on token discover
 */
ANSIC KeepSvr jsnLoad( const char * name
                     , void       * autom )
{ char * fname= alloca( strlen( name ) + 16 );
  FILE * file;

  strcpy( fname, name );
  strcat( fname, ".jsn"   );

  if (( file= fopen( fname, "rt" ) ))
  { int sz;
    char buff[ 4098 ];

    KeeperRec keep; jsnInit( &keep, autom, name ); 

    do
    { sz= fread( buff, 1, sizeof( buff ), file );
      jsonParse( &keep, buff,  sz );
    } while( sz >= sizeof( buff ) );

    fclose( file );
    return( jsnDone( &keep ) );
  }

  return( jsnSave );
}


/*
 *
 */
ANSIC int jsonParseStr( KeeperRec * jsn, const char * str  )
{ return( jsonParse( jsn, str, strlen( str )));
}

ANSIC size_t jsonDigest( char    *ptr, size_t size
                       , size_t nmemb, void *userdata )
{ size *= nmemb;

  jsonParse( (KeeperRec*)userdata, ptr, size );
  return( size );
}


