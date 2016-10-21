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

static class CLASS_A_DESC
{
public:
    CLASS_A_DESC()
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        propMgr.AddProperty( new PROPERTY<A, int>( "A", &A::setA, &A::getA ) );
        propMgr.AddProperty( new PROPERTY<A, const wxPoint&>( "point", &A::setPoint, &A::getPoint ) );
    }
} _CLASS_A_DESC;

static class CLASS_B_DESC
{
public:
    CLASS_B_DESC()
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        propMgr.AddProperty( new PROPERTY<B, int>( "C", &B::setC, &B::getC ) );
        propMgr.InheritsAfter( TYPE_HASH( B ), TYPE_HASH( A ) );
    }
} _CLASS_B_DESC;

static class CLASS_C_DESC
{
public:
    CLASS_C_DESC()
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        propMgr.AddProperty( new PROPERTY<C, bool>( "bool", &C::setBool, &C::getBool ) );
        propMgr.AddProperty( new PROPERTY<C, int>( "new", &C::setNew, &C::getNew ) );
        propMgr.InheritsAfter( TYPE_HASH( C ), TYPE_HASH( A ) );
    }
} _CLASS_C_DESC;

static class CLASS_D_DESC
{
public:
    CLASS_D_DESC()
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        propMgr.AddProperty( new PROPERTY<D, D::enum_test>( "enum", &D::setEnum, &D::getEnum ) );
        propMgr.AddTypeCast( new TYPE_CAST<D, A> );
        propMgr.AddTypeCast( new TYPE_CAST<D, C> );
        propMgr.InheritsAfter( TYPE_HASH( D ), TYPE_HASH( A ) );
        propMgr.InheritsAfter( TYPE_HASH( D ), TYPE_HASH( C ) );
    }
} _CLASS_D_DESC;


int main()
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

