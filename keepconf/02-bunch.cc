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
 *  DESCRIPCION: keeps stored an bunch of values among runs
 *
 */


#include "keepconf.h"


/* Initialized values, may be seen as "default values", on config file lack
 */
static int   counter= 0;
const char * message= "hello";
unsigned char   flag= 123;

// KEEPXMLLIST( correse )  // Uncomment that comment in next an for xml version

KEEPJSNLIST( correse )
{ KEEPGLOB( counter );
  KEEPGLOB( message );
  KEEPGLOB( flag    );
};


int main( int argc, char ** argv )
{ printf( "%s has been executed %d times\n"
          "the message is %s\n"
        , *argv, counter, message );

  counter++;         // Increase executions

  return( counter );
}








