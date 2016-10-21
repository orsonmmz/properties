#include "inspected.h"
#include <iostream>
#include <wx/gdicmn.h>      // wxPoint

using namespace std;

// TODO
// handle different types of setters/getters (const, non-const, reference, pointer)
//  enums

class A : public INSPECTED
{
public:
    virtual void setA( int a ) { cout << "A::setA" << endl; m_a = a; }
    virtual int getA() const { cout << "A::getA" << endl; return m_a; }

    void setPoint( const wxPoint& p ) { cout << "A::setPoint" << endl; m_p = p; }
    const wxPoint& getPoint() const { cout << "A::getPoint" << endl;return m_p; }

protected:
    int m_a;
    wxPoint m_p;
};

class B : public A
{
public:
    void setA( int a ) { cout << "B::setA" << endl; m_a = a; }
    int getA() { cout << "B::getA" << endl; return m_a; }

    void setC( int a ) { cout << "B::setC" << endl; }
    int getC() const { cout << "B::getC" << endl; }
};

class C : public INSPECTED
{
public:
    bool getBool() const { cout << "C::getBool " << endl; return m_bool; }
    void setBool( bool a ) { cout << "C::setBool " << endl; m_bool = a; }

    int getNew() const { cout << "C::getNew " << endl; return m_m; }
    void setNew( int m ) { cout << "C::setNew " << endl; m_m = m;}

    int m_m;
    bool m_bool;
};

class D : public A, public C
{
public:
    enum enum_test { TEST1, TEST2, TEST3 };

    virtual void setA( int a ) { cout << "D::setA" << endl; m_aa = a; }
    virtual int getA() const { cout << "D::getA" << endl; return m_aa; }

    enum_test getEnum() const { cout << "D::getEnum" << endl; }
    void setEnum( enum_test val ) { cout << "D::setEnum" << endl;}

int m_aa;
};

static PROPERTY<A, int> prop1( "A", &A::setA, &A::getA );
static PROPERTY<A, const wxPoint&> prop2( "point", &A::setPoint, &A::getPoint );
static PROPERTY<B, int> prop3( "C", &B::setC, &B::getC );
static PROPERTY<C, bool> prop4( "bool", &C::setBool, &C::getBool );
static PROPERTY<C, int> prop6( "new", &C::setNew, &C::getNew );
static PROPERTY<D, D::enum_test> prop5( "enum", &D::setEnum, &D::getEnum );
static INHERITS_AFTER_BASE a( TYPE_HASH( B ), TYPE_HASH( A ) );
static INHERITS_AFTER_BASE b( TYPE_HASH( D ), TYPE_HASH( A ) );
static INHERITS_AFTER_BASE c( TYPE_HASH( D ), TYPE_HASH( C ) );
static TYPE_CAST<D, C> DtoC;
static TYPE_CAST<D, A> DtoA;

main()
{
    A a;
    B b;
    D d;

    cout << "&d = " << &d << endl;
    cout << "(A*) &d = " << static_cast<A*>( &d ) << endl;
    cout << "(C*) &d = " << static_cast<C*>( &d ) << endl;

    A* ptr = &a;
    ptr->Set( "A", 42 );

    ptr = &b;
    ptr->Set( "A", 100 );
    ptr->Set( "point", wxPoint( 100, 200 ) );

    ptr = &d;
    ptr->Set( "A", 23 );
    ptr->Set( "bool", true );
    ptr->Set( "new", 128 );

    ptr = &a;
    auto prop_a = ptr->Get<int>( "A" );

    ptr = &b;
    auto prop_point = ptr->Get<wxPoint>( "point" );

    ptr = &d;
    auto prop_bool = ptr->Get<bool>( "bool" );
    auto prop_aa = ptr->Get<int>( "A" );
    auto prop_new = ptr->Get<int>( "new" );

    cout << *prop_a << endl;
    cout << prop_point->x << " " << prop_point->y << endl;
    cout << *prop_bool << " " << *prop_aa << " " << *prop_new << endl;
}

