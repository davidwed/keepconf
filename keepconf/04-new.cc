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
 *     FILE: 01-new.cc
 *     DATE: jun 2017
 *
 *  DESCRIPCION: retrieves a stored object from heap, thus, a new object
 *
 */

#include "keepconf.h"

class PersintentExample
{ public:
           int    aInteger;
  unsigned char   aByte;
     const char * ourString;
 unsigned short   anyType;
            int   doesNotMatter;  // This in not going to be saved
   unsigned int   uninitialized;
///
///   This is a way to have defalt values
///  when no config file is present
///

 PersintentExample()
 { aInteger= -3;
      aByte=  1;
  ourString= "Hello";
    anyType= 16384;
 }


};

PersintentExample defaultObject;

/**
 *    From below till main() is the piece of code you must add
 *  to have your variable persisted, both over xml or json
 */

KEEP_LOADER( PersintentExample )
{ KEEPITEM( aInteger      );
  KEEPITEM( aByte         );
  KEEPITEM( ourString     );
  KEEPITEM( anyType       );
  KEEPITEM( uninitialized );
}


int main( int argc, char ** argv )
{ PersintentExample * loaded= NULL;

  LOADXML( loaded ); //  LOADJSN( loaded )

  if ( !loaded )
  { fprintf( stderr
           , "Config file was not in disk. About to createa a default one\n" );
    SAVEXML( defaultObject, "loaded" ); //  SAVEJSN( defaultObject, "loaded" );
    fprintf( stderr
           , "Now, it's created, run one more time\n" );
    return( 0 );
  }


  printf( "%s has been executed %d times (%s)"
         , *argv, loaded->uninitialized, loaded->ourString );

  loaded->uninitialized++;         // Increase executions

  SAVEXML( loaded );               // SAVEJSN( loaded );  // Return to disk

  return( loaded->uninitialized );
}








