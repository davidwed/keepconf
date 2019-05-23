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
  
  Aemet()
  { data="hola123";
    metadata="que124";
    next= NULL;
    integer= 0;
  }

  Aemet( Aemet * prev )
  { next= prev;
    data="hola123";
    metadata="que124";
    integer= 0;
  }
  
/* Must tell how to populate the list
 */
  friend void linkObject( Aemet * hld )
  { //hld->next= next; next= hld;
   // fprintf( stderr, "Linking object %s over %s\n", typeId( *hld ), typeId( *this ) );
  }

/* Must tell how to walk the list
 */
  friend Aemet * nextObject(  Aemet * hld ) 
  { return( hld->next ); 
  }

  friend void buildObject( Aemet & hld )
  { fprintf( stderr, "Data %s Meta %s\n", hld.data, hld.metadata );
  }

};

KEEP_LOADER( Aemet )
{ KEEPITEM( data      );
  KEEPITEM( metadata  );

  KEEPITEM( integer  );
};



Aemet ** list; 

// KEEPJSN( list );  // This does the job ( json version )


KEEPXML( list );  // This does the job ( xml version )


int main( int argc, char ** argv )
{ if ( list ) 
  { puts( "yet created" );
  }
  else
  { *list= new Aemet; 
  }
  return( 0 );
}








