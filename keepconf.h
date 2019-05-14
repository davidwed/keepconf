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
 *     FILE: keepconf.h
 *     DATE: jun 2013
 *
 *  DESCRIPCION: arbitrary format config file maintainer.
 *
 */

#ifndef KEEPCONF_H_INCLUDED
#define KEEPCONF_H_INCLUDED

#include <alloca.h>
#include "parse.h"

#ifdef __cplusplus // Allow mix witth c code

#include <typeinfo>
#include <string.h>

#define INTEGER_NAN (int)0x80000000
#define RADIX 10

#define OBJECT_SAVE  0x1
#define OBJECT_LOAD  0x0
#define OBJECT_BUILD  -1

#ifdef Uses_opstream
 extern StreamableInit si;
#else
 #define si
#endif

enum  ITEM_DONE
{ ITEM_VOID= 0
, ITEM_FAULT   // From here, not this item
, ITEM_NAMES
, ITEM_KEYS
, ITEM_NONE
, ITEM_OBJS
, ITEM_OBJECT
, ITEM_LINK
, ITEM_NEXT
, ITEM_BUILT

, ITEM_LOADED
, ITEM_WRITTEN
};

#ifdef _MSC_VER
  #define qlong __int64
  #define qword __uint64
  #define ulltoa _ui64toa
  #define ltoa   _ltoa
  #define itoa   _itoa
  #define gcvt   _gcvt
  #define strdup _strdup
#else
  #define qlong long long
  #define qword unsigned long long
#endif


/*  Supported formats
 */
  extern const char * XMLTYPE;
  extern const char * JSNTYPE;


  extern "C" int strcasecmp( const char *s1, const char *s2);
  extern "C" char   *strdup( const char *s );

/* Try to standarize this key macro. Same objects must generate same object names.
 */


template < class t > inline const char * typeId( const t & obj )
{ const char * ptr= typeid( obj ).name();

#ifdef __GNUG__
  if ( *ptr == 'P' ) ptr++;
  while( *ptr && (*ptr < 'A' ))
  { ptr++;
  }
#endif

#ifdef _MSC_VER
  const char * needle;
  while( needle= strchr( ptr, ' ' ) )
  { ptr= needle+1;
  }
#endif

  return( ptr );
}

/* Temporary reference to an int
 */

struct intRef
{ int i;

  intRef( int z ) { i= z; }
  operator int & () { return( i ); }

};


/* default (void) builder
 */
template <class T> void buildObject( T & obj )
{ // fprintf( stderr, "%s does not need build\n", typeId( obj ) );
}

/* semantic helpers. Pure types must de coded separately
 */

inline const char * typeId( const bool           & type ) { return( "bool"  );}
inline const char * typeId( const char           & type ) { return( "char"  );}
inline const char * typeId( const short          & type ) { return( "short" );}
inline const char * typeId( const int            & type ) { return( "int"   );}
inline const char * typeId( const long           & type ) { return( "long"  );}
inline const char * typeId( const qlong          & type ) { return( "llong" );}

inline const char * typeId( const unsigned char  & type ) { return( "byte"  );}
inline const char * typeId( const unsigned short & type ) { return( "word"  );}
inline const char * typeId( const unsigned int   & type ) { return( "uint"  );}
inline const char * typeId( const unsigned long  & type ) { return( "dword" );}
inline const char * typeId( const unsigned qlong & type ) { return( "qword" );}

inline const char * typeId(               char * & type ) { return( "str"   );}
inline const char * typeId(         const char * & type ) { return( "str"   );}

inline const char * typeId(                float & type ) { return( "float" );}
inline const char * typeId(               double & type ) { return( "double");}

/* =====================================================[ JACS 2012-06-08 ]== *\
 *                                                                            *
 *   altomaltes@yahoo.es                                                      *
 *                                                                            *
 *  CLASS: KeeperRec                                                         *
 *                                                                            *
 *  @brief To manage object storing.                                          *
 *                                                                            *
\* ========================================================================== */
struct ObjConfRec;

typedef const void * (*KeepLoader)( ObjConfRec &, void * );

const void * dummyLoader( ObjConfRec &, void * );



