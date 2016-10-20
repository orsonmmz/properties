#include "inspected.h"
#include <iostream>
#include <wx/gdicmn.h>      // wxPoint

using namespace std;

// TODO
// handle different types of setters/getters (const, non-const, reference, pointer)
// handle mutliinheritance
// traverse the class tree structure when looking for properties
// possiblities:
//  static map<typeindex, map<wxString, PROPERTY_BASE*> props; : does not include relationship between the classes
//  force declaring the base type, return the basetype using typeindex : does not allow to traverse the class tree
//  add a structure describing the class tree
//  enforce a static map in every class (static_assert?)
//  enums

class A : public INSPECTED
{
public:
    A()
    {
        cout << "constructing " << typeid(*this).name() << std::endl;
    }

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

class C
{
public:
    bool getBool() const { cout << "C::getBool " << endl; return m_bool; }
    void setBool( bool a ) { cout << "C::setBool " << endl; m_bool = a; }

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
static PROPERTY<D, D::enum_test> prop5( "enum", &D::setEnum, &D::getEnum );
static INHERITS_AFTER_BASE a( TYPE_HASH( B ), TYPE_HASH( A ) );
static INHERITS_AFTER_BASE b( TYPE_HASH( D ), TYPE_HASH( A ) );
static INHERITS_AFTER_BASE c( TYPE_HASH( D ), TYPE_HASH( C ) );

main()
{
    A a;
    B b;
    D d;

    //a.Set( "A", 42 );
    //b.Set( "A", 100 );
    //b.Set( "point", wxPoint( 100, 200 ) );
    //d.Set( "A", 23 );
    d.Set( "bool", true );

    //auto prop_a = a.Get<int>( "A" );
    //auto prop_point = b.Get<wxPoint>( "point" );
    //auto prop_bool = d.Get<bool>( "bool" );
    auto prop_aa = d.Get<int>( "A" );

    //if( prop_a )
        //cout << *prop_a << endl;

    //if( prop_point )
        //cout << prop_point->x << " " << prop_point->y << endl;
}

