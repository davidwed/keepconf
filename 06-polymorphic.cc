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
 *     FILE: 01-polymorphic.cc
 *     DATE: jun 2017
 *
 *  DESCRIPCION: Holds a list of arbitrary type objects
 *
 */

#include "keepconf.h"


/*  Below is the walker
 */

typedef struct ListRec
{ ListRec * next;

/*    Because of c++ ways, we don´t use virtualization, but object itself
 *  must be virtual. This allows C++ get the type on runtime.
 */

  virtual void linkObject( ListRec * hld )
  { hld->next= next; next= hld;
    fprintf( stderr, "Linking object %s over %s\n", typeId( *hld ), typeId( *this ) );
  }

/*  It's possible to have another type of linker for vectors, arrays, etc.
 */

/* Must tell how to walk the list
 */
  friend ListRec * nextObject(  ListRec * hld ) { return( hld->next ); }
  friend ListRec * firstObject( ListRec * hld ) { return( hld       ); }

} ListRec;



/*
 *  Now we can define our objects
 */

struct Common
{ int a;
  int b;
  int c;
};


/* An arbitrary derived type
 */
struct ListRec1: public Common, ListRec
{ int ownInteger;
  unsigned char arr[5][2];
};

/* Another one
 */
struct ListRec2: public Common, ListRec
{ int ownInteger;
  const char * str2;
};

/* A more complex one
 */
struct ListRec3: public ListRec2
{ int ownInteger;
  const char * escaped;
};


ListRec * storer06;  // Managed list


/** ===========================================================================
 *  From this point there must be added to existent code to have persistence
 */

KEEP_LOADER( ListRec )
{ KEEPLIST( next );
}

KEEP_LOADER( Common )
{ KEEPITEM( a );
  KEEPITEM( b );
  KEEPITEM( c );
}

REGISTERCLASS( ListRec1 )
{ KEEPROOT( Common );      // Process parent class

  KEEPITEM( arr );
  KEEPITEM( ownInteger );
}

REGISTERCLASS( ListRec2 )
{ KEEPROOT( Common );

  KEEPITEM( str2 );
  KEEPITEM( ownInteger );
}

REGISTERCLASS( ListRec3 )
{ KEEPROOT( ListRec2 );

  KEEPITEM( escaped );
  KEEPITEM( ownInteger );
}



/**
 *   No more needed
 * ============================================================================ */

ListRec * createDefaultConfig( )
{ ListRec * seed= new ListRec;

  ListRec1 * l1= new ListRec1;

  l1->a= 67; l1->b= 89; l1->c= 90; l1->ownInteger= 1111;
  memset(l1->arr,0,sizeof(l1->arr));
  l1->arr[0][0]= 12;
  l1->arr[1][0]= 13;
  l1->arr[2][0]= 14;
  l1->arr[3][0]= 15;
  l1->arr[4][0]= 16;
  seed->next= l1;

  ListRec2 * l2= new ListRec2;

  l2->a= 1;  l2->b= 12;   l2->c= 23; l2->ownInteger= 22222;
  l2->str2= "hello&<folks>";
  l1->next= l2;

  ListRec3 * l3= new ListRec3;

  l3->a= 1; l3->b= 12; l3->c= 23; l3->ownInteger= 333;
  l3->str2=    "this is a string";
  l3->escaped= "hello&<folks>";
  l2->next= l3;

  l3->next= NULL;

  return( seed );
}


/*
 *   This example builds a polimorphic linked list first time,
 * an modiies it on posterior executions. It allows to know the type
 * of the nodes as in java.
 */

KEEPXML( storer06 );  // This does the job ( xml version )

//KEEPJSN( storer06 );  // No json version

int main( int argc, char ** argv )
{ if ( !storer06 )     // No config on disk yet, let´s create one of them
  { fprintf( stderr, "A default config is about to be created. run this again" );
    storer06=  createDefaultConfig();
    return( 1 );
  }

//
// Iterate the recovered list,  modifying some value based on the type
//

  for( ListRec * ptr= storer06->next
     ;           ptr
     ;           ptr= ptr->next )
  { const char * id= typeId( *ptr );
    int res;

    if ( !strcmp( id, "ListRec1" ))
    { ListRec1 * lst= (ListRec1 *)ptr;
      res= lst->ownInteger++;
    }

    if ( !strcmp( id, "ListRec2" ))
    { ListRec2 * lst= (ListRec2 *)ptr;
      res= lst->ownInteger--;
    }

    if ( !strcmp( id, "ListRec3" ))
    { ListRec3 * lst= (ListRec3 *)ptr;
      res= lst->ownInteger+=3;
    }

    fprintf( stderr, "Modifyng %s element to %d\n", id, res );
  }


    return( 0  );
}