/*  Registering object types
 */

extern void * xmlconfFlexSeed;


template <class T> T * newObject( )
{ T * obj= new T( si ) ;

  return( obj );
}

template < class T > struct Registrar
{ void       * next;
  const char * name;

 /* the functors
  */

  T  * ( * create  )( );
  KeepLoader loader;

  Registrar< T > * getNext( )
  { if ( this )
    { return( ( Registrar< T > * ) this->next );
    }

    return( ( Registrar< T > * ) xmlconfFlexSeed );   // Tricky
  }

  Registrar( const char * nm, ITEM_DONE( *ld)( ObjConfRec & cnf, T & hld )  )   // Write version
  { name= nm;
    next= xmlconfFlexSeed;
    xmlconfFlexSeed= this;

    create= ::newObject;
    loader= reinterpret_cast< KeepLoader >( ld );
  }

};



struct ObjLevelRec
{ int          index;
  const char * objs;
  const char * names;

  KeepLoader   loader;
  void       * holder; // Holder for object about to be created

  ITEM_DONE test( const char * name, const char * type )
  { if ( names )
    { if ( strcasecmp( name, names ))
      { return( ITEM_NAMES );   /* Not this object, try next  */
    } }

    if ( objs )
    { if ( *objs )
      { if ( strcasecmp(  type, objs  ))
        { return( ITEM_OBJS  );   /* Not this object, try next  */
    } } }

    return( ITEM_VOID );
  }

};


const void * test( ObjLevelRec * info, const char * name, const char * type );

template <class T> Registrar< T > * findClassByName( const char * name )
{ for( Registrar < T > * ptr= (Registrar < T > *)xmlconfFlexSeed
     ;                   ptr
     ;                   ptr= (Registrar < T > *)ptr->next )
  { if ( !strcmp( ptr->name, name ))
    { return( ptr );
  } }

  return( NULL );
}




/*  The struct starts here
 */




  template <typename T>  ITEM_DONE loadObject( struct ObjConfRec & cnf, T & t )
  { void load( ObjConfRec &, T & obj );

    if ( cnf.code == OBJECT_LOAD )
    { if ( !cnf.info->objs )
      {// cnf.info->objs= typeId( t );    // JSON lacks this
      }

      if ( !cnf.value )                   // closing object
      { buildObject( t );
        return( ITEM_BUILT );
    } }

    load( cnf, t );
    return( ITEM_LOADED );              // Load or save
  }

struct ObjConfRec
{ int code;
  const char  * value;

  ObjLevelRec   levels[ 256 ];
  ObjLevelRec * info;
  ObjectKeeper  parserStub;    // Writer
  KeeperRec     xmlData;       // Writer

  ObjConfRec( )
  { value= NULL;
    info= levels;
    memset( levels, 0, sizeof( levels ));
  }

/*  Save members
 */

  void enter( const char * id
            , const char * name
            , const char * value
            , int idx );

  const char * leave( int idx );

  template <class T, int els> ITEM_DONE save( T (& t)[els], const char * key, const char * id ) // ene 2019
  { void load( ObjConfRec &, T & );

    int idx= 0; while( idx < els )
    { enter( id, key, NULL, idx ? idx : -els );
      load( *this, t[ idx ] );
      idx++;
      leave( idx < els ? idx : 0  );
    }    
   
    return( ITEM_VOID ); 
  }

