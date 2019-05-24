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

  Aemet( )  // The constructor must be capable to populate the list
  { data= "Hello"; 
    metadata= "World";
    next= NULL;
    integer= 0;
  }

// Must tell how to walk the list (toAdd==NULL) or link a new element (toAdd!=NULL)
//
  friend Aemet * nextObject( Aemet * hld
                           , Aemet * toAdd )  
  { if ( toAdd )           // Write version, link element
    { toAdd->next= NULL;   // Terminate list

      if ( hld )           // Yet seeded
      { while( hld->next ) // Reach the end of the list
        { hld= hld->next;
        }
        hld->next= toAdd;  // Link new element at the end
        return( NULL );    // Keep the seed 
      }

      return( toAdd );    
        

      //toAdd->next= hld;   // For inverse LIFO lists
      //return( toAdd );    // Actualize list holder 
    } 
    else                  // Read version, walk list 
    { return( hld ? hld->next : NULL );
  } }

  friend void buildObject( Aemet & hld )
  { fprintf( stderr, "Inizializing Data (optional) %s Meta %s\n", hld.data, hld.metadata );
  }

};

KEEP_LOADER( Aemet )
{ KEEPITEM( data      );
  KEEPITEM( metadata  );
  KEEPITEM( integer  );
};

Aemet * list= NULL;


KEEPXML( list );  // This does the job ( xml version )


int main( int argc, char ** argv )
{ if ( list )
  { puts( "yet created" );
  }
  else                   // Create a little test list
  { list= nextObject( list, new Aemet( ) ); list->integer= 1;
          nextObject( list, new Aemet( ) ); list->next->integer= 2;
          nextObject( list, new Aemet( ) ); list->next->next->integer= 3;
  }

  list->integer++;
  printf( "Test results %d %d %d\n"
        , list->integer
        , list->next->integer
        , list->next->next->integer );
  return( list->integer );
}








