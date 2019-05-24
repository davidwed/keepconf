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
 *     FILE: 07-array.cc
 *     DATE: jun 2017
 *
 *  DESCRIPCION: keeps stored an array of objects among executions.
 *
 */

#include "keepconf.h"

struct Aemet
{ Aemet()
  { data="hola";
    metadata="que";
  }

  const char * data;
  const char * metadata;
};

KEEP_LOADER( Aemet )
{ KEEPITEM( data      );
  KEEPITEM( metadata  );
};

struct Aemet array[ 10 ];


 KEEPXML( array );  // This does the job ( xml version ) 
// KEEPJSN( array );  // This does the job  ( json version )

int main( int argc, char ** argv )
{ printf( "%s has %s\n"
        , array[ 9 ].metadata
        , array[ 9 ].data );

  return( 0 );
}