  template <class T> ITEM_DONE save( T & t, const char * key, const char * id )
  { void load( ObjConfRec &, T & );

    enter( id, key, NULL, INTEGER_NAN );
    if ( &t )               // Test for void object
    { load( *this, t );
    }
    leave( INTEGER_NAN );
    return( ITEM_VOID );
  }

/*
 */
  template < class T
           , class O > ITEM_DONE save( T * & t, const char * key, O & streamer )
  { struct HolderRec
    { HolderRec      * next;

      const char     * type;
      T              * item;
      Registrar< T > * fact;
    };

    T          * ptr= firstObject( t );
    HolderRec  * list= NULL;
    int loop= 0;

    while( ptr )
    { HolderRec * hld= (HolderRec *)alloca( sizeof( HolderRec ) );

      hld->item=          ptr;                        // Whath ? execution time resolves name
      hld->type= typeId( *ptr );                      // Whath ? execution time resolves name
      hld->fact= findClassByName<T>( hld->type );     // Search for a suitable builder

      if ( ! hld->fact )
      { fprintf( stderr
               , "You must register the streamable class \"%s\"\n"
               , hld->type );
      }
      else
      { hld->next= list;
        list= hld; loop--;
      }
      ptr= nextObject( ptr );
    }

    while( list )
    { enter( list->type, key, NULL, loop ); //??
      list->fact->loader( *this, list->item );
      leave( list->next ? loop : 0 );

      if ( loop < 0 )
      { loop= 0;
      } loop++;
      list= list->next;
    }

    return( ITEM_WRITTEN );
  }

  template <class T> KeepLoader loadObjectCast(  )
  { return( reinterpret_cast< KeepLoader >( loadObject<T> ));
  } 

  template <class T> ITEM_DONE load( T *& t, const char * key, const char * id )
  { if ( strcmp( info->names, key ))
    { return( ITEM_NAMES );
    }

    if ( ! t )
    { t= new T( si );
    }

    info->holder= t;
    info->loader= loadObjectCast<T>();

    return( ITEM_VOID );
  }

  template <class T, int els>ITEM_DONE load( T (& t)[els], const char * key, const char * id ) // ene 2019
  { if ( strcmp( info->names, key ))
    { return( ITEM_NAMES );
    }

    int idx= info->index >= els ? 0 : info->index; // Array size overloaded

    if ( idx == INTEGER_NAN )                      // Array size unknown, start of object
    { idx= 0;
    }

    if ( idx < 0 )
    { fprintf( stderr, "Index of %d (%d) for %s %s\n"
             , info->index, els
             , key, id ); 
      info->holder= t + idx;
    }
    
    info->holder= t + idx;
    info->loader= loadObjectCast<T>();

    return( ITEM_VOID );
  }

  template <class T> ITEM_DONE load( T & t, const char * key, const char * id )
  { if ( strcmp( info->names, key ))
    { return( ITEM_NAMES );
    }

    info->holder= &t;
    info->loader= loadObjectCast<T>();

    return( ITEM_VOID );
  }


/* For amorphous collections
 */

  template <class T
           ,class O> const ITEM_DONE load( T * & t, const char * key, O & streamer )
  { Registrar< T > * fact;

    if ( ! info->objs )       /* No file */
    { return( ITEM_VOID );
    }

    if ( strcasecmp( info->names, key  ))
    { return( ITEM_NAMES );   /* Not this object, try next  */
    }

    fact= findClassByName<T>( info->objs ); // Search for a suitable builder

    if ( !fact )
    { fprintf( stderr, "Class not registered for streaming\n"
                       "put \"REGISTERCLASS( %s );\" on your code\n\n"
             , info->objs );
      return( ITEM_OBJECT );
    }

    T * newObj= fact->create();

//    if ( t == streamer )               // No owner dependent
//    { newObj->linkObject( streamer );
//    }
//    else                               // Owner dependent
//    { streamer->linkObject( newObj );
//    }

    streamer.linkObject( newObj );

    info->holder= newObj;
    info->loader= fact->loader;

    return( ITEM_BUILT );    // All done, catch
  }


/* Root objects must indicate type, because of flawn on typeid
 */

  template <class T> ITEM_DONE root( T & t, const char * key )
  { switch( code )
    { case OBJECT_SAVE: return( save( t, key, "" ));  //  SAVE version
      case OBJECT_LOAD: return( load( t, key, "" ));  //  LOAD version
    }
    return( ITEM_VOID );
  }

