#ifndef BUSBOY_LIST
#define BUSBOY_LIST

#include <cstddef>
#include <initializer_list>

using std::initializer_list;

namespace busboy {

    template <typename T> class List {

        public:

        struct ListNode {

            ListNode * prev;
            ListNode * next;
            T * data;

            ListNode ( const T & init ) {
                this->prev = nullptr;
                this->next = nullptr;
                this->data = new T( init );
            }

            ~ListNode () {
                delete this->data;
            }

        };

        class BaseIter {

            protected:

            ListNode * curr;

            public:

            BaseIter (
                ListNode * curr
            ) : curr( curr ) {}

            const T & operator* () const {
                if ( !this->curr )
                    throw "Invalid List::Iter dereference.";
                return *( this->curr->data );
            }

            const T * operator-> () const {
                if ( !this->curr )
                    throw "Invalid List::Iter dereference.";
                return this->curr->data;
            }

            operator bool () const {
                return this->curr;
            }

        };

        class Iter;

        class ConstIter : public BaseIter {

            public:

            ConstIter (
                ListNode * curr
            ) : BaseIter( curr ) {}

            ConstIter (
                const Iter & it
            ) : BaseIter( it.curr ) {}

            ConstIter & operator-- ( int ) {
                if ( this->curr )
                    this->curr = this->curr->prev;
                return *this;
            }

            ConstIter & operator++ ( int ) {
                if ( this->curr )
                    this->curr = this->curr->next;
                return *this;
            }

            ConstIter & operator= ( const ConstIter & other ) {
                this->curr = other.curr;
                return *this;
            }

            operator bool () const {
                return this->curr;
            }

        };

        class Iter : public ConstIter {

            protected:

            List<T> & lst;

            public:

            Iter (
                List<T> & lst,
                ListNode * curr
            ) : ConstIter( curr ), lst( lst ) {}

            Iter (
                List<T> & lst,
                const ConstIter & it
            ) : ConstIter( it ), lst( lst ) {}

            T & operator* () {
                if ( !this->curr )
                    throw "Invalid List::Iter dereference.";
                return *( this->curr->data );
            }

            T * operator-> () {
                if ( !this->curr )
                    throw "Invalid List::Iter dereference.";
                return this->curr->data;
            }

            Iter & operator-- ( int ) {
                ConstIter::operator--( 1 );
                return *this;
            }

            Iter & operator++ ( int ) {
                ConstIter::operator++( 1 );
                return *this;
            }

            Iter & operator= ( const Iter & other ) {
                this->lst = other.lst;
                this->curr = other.curr;
                return *this;
            }

            Iter & insertBefore ( const T & other ) {
                if ( !this->curr )
                    throw "insertBefore() on dead List::Iter.";
                ListNode * pre = new ListNode( other );
                if ( this->curr == this->lst.head )
                    this->lst.head = pre;
                if ( this->curr->prev ) {
                    this->curr->prev->next = pre;
                    pre->prev = this->curr->prev;
                }
                pre->next = this->curr;
                this->curr->prev = pre;
                this->lst.len++;
                return *this;
            }

            Iter & insertAfter ( const T & other ) {
                if ( !this->curr )
                    throw "insertAfter() on dead List::Iter.";
                ListNode * post = new ListNode( other );
                if ( this->curr == this->lst.tail )
                    this->lst.tail = post;
                if ( this->curr->next ) {
                    this->curr->next->prev = post;
                    post->next = this->curr->next;
                }
                post->prev = this->curr;
                this->curr->next = post;
                this->lst.len++;
                return *this;
            }

            Iter & insertAfter (
                const ConstIter & begin,
                const ConstIter & end
            ) {
                for ( auto it = begin; it; it++ ) {
                    this->insertAfter( *it )++;
                }
                return *this;
            }

            Iter & slice () {
                if ( !this->curr )
                    throw "slice() on dead List::Iter.";
                if (
                    this->curr == this->lst.head &&
                    this->curr == this->lst.tail
                ) {
                    this->lst.head = this->lst.tail = nullptr;
                } else if ( this->curr == this->lst.head ) {
                    this->lst.head = this->curr->next;
                } else if ( this->curr == this->lst.tail ) {
                    this->lst.tail = this->curr->prev;
                }
                if ( this->curr->prev && this->curr->next ) {
                    this->curr->prev->next = this->curr->next;
                    this->curr->next->prev = this->curr->prev;
                } else if ( this->curr->prev ) {
                    this->curr->prev->next = nullptr;
                } else if ( this->curr->next ) {
                    this->curr->next->prev = nullptr;
                }
                ListNode * toSlice = this->curr;
                this->curr = this->curr->next;
                delete toSlice;
                this->lst.len--;
                return *this;
            }

        };

        protected:

        ListNode * head;
        ListNode * tail;
        size_t len;

        public:

        List () {
            this->head = nullptr;
            this->tail = nullptr;
            this->len = 0;
        }

        List ( const List<T> & other ) : List () {
            *this = other;
        }

        List ( initializer_list<T> init ) : List () {
            const T * item = init.begin();
            while ( item != init.end() )
                this->push( *item++ );
        }

        ~List () {
            this->clear();
        }

        List<T> & operator= ( const List<T> & other ) {
            if ( &other == this )
                return *this;
            this->clear();
            for ( ConstIter it = other.begin(); it; it++ )
                this->push( *it );
            return *this;
        }

        bool operator== ( const List<T> & other ) const {
            if ( &other == this )
                return true;
            if ( other.len != this->len )
                return false;
            ConstIter me = this->begin();
            ConstIter you = other.begin();
            while ( me ) {
                if ( *me != *you )
                    return false;
                me++;
                you++;
            }
            return true;
        }

        bool operator!= ( const List<T> & other ) const {
            return !( *this == other );
        }

        List<T> & push ( const T & value ) {
            if ( this->len ) {
                this->end().insertAfter( value );
            } else {
                this->head = this->tail = new ListNode( value );
                this->len++;
            }
            return *this;
        }

        List<T> & unshift ( const T & value ) {
            if ( this->len ) {
                this->begin().insertBefore( value );
            } else {
                this->head = this->tail = new ListNode( value );
                this->len++;
            }
            return *this;
        }

        T & peekHead () {
            if ( !this->len )
                throw "peekHead() on empty List.";
            return *( this->head->data );
        }

        T & peekTail () {
            if ( !this->len )
                throw "peekTail() on empty List.";
            return *( this->tail->data );
        }

        const T & peekHead () const {
            if ( !this->len )
                throw "peekHead() on empty List.";
            return *( this->head->data );
        }

        const T & peekTail () const {
            if ( !this->len )
                throw "peekTail() on empty List.";
            return *( this->tail->data );
        }

        T pop () {
            if ( !this->len )
                throw "pop() on empty List.";
            T copy = this->peekTail();
            this->end().slice();
            return copy;
        }

        T shift () {
            if ( !this->len )
                throw "shift() on empty List.";
            T copy = this->peekHead();
            this->begin().slice();
            return copy;
        }

        List<T> & concat ( const List<T> & other ) {
            if ( this->isEmpty() ) {
                *this = other;
            } else {
                this->end().insertAfter( other.begin(), other.end() );
            }
            return *this;
        }

        List<T> & operator+= ( const List<T> & other ) {
            return this->concat( other );
        }

        size_t getLength () const {
            return this->len;
        }

        bool isEmpty () const {
            return this->len == 0;
        }

        List<T> & reverse () {
            // TODO: something more efficient
            List<T> newList;
            while ( !this->isEmpty() ) {
                newList.push( this->peekTail() );
                this->pop();
            }
            *this = newList;
            return *this;
        }

        List<T> & clear () {
            while ( this->len )
                this->pop();
            return *this;
        }

        List<T> & compact () {
            return *this;
        }

        Iter begin () {
            return Iter( *this, this->head );
        }

        Iter end () {
            return Iter( *this, this->tail );
        }

        ConstIter begin () const {
            return ConstIter( this->head );
        }

        ConstIter end () const {
            return ConstIter( this->tail );
        }

    };

};

#endif
