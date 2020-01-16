#include <iostream>
#include <sstream>
#include "pack2.h"
namespace pack2
{
bool cShape::operator==( const cShape& other ) const
{
    if( myLocX != other.myLocX )
        return false;
    if( myLocY != other.myLocY )
        return false;
    if( myX != other.myX )
        return false;
    if( myLocY != other.myLocY )
        return false;
    return true;
}
std::string cShape::text() const
{
    std::stringstream ss;
    ss << myUserID <<" "<< myProgID <<" "<< myX <<" x " << myY
       << " at " << myLocX << ", "<< myLocY;
    if( isPacked() )
        ss << " packed";
    ss << "\n";
    return ss.str();
}
}