  template <class T> ITEM_DONE launch( T  & t, const char * key )
  { switch( code )
    { case OBJECT_SAVE: return( save( t, key, typeId( t )));  //  SAVE version
      case OBJECT_LOAD: return( load( t, key, typeId( t )));  //  LOAD version
    }
    return( ITEM_VOID );
  }

/* For heap objects
 */
  template <class T> ITEM_DONE launch( T * & t, const char * key )
  { switch( code )
    { case OBJECT_SAVE:               // SAVE version
        save( *t, key, typeId( t ) ); // Same as allocated object
      break;

      case OBJECT_LOAD:
        if ( strcmp( info->names, key ))
        { return( ITEM_NAMES );
        }

        t= new T( si );                    // Must create on load
        info->holder= t;                   // Tell engine also
        info->loader= loadObjectCast<T>();
      return( ITEM_LOADED );
    }
    return( ITEM_VOID );
  }

/* For arrays
 */

//  template <class T> ITEM_DONE launch( T * t, const char * key, int els )
//  { switch( code )
//    { case OBJECT_SAVE: return( save( t, key, els ));  //  SAVE version
//      case OBJECT_LOAD: return( load( t, key, els ));  //  LOAD version
//    }
//    return( ITEM_VOID );
//  }

  template <class T, int els> ITEM_DONE launch( T (&t)[els], const char * key )
  { switch( code )
    { case OBJECT_SAVE: return( save( t, key,key ));  //  SAVE version
      case OBJECT_LOAD: return( load( t, key,key ));  //  LOAD version
    }
    return( ITEM_VOID );
  }

  template< class T
          , class O> ITEM_DONE launch( T * & t, const char * key, O & streamer )
  {  switch( code )                   //  LDROOT must be skipped
    { case OBJECT_LOAD: return( load( t, key, streamer )); //  LOAD version
      case OBJECT_SAVE://if ( *info[-1].objs )
                        return( save( t, key, streamer )); //  SAVE version
    }
    return( ITEM_VOID );
  }


  template < class T > ITEM_DONE launchStd( T * t, const char * key
                                          , int x0
                                          , int y0= 1
                                          , int z0= 1 )
  { const char * id= typeId( *t );
    const char * fromString( T &, const char  * );
    const char *   asString( char * buff, T  w );
             T * itm= t;
        int els= x0*y0*z0;

    switch( code )
    { case OBJECT_SAVE:
      { int x,y,z;
        char * buff= (char*)alloca( 64 * x0*y0*z0 + 64 );
        char * ptr= buff;

        for( z= 0; z < z0; z++ )
        { if ( z )
          { *ptr++= ','; *ptr++= ' '; *ptr++= ' ';
          }
          for( y= 0; y < y0; y++ )
          { if ( y )
            { *ptr++= ','; *ptr++= ' ';
            }
            for( x= 0; x < x0; x++ )
            { if ( x ) { *ptr++= ','; }

              ptr+= sprintf( ptr, asString( ptr+64, *itm ));
              itm++;
        } } }

        enter( id, key, buff, x*y*z );
      }
      return( ITEM_WRITTEN );

      case OBJECT_LOAD:        //  LOAD version
        if ( info->test( key, id ) )
        { return( ITEM_KEYS );
        }

        if ( info->index == INTEGER_NAN )  // one time dump
        { //strcpy( itm, value );
        }
        else if ( info->index < els )      // Sanity check
        { fromString( itm[ info->index ], value );
        }

      return( ITEM_LOADED );
    }

    return( ITEM_VOID );
  }

