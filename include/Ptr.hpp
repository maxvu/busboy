#ifndef BUSBOY_PTR
#define BUSBOY_PTR

namespace busboy {

    namespace {

        template <typename T> struct PtrSlot {

            T data;
            size_t count;

            PtrSlot () : count( 0 ) {

            }

            PtrSlot ( const T & init ) : data( init ), count( 0 ) {

            }

        };

    };

    template <typename T> class Ptr {

        protected:

        PtrSlot<T> * slot;

        public:

        Ptr () : slot( new PtrSlot<T> ) {
            this->slot->count++;
        }

        Ptr ( const T & init ) : slot( new PtrSlot<T>( init ) ) {
            this->slot->count++;
        }

        Ptr ( const Ptr<T> & other ) : slot( other.slot ) {
            this->slot->count++;
        }

        ~Ptr () {
            if ( !--this->slot->count )
                delete this->slot;
        }

        Ptr & operator= ( const Ptr<T> & other ) {
            if ( !--this->slot->count )
                delete this->slot;
            this->slot = other.slot;
            this->slot->count++;
        }

        bool operator== ( const Ptr<T> & other ) const {
            return this->slot == other.slot;
        }

        bool operator!= ( const Ptr<T> & other ) const {
            return !( *this == other );
        }

        const T & operator* () const {
            return this->slot->data;
        }

        const T * operator-> () const {
            return this->slot->data;
        }

        T & operator* () {
            this->makeUnique();
            return this->slot->data;
        }

        T * operator-> () {
            this->makeUnique();
            return this->slot->data;
        }

        bool isUnique () const {
            return this->slot->count == 1;
        }

        Ptr<T> & makeUnique () {
            if ( this->isUnique() )
                return *this;
            this->slot->count--;
            this->slot = new PtrSlot<T>( this->slot->data );
            this->slot->count++;
            return *this;
        }

    };

};

#endif
