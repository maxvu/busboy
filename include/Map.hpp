#ifndef BUSBOY_MAP
#define BUSBOY_MAP

#define BUSBOY_MAP_DEFAULTINITCAP 16    // default capacity when not specified
#define BUSBOY_MAP_MINCAP         8     // minimum accept capacity
#define BUSBOY_MAP_MAXLOAD        0.85  // maximum occupancy before resize()
#define BUSBOY_MAP_GROWTHFACTOR   1.85  // scaling factor when auto-resize()'ing

#include <cstddef>
#include <cstring>

#include "String.hpp"

namespace busboy {

    template <typename T> class Map {

        public:

        static size_t djb2 ( const String & s ) {
            //http://www.cse.yorku.ca/~oz/hash.html
            size_t hash = 5381;
            for ( size_t i = 0; i < s.getLength(); i++ )
                hash = ( hash << 5 ) + hash + s[ i ];
            return hash;
        }

        static size_t sdbm ( const String & s ) {
            //http://www.cse.yorku.ca/~oz/hash.html
            size_t hash = 0;
            for ( size_t i = 0; i < s.getLength(); i++ )
                hash = s[ i ] + ( hash << 6 ) + ( hash << 16 ) - hash;
            return hash;
        }

        protected:

        struct MapSlot {

            String key;
            T * val;

            MapSlot ( const String & key, const T & val ) {
                this->key = key;
                this->val = new T( val );
            }

            MapSlot ( const MapSlot & other ) {
                this->key = other.key;
                this->val = new T( *( other.val ) );
            }

            ~MapSlot () {
                delete this->val;
            }

            MapSlot & operator= ( const MapSlot & other ) {
                this->key = other.key;
                *( this->val ) = other.val;
                return *this;
            }

        };

        struct MapSlotBank {

            MapSlot ** slots;
            size_t cap;
            size_t occ;

            inline size_t idxLo ( size_t hash ) {
                return hash % ( this->cap / 2 );
            }

            inline size_t idxHi ( size_t hash ) {
                return ( hash % ( this->cap / 2 ) ) + ( this->cap / 2 );
            }

            MapSlotBank ( size_t cap ) {
                this->slots = new MapSlot * [ cap ];
                this->cap = cap;
                this->occ = 0;
                for ( size_t i = 0; i < cap; i++ )
                    this->slots[ i ] = nullptr;
            }

            ~MapSlotBank () {
                this->zero();
                delete[] this->slots;
            }

            MapSlotBank * zero () {
                for ( size_t i = 0; i < this->cap; i++ ) {
                    if ( this->slots[ i ] ) {
                        delete this->slots[ i ];
                        this->slots[ i ] = nullptr;
                        this->occ--;
                    }
                }
                return this;
            }

            MapSlot * peekLo ( size_t hash ) {
                return this->slots[ idxLo( hash ) ];
            }

            MapSlot * peekHi ( size_t hash ) {
                return this->slots[ idxHi( hash ) ];
            }

            MapSlot * popLo ( size_t hash ) {
                if ( MapSlot * popped = this->slots[ idxLo( hash ) ] ) {
                    this->slots[ idxLo( hash ) ] = nullptr;
                    this->occ--;
                    return popped;
                }
                return nullptr;
            }

            MapSlot * popHi ( size_t hash ) {
                if ( MapSlot * popped = this->slots[ idxHi( hash ) ] ) {
                    this->slots[ idxHi( hash ) ] = nullptr;
                    this->occ--;
                    return popped;
                }
                return nullptr;
            }

            MapSlotBank * setLo ( size_t hash, MapSlot * slot ) {
                if ( MapSlot * existing = this->peekLo( hash ) ) {
                    delete this->slots[ this->idxLo( hash ) ];
                    this->slots[ this->idxLo( hash ) ] = slot;
                } else {
                    this->slots[ this->idxLo( hash ) ] = slot;
                    this->occ++;
                }
                return this;
            }

            MapSlotBank * setHi ( size_t hash, MapSlot * slot ) {
                if ( MapSlot * existing = this->peekHi( hash ) ) {
                    delete this->slots[ this->idxHi( hash ) ];
                    this->slots[ this->idxHi( hash ) ] = slot;
                } else {
                    this->slots[ this->idxHi( hash ) ] = slot;
                    this->occ++;
                }
                return this;
            }

            double getLoad () const {
                return ( double ) this->occ / ( double ) this->cap;
            }

        };

        public:

        class ConstIter {

            protected:

            const MapSlotBank * bank;
            size_t curr;

            public:

            ConstIter (
                const MapSlotBank * bank,
                size_t curr
            ) : bank( bank ), curr( curr ) {
                while ( *this && !this->bank->slots[ this->curr ] )
                    this->curr++;
            }

            const T & operator* () const {
                return *( this->operator->() );
            }

            const T * operator-> () const {
                if ( !*this )
                    throw "Access on dead Map::ConstIter.";
                return this->bank->slots[ this->curr ]->val;
            }

            const String & getKey () const {
                if ( !*this )
                    throw "Key access on dead Map::ConstIter.";
                return this->bank->slots[ this->curr ]->key;
            }

            const T & getVal () const {
                return this->operator*();
            }

            operator bool () const {
                return this->curr < this->bank->cap;
            }

            ConstIter & operator++ ( int ) {
                if ( !*this )
                    return *this;
                this->curr++;
                while (
                    this->curr < this->bank->cap &&
                    !this->bank->slots[ this->curr ]
                ) {
                    this->curr++;
                }
                return *this;
            }

        };

        class Iter : public ConstIter {

            public:

            Iter (
                const MapSlotBank * bank,
                size_t curr
            ) : ConstIter( bank, curr ) {

            }

            T & getVal () {
                return const_cast<T&>( this->operator*() ); // Is this awful?
            }

            Iter & slice () {
                if ( !*this )
                    throw "slice() on dead Map::Iter.";
                MapSlotBank * bank = const_cast<MapSlotBank*>( this->bank );
                delete bank->slots[ this->curr ];
                bank->slots[ this->curr ] = nullptr;
                bank->occ--;
                return *this;
            }

        };

        protected:

        MapSlotBank * bank;

        Map & resize ( size_t newCapacity ) {
            MapSlotBank * oldBank = this->bank;
            this->bank = new MapSlotBank( newCapacity );
            for ( size_t i = 0; i < oldBank->cap; i++ ) {
                if ( MapSlot * existing = oldBank->slots[ i ] )
                    this->set( existing->key, *( existing->val ) );
            }
            delete oldBank;
            return *this;
        }

        public:

        Map () : Map( BUSBOY_MAP_DEFAULTINITCAP ) {

        }

        Map ( size_t initialCapacity ) {
            if ( initialCapacity < BUSBOY_MAP_MINCAP )
                initialCapacity = BUSBOY_MAP_MINCAP;
            this->bank = new MapSlotBank( initialCapacity );
        }

        Map (
            const Map & other
        ) : Map( other.bank->occ * BUSBOY_MAP_GROWTHFACTOR ) {
            for ( ConstIter it = other.begin(); it; it++ )
                this->set( it.getKey(), it.getVal() );
        }

        ~Map () {
            delete this->bank;
        }

        Map & operator= ( const Map<T> & other ) {
            if ( &other != this ) {
                this->clear().reserve( other.bank->occ );
                for ( ConstIter it = other.begin(); it; it++ )
                    this->set( it.getKey(), it.getVal() );
            }
            return *this;
        }

        bool operator== ( const Map<T> & other ) const {
            if ( &other == this )
                return true;
            if ( this->bank->occ != other.bank->occ )
                return false;
            for ( ConstIter it = this->begin(); it; it++ ) {
                if ( !other.has( it.getKey() ) )
                    return false;
                if ( other.peek( it.getKey() ) != this->peek( it.getKey() ) )
                    return false;
            }
            return true;
        }

        bool operator!= ( const Map<T> & other ) const {
            return !( *this == other );
        }

        size_t getLength () const {
            return this->bank->occ;
        }

        size_t getCapacity () const {
            return this->bank->cap;
        }

        bool has ( const String & key ) const {
            MapSlot * lo = this->bank->peekLo( djb2( key ) );
            MapSlot * hi = this->bank->peekHi( sdbm( key ) );
            if ( lo && lo->key == key )
                return true;
            if ( hi && hi->key == key )
                return true;
            return false;
        }

        bool has ( const String * key ) const {
            return this->has( *key );
        }

        const T & peek ( const String & key ) const {
            return ( *this )[ key ];
        }

        T & peek ( const String & key ) {
            return ( *this )[ key ];
        }

        const T & operator[] ( const String & key ) const {
            MapSlot * lo = this->bank->peekLo( djb2( key ) );
            MapSlot * hi = this->bank->peekHi( sdbm( key ) );
            if ( lo && lo->key == key )
                return *( lo->val );
            if ( hi && hi->key == key )
                return *( hi->val );
            throw "peek() on non-existent Map key.";
        }

        T & operator[] ( const String & key ) {
            MapSlot * lo = this->bank->peekLo( djb2( key ) );
            MapSlot * hi = this->bank->peekHi( sdbm( key ) );
            if ( lo && lo->key == key )
                return *( lo->val );
            if ( hi && hi->key == key )
                return *( hi->val );
            throw "peek() on non-existent Map key.";
        }

        Map<T> & set ( const String & key, const T & val ) {
            if ( this->has( key ) ) {
                this->peek( key ) = val;
                return *this;
            }
            if ( this->bank->getLoad() > BUSBOY_MAP_MAXLOAD )
                this->resize( this->bank->cap * BUSBOY_MAP_GROWTHFACTOR );
            size_t h1 = djb2( key );
            size_t h2 = sdbm( key );
            if ( !this->bank->peekLo( h1 ) ) {
                this->bank->setLo( h1, new MapSlot( key, val ) );
                return *this;
            } else if ( !this->bank->peekHi( h2 ) ) {
                this->bank->setHi( h2, new MapSlot( key, val ) );
                return *this;
            }
            MapSlot * initial = new MapSlot( key, val );
            MapSlot * swap = this->bank->popLo( h1 );
            this->bank->setLo( h1, initial );
            MapSlot * juggle = swap;
            try {
                size_t jh1;
                size_t jh2;
                while (
                    ( jh1 = djb2( juggle->key ) ) &&
                    ( jh2 = sdbm( juggle->key ) ) &&
                    juggle != initial &&
                    this->bank->peekLo( jh1 ) &&
                    this->bank->peekHi( jh2 )
                ) {
                    if ( this->bank->peekLo( jh1 )->key == juggle->key ) {
                        swap = this->bank->popHi( jh2 );
                        this->bank->setHi( jh2, juggle );
                        juggle = swap;
                    } else {
                        swap = this->bank->popLo( jh1 );
                        this->bank->setLo( jh1, juggle );
                        juggle = swap;
                    }
                }
                if ( !this->bank->peekLo( jh1 ) ) {
                    this->bank->setLo( jh1, juggle );
                } else if ( !this->bank->peekHi( jh2 ) ) {
                    this->bank->setHi( jh2, juggle );
                } else {
                    delete juggle;
                    this->resize( this->bank->cap * BUSBOY_MAP_GROWTHFACTOR );
                    this->set( key, val );
                }
            } catch ( std::exception & ex ) {
                delete initial;
                throw ex;
            }
            return *this;
        }

        Map<T> & set ( const String * key, const T & val ) {
            return this->set( *key, val );
        }

        Map<T> & unset ( const String & key ) {
            size_t h1 = djb2( key );
            size_t h2 = sdbm( key );
            MapSlot * lo = this->bank->peekLo( h1 );
            MapSlot * hi = this->bank->peekHi( h2 );
            if ( lo && lo->key == key )
                delete this->bank->popLo( h1 );
            if ( hi && hi->key == key )
                delete this->bank->popHi( h2 );
            return *this;
        }

        Map<T> & unset ( const String * key ) {
            return this->unset( *key );
        }

        Map<T> & clear () {
            for ( Iter it = this->begin(); it; it++ )
                it.slice();
            return *this;
        }

        Map<T> & reserve ( size_t desiredCapacity ) {
            if ( desiredCapacity < BUSBOY_MAP_MINCAP )
                desiredCapacity = BUSBOY_MAP_MINCAP;
            if ( desiredCapacity <= this->bank->cap )
                return *this;
            return this->resize( desiredCapacity * BUSBOY_MAP_GROWTHFACTOR );
        }

        Map<T> & compact () {
            if ( this->bank->occ >= BUSBOY_MAP_MINCAP )
                this->resize( this->bank->occ );
            return *this;
        }

        Iter begin () {
            return Iter( this->bank, 0 );
        }

        ConstIter begin () const {
            return ConstIter( this->bank, 0 );
        }

    };

};

#endif
