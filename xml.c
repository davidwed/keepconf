/**
 *     AUTHOR: Jose Angel Caso Sanchez, 2017   ( altomaltes@yahoo.es )
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
 *     FILE: xml.c
 *     DATE: ene 2017
 *
 *  DESCRIPCION: ligthweigth xml parser
 *               is event driven
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parse.h"



/*  Supported formats
 */
  const char * XMLTYPE= ".xml";


enum
{ PHASE_IDLE
, PHASE_START

, PHASE_TOKEN
, PHASE_TOKEN_ENTER
, PHASE_TOKEN_LEAVE
, PHASE_NAME
, PHASE_LEAVE
, PHASE_VALUE

};


void restart( KeeperRec * xml, int code )
{ xml->ptr= xml->buffItm;
 *xml->buffItm=
 *xml->buffNme=
 *xml->buffVal= 0;
  xml->phase= code;
}



static int xmlParse( KeeperRec * xml, const char * dat, int sz  )
{ while( sz-- )
  { char itm= *dat++;

    if ( xml->phase & STRIP_BLANKS ) /* Must be first, junk */
    { if ( itm <= ' ' )
      { continue;
    } }

    if ( xml->phase & EXPECT_EQSGN )
    { if ( itm == '=' )
      { BITRESET( xml->phase, EXPECT_EQSGN );
      }
      continue;
    }

    if ( xml->phase & EXPECT_QUOTE )  /* Waiting for first quote */
    { if ( itm == '"' )
      { BITRESET( xml->phase, EXPECT_QUOTE | STRIP_BLANKS );
      }
      continue;
    }


/* Build possible values
 */
    switch( xml->phase & 0xFFFFF )
    { case PHASE_IDLE:
        if ( itm != '>' )
        { continue;
        }

        xml->phase= PHASE_START;
      break;

      case PHASE_LEAVE:
        if ( itm != '>' )
        { return( -1 );
        }

        pushElement( xml->data, xml->idx        /* convey the pair (OBJECT) */
                   , xml->buffItm
                   , xml->buffNme
                   , xml->buffVal );

        xml->idx= 0;
        xml->items= 0;
        xml->ptr= xml->buffItm;
        xml->phase= PHASE_START | STRIP_BLANKS;
      break;

      case PHASE_START:
        if ( itm != '<' )
        { continue;
        }

        xml->ptr= xml->buffItm;
        *xml->buffItm=
        *xml->buffNme=
        *xml->buffVal= 0;
        xml->phase= PHASE_TOKEN;
      break;

/* Token related
 */

      case PHASE_TOKEN:
        if ( itm == '/' )
        { xml->phase= PHASE_TOKEN_LEAVE;
          continue ;
        }
        xml->phase= PHASE_TOKEN_ENTER;

      case PHASE_TOKEN_ENTER:
        *xml->ptr= 0;

        if ( itm == '>' )                         /* Premature ending, it's a non argument token*/
        { pushElement( xml->data, INTEGER_NAN     /* Exit level */
                     , xml->buffItm
                     , xml->buffNme               /* 0 */
                     , xml->buffVal );            /* 0 */
          restart( xml, PHASE_START | STRIP_BLANKS );
        }

        if ( itm > ' ' )
        { *xml->ptr++= itm;
          continue;
        }

        xml->ptr= xml->buffNme;
        xml->phase= PHASE_NAME | STRIP_BLANKS;
        xml->items= xml->idx= INTEGER_NAN;
      break;

      case PHASE_TOKEN_LEAVE:
        if ( itm != '>' )
        { *xml->ptr++= itm;
          continue;
        }

        pushElement( xml->data, xml->level     /* Exit level */
                   , NULL, NULL, NULL);
        xml->level--;
        xml->phase= PHASE_START | STRIP_BLANKS;
      continue;

/* Name related
 */
      case  PHASE_NAME:
        switch( itm )
        { case '>':
            xml->level++;
   
            pushElement( xml->data
                       , xml->idx ? xml->idx : xml->items         /* convey the pair (OBJECT) */
                       , xml->buffItm
                       , xml->buffNme
                       , xml->buffVal );
            restart( xml, PHASE_START | STRIP_BLANKS );
          continue;

          case '/':
           xml->phase= PHASE_LEAVE;
          continue;

          case '=':
           *xml->ptr= 0;
           xml->ptr= xml->buffVal;
           xml->phase= PHASE_VALUE | STRIP_BLANKS | EXPECT_QUOTE;
          break;

          default: *xml->ptr++= itm;
        }
      continue;

      case  PHASE_VALUE:
        *xml->ptr= 0;

        if ( xml->token )            /* On escape        */
        { if ( itm == ';' )          /* End escape       */
          { xml->token--;            /* out escape phase     */
            xml->ptr -= xml->token;  /* Reach the beginning  */
            xml->token= 0;           /* out escape phase     */

                 if ( !strcmp( xml->ptr, "quot" )) { itm= '"' ; }
            else if ( !strcmp( xml->ptr, "apos" )) { itm= '\''; }
            else if ( !strcmp( xml->ptr, "lt"   )) { itm= '<' ; }
            else if ( !strcmp( xml->ptr, "gt"   )) { itm= '>' ; }
            else if ( !strcmp( xml->ptr, "amp"  )) { itm= '&' ; }
            else { itm= 0;  xml->errors++; continue; }
            *xml->ptr++= itm; continue;  /* Add to serial */
          }
          xml->token++;
        }

        switch( itm )
        { case  '&' : xml->token= 1;    break;  /* Detect escape, start escape xlate */
          default   : *xml->ptr++= itm; break;  /* Add to serial                     */

          case  '"':                            /* End of item        */
           xml->ptr= xml->buffNme;

            if ( !strcmp( xml->buffNme, "items" ))  /* Test for metadata */
            { xml->idx= 0;
              xml->items= atol( xml->buffVal );
            }
            else if ( !strcmp( xml->buffNme, "idx" ))
            { xml->idx  = atol( xml->buffVal );
            }

            xml->phase= PHASE_NAME | STRIP_BLANKS;
          break;

          case  ',':         /* Test for array */
            pushElement( xml->data, xml->idx
                       , xml->buffItm
                       , xml->buffNme
                       , xml->buffVal );
            xml->idx++;                      /* convey the value ( ARRAY ) */
            xml->ptr= xml->buffVal;
          break;

        }

       break;
    } }

  return( 0 );
}