  template < class T > ITEM_DONE launchStd( T & t, const char * key )
  { const char * id= typeId( t );
    const char * fromString( T &, const char  * );
    const char *   asString( char * buff, T w );

    switch( code )
    { case OBJECT_SAVE:
      { char buff[ 64 ];
        asString( buff, t ) ;
        enter( id, key, buff, INTEGER_NAN );       // LOAD version
      }
      return( ITEM_WRITTEN );

      case OBJECT_LOAD:
        if ( info->test( key, id ) )
        { return( ITEM_KEYS );
        }

        fromString( t, value );
      return( ITEM_LOADED );
      }
      return( ITEM_VOID );
  }

/* Leaf types
 */
  ITEM_DONE launch(          qlong (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch( unsigned qlong (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch(            int (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch( unsigned   int (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch(          short (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch( unsigned short (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch(           char (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch( unsigned  char (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch(           bool (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch(  const   char* (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch(          char* (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch(         double (&t), const char * id ) { return( launchStd( t, id )); }
  ITEM_DONE launch(          float (&t), const char * id ) { return( launchStd( t, id )); }

 // template < class T, int x >
   // ITEM_DONE launch( T (&w)[ x ]         , const char * id ) { return( launchStd( (T*)w, id, x )); }


  template < class T, int y, int x >
    ITEM_DONE launch( T(&w)[ y ][ x ]     , const char * id ) { return( launchStd( (T*)w, id, x,y ));  }

  template < class T, int y, int x, int z >
    ITEM_DONE launch( T(&w)[ y ][ x ][ z ], const char * id ) { return( launchStd( (T*)w, id, x,y,z ));  }



/* For 1D sized vectors
 */

  template < int x > ITEM_DONE launch(           qlong (&w)[ x ], const char * id ) { return( launchStd( w, id, x ));}
  template < int x > ITEM_DONE launch(  unsigned qlong (&w)[ x ], const char * id ) { return( launchStd( w, id, x ));}
  template < int x > ITEM_DONE launch(             int (&w)[ x ], const char * id ) { return( launchStd( w, id, x ));}
  template < int x > ITEM_DONE launch(  unsigned   int (&w)[ x ], const char * id ) { return( launchStd( w, id, x ));}
  template < int x > ITEM_DONE launch(            long (&w)[ x ], const char * id ) { return( launchStd( w, id, x ));}
  template < int x > ITEM_DONE launch(  unsigned  long (&w)[ x ], const char * id ) { return( launchStd( w, id, x ));}
  template < int x > ITEM_DONE launch(           short (&w)[ x ], const char * id ) { return( launchStd( w, id, x ));}
  template < int x > ITEM_DONE launch(  unsigned short (&w)[ x ], const char * id ) { return( launchStd( w, id, x ));}
  template < int x > ITEM_DONE launch(            char (&w)[ x ], const char * id ) { return( launchStd( w, id, x ));}
  template < int x > ITEM_DONE launch(  unsigned  char (&w)[ x ], const char * id ) { return( launchStd( w, id, x ));}


};



/** ================================================= [ JACS, 10/01/2006 ] == *\
 *                                                                            *
 *   JASC 2006                                                                *
 *                                                                            *
 *  TEMPLATE load                                                             *
 *                                                                            *
 *  @brief Here is the magic. both load save & search templates.              *
 *                                                                            *
 *     Pure type templates are instantiated one by one.                       *
 *                                                                            *
 *    Templates from user types are instantiated "on the fly". There is an    *
 *  instantiation for data and another for arrays of data, since their xml    *
 *  representation differs.                                                   *
\* ========================================================================= **/


#define REGISTERCLASS( T )  \
Registrar< T > REGISTEREDCLASS##T( #T, loadObject<T> ); \
void load( ObjConfRec & cnf, T & hld )



/** ====================================================[ JACS 2012-06-08 ]== *\
 *                                                                            *
 *   altomaltes@yahoo.es                                                      *
 *                                                                            *
 *  TEMPLATE XmlPersist                                                       *
 *                                                                            *
 *  @brief Make variable persistent.                                          *
 *                                                                            *
 *  @info                                                                     *
 *    stub( reinterpret_cast< XlmLoad >                                       *
 *            ( static_cast< int (*)( ObjConfRec *, T (& t), const char * )>  *
 *             (::load) ))                                                    *
 *                                                                            *
\* ========================================================================= **/

template < class T > ITEM_DONE startObject( ObjConfRec & cnf, T & hld )
{ return( cnf.launch( hld, cnf.info->names ));
// return( loadObject( cnf, hld ));
}


template < class T > struct ObjStorer: public ObjConfRec
{ void startLevels( T           & varObj
                  , const char  * varName )
  { levels->holder= &varObj;
    levels->loader= reinterpret_cast< KeepLoader >( startObject<T> );
    levels->names = varName;
    levels->objs  = typeId( varObj );

    info= levels;
  }

  void save( KeepSvr      svr
           , const char * varName
           , T          & varObj )
  { memset( &xmlData, 0, sizeof( KeeperRec ));

    code   = OBJECT_SAVE;
    parserStub= svr( &xmlData
                   , varName
                   , this );
    startLevels( varObj, varName );
    launch( varObj, varName );
    xmlData.level= -1;
    parserStub( &xmlData );      /* Close stream */
  }

/* Save constructor
 */
  ObjStorer( T          & varObj
           , const char * varName
           , KeepSvr      svr )
  { save( svr, varName, varObj );
  }

/* Load constructor
 */
  ObjStorer( T          & varObj
           , const char * varName )
  { code      = OBJECT_LOAD;
    info      = levels;
    parserStub= NULL;

    startLevels( varObj, varName );
  }

};


template < class T > struct ObjKeeper: public ObjStorer< T >
{ KeepSvr      saver;
  T          & varObj;
  const char * varName;

  ObjKeeper( T          & vObj          // Load object
           , const char * vName
           , KeepLdr      loader )
           : ObjStorer<T>( vObj, vName )
           , varObj(  vObj  )
           , varName( vName )
  { saver= loader( varName
                 , this );  // Tell stub
  }

  ~ObjKeeper(  )                          // Save object on exit
  { ObjStorer<T>::save( saver, varName, varObj );
  }
};


template < typename T > const void * copy( const char * varName, T & toSave
                                         , KeepSvr      saver
                                         , const char * file= NULL  )
{ ObjStorer<T> storer( toSave
                     , file ? file : varName
                     , saver );
  return( 0 );
}

/* Load version
 */
template < typename T > int copy( T & toLoad,  const char * varName
                                , KeepLdr      loader
                                , const char * file = NULL  )
{ ObjStorer<T> storer( toLoad, varName );

  return( ! loader( file ? file : varName, &storer ) );
}

#define LOADXML( var, ... ) copy(  var, #var, xmlLoad, ##__VA_ARGS__ )
#define SAVEXML( var, ... ) copy( #var,  var, xmlSave, ##__VA_ARGS__ )

#define LOADJSN( var, ... ) copy(  var, #var, jsnLoad, ##__VA_ARGS__ )
#define SAVEJSN( var, ... ) copy( #var,  var, jsnSave, ##__VA_ARGS__ )


#define KEEPXML( var ) ObjKeeper<typeof(var)>XMLKEEP##var( var, #var, xmlLoad )
#define KEEPJSN( var ) ObjKeeper<typeof(var)>XMLKEEP##var( var, #var, jsnLoad )
#define CAP_TYPEOF 1

#if __cplusplus < 201103L
  #ifndef __GNUG__
    #define KEEPXML( var, t ) ObjKeeper<t>XMLKEEP##var( var, #var, xmlLoad )
    #define KEEPJSN( var, t ) ObjKeeper<t>XMLKEEP##var( var, #var, jsnLoad )
    #undef CAP_TYPEOF 
  #endif
#endif


/* This trick allows to persist a bunch of global variables
 */

#define KEEPBUNCH( name, loader  ) \
struct TYPE##name{} name; \
ObjKeeper<TYPE##name>KEEP##name##loader( name, #name, loader ); \
void load( ObjConfRec & cnf, TYPE##name &  )

#define KEEPXMLLIST( name ) KEEPBUNCH( name, xmlLoad )
#define KEEPJSNLIST( name ) KEEPBUNCH( name, jsnLoad )

#define KEEP_LOADER( T ) void load( ObjConfRec & cnf, T & hld )
#define KEEP_STORED( obj ) ObjStorer< typeof( obj )  > storerOf##obj( obj, #obj )


/*    Hide implementation detais to user. Not very purist, but c++ does not
 *  allow this handy type of things
 */

  #define KEEPROOT( obj        ) cnf.root((obj&)(hld), #obj )
  #define KEEPITEM( obj,  ...  ) cnf.launch( hld.obj , #obj, ##__VA_ARGS__ )
  #define KEEPLIST( obj        ) cnf.launch( hld.obj , #obj, hld )
  #define KEEPGLOB( obj        ) cnf.launch(     obj , #obj )

#else

  #define KEEPITEM( obj  )
  #define KEEPROOT( obj  )
  #define KEEPLIST( obj  )

#endif
#endif

