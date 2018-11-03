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
 *     FILE: 01-integer.cc
 *     DATE: jun 2017
 *
 *  DESCRIPCION: Adds persiting capabilities to an object
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
/**
 *   This is a way to have defalt values
 *  when no config file is present
 */

 PersintentExample()
 { aInteger= -3;
      aByte=  1;
  ourString= "Hello";
    anyType= 16384;
 }

};

PersintentExample objectExample;

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


KEEPXML( objectExample );  // This does the job ( xml version )
//KEEPJSN( objectExample );  // This does the job ( json version )


/**
 *   A builder for the loaded objects can be added,
 * but is optional
 */
void buildObject( PersintentExample & obj )
{ obj.doesNotMatter= 5;
  fprintf( stderr
         , "#\n"
           "# %s has been built\n"
           "#\n\n"
         , typeId( obj ));
}


int main( int argc, char ** argv )
{ printf( "%s has been executed %d times"
        , *argv, objectExample.uninitialized );

  objectExample.uninitialized++;         // Increase executions

  return( objectExample.uninitialized );
}