/*
 */
static char * escapeString( KeeperRec  * xml )
{ char * src;
  char * dst;

  for(  src= xml->ptr, dst= xml->buffVal
     ; *src
     ;  src++ )
  { switch( *src )
    { case '"' : strcpy( dst, "&quot;" ); dst+= 6; break;
      case '\'': strcpy( dst, "&apos;" ); dst+= 6; break;
      case '<' : strcpy( dst, "&lt;"   ); dst+= 4; break;
      case '>' : strcpy( dst, "&gt;"   ); dst+= 4; break;
      case '&' : strcpy( dst, "&amp;"  ); dst+= 5; break;
      default  : *dst++= *src;                    break;
   } }
   *dst= 0;
   return( xml->buffVal );
}

static void parserStub( KeeperRec  * xml )
{ int loop;

  if ( xml->level < 0 )
  { if ( xml->file )
    { fclose( xml->file );
      xml->file= NULL;
      return;
  } }

  for( loop= 0                /* Indent */
     ; loop<xml->level
     ; loop++ )
  { fputs( "  ", xml->file );
  }

  if ( xml->ptr )             /* Is a leave, ptr holds value */
  { if ( ! *xml->ptr )        /* Leave level                 */
    { fprintf( xml->file
             , "</%s>\n"
             , xml->buffNme );
      return;
    }

    if ( xml->idx == INTEGER_NAN )
    { escapeString( xml );

      fprintf( xml->file
             , "<%s %s=\"%s\"/>\n"
             , xml->buffNme
             , xml->buffItm
             , xml->buffVal );
    }
    else
    { escapeString( xml );

      fprintf( xml->file
             , "<%s items=\"%d\" %s=\"%s\"/>\n"
             , xml->buffNme
             , xml->idx
             , xml->buffItm
             , xml->buffVal );
  } }
  else                               /* Is a branch, so level involved */
  { if ( xml->idx == INTEGER_NAN )
    { fprintf( xml->file             /* Single value */
             , *xml->buffItm ? "<%s class=\"%s\">\n"
                             :  "<%s>\n"
             , xml->buffNme
             , xml->buffItm );
    }
    else                             /* Array */
    { fprintf( xml->file
             , "<%s %s=\"%d\" class=\"%s\">\n"
             , xml->buffNme
             , xml->idx > 0 ? "idx"    : "items"
             , xml->idx > 0 ? xml->idx : - xml->idx
             , xml->buffItm );
} } }





/** Returns the write driver, must be called on token serializing
 */
ANSIC ObjectKeeper xmlSave( KeeperRec  * xml
                          , const char * name
                          , void       * data  )
{ char * fname= alloca( strlen( name ) + 16 );

  xml->data= data;
  xml->phase= PHASE_IDLE;
  xml->idx= INTEGER_NAN;
  xml->items=
  xml->level= 0;
  xml->ptr= NULL;

  strcpy( fname, name );
  strcat( fname, ".xml"   );

  if (( xml->file= fopen( fname, "w" ) ))
  { fprintf( xml->file
           , "<?xml version=\"1.0\" ?>\n"
             "<config style=\"%s\">\n"
           , "rich" );
  }

  return( parserStub );
}

/** calls pushElement on token discover
 */
ANSIC KeepSvr xmlLoad( const char * name
                     , void       * autom )
{ char * fname= alloca( strlen( name ) + 16 );
  FILE * file;


  strcpy( fname, name );
  strcat( fname, ".xml"   );

  if (( file= fopen( fname, "rt" ) ))
  { int sz;
    char buff[ 256 ];

    KeeperRec xml;

    xml.data = autom;
    xml.phase= PHASE_IDLE;
    xml.ptr  = NULL;
    xml.items= 0;
    xml.level= 0;
    xml.idx  = INTEGER_NAN;

    do
    { sz= fread( buff, 1, sizeof( buff ), file );
      xmlParse( &xml, buff,  sz );
    } while( sz >= sizeof( buff ) );

    fclose( file );
  }

  return( xmlSave );
}

/*
 *
 */
ANSIC int xmlParseStr( KeeperRec * jsn, const char * str  )
{ return( xmlParse( jsn, str, strlen( str )));
}

ANSIC size_t xmlDigest( char    *ptr, size_t size
                      , size_t nmemb, void *userdata )
{ size *= nmemb;

  xmlParse( (KeeperRec*)userdata, ptr, size );
  return( size );
}





