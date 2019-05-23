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
 *     FILE: 05-list.cc
 *     DATE: jun 2017
 *
 *  DESCRIPCION: keeps stored an integer value among executions.
 *
 */

#include "keepconf.h"

struct Aemet
{ Aemet * next;
  const char * data;
  const char * metadata;
  int integer;

  Aemet( Aemet * prev )  // The constructor must be capable to populate the list
  { next= prev;
  }

/* Must tell how to walk the list
 */
  friend Aemet * nextObject(  Aemet * hld )
  { return( hld->next );
  }

  friend void buildObject( Aemet & hld )
  { fprintf( stderr, "Inizializing Data %s Meta %s\n", hld.data, hld.metadata );
  }

};

KEEP_LOADER( Aemet )
{ KEEPITEM( data      );
  KEEPITEM( metadata  );
  KEEPITEM( integer  );
};

Aemet * list= NULL;


 KEEPJSNLST( list );  // This does the job ( xml version )


int main( int argc, char ** argv )
{ if ( list )
  { puts( "yet created" );
  }
  else                   // Create a little test list
  { list= new Aemet( list ); list->integer= 1; list->data= "Hello"; list->metadata= "World";
    list= new Aemet( list ); list->integer= 2; list->data= "Hello"; list->metadata= "World";
    list= new Aemet( list ); list->integer= 3; list->data= "Hello"; list->metadata= "World";
  }

  list->integer++;
  printf( "Test results %d %d %d\n"
        , list->integer
        , list->next->integer
        , list->next->next->integer );
  return( list->integer );
}








