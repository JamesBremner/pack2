
#include <cmath>
#include "pack2.h"


#define SMALL_DIFF 0.01

using namespace std;

namespace pack2
{

cCut::cCut( double x0, double y0, double x1, double y1 )
{
    if( fabs( x0 - x1 ) < 0.01 )
    {
        myIsVertical = true;
        myIntercept = x0;
        if( y0 < y1 )
        {
            myStart = y0;
            myStop  = y1;
        }
        else
        {
            myStart = y1;
            myStop  = y0;
        }
    }
    else
    {
        myIsVertical = false;
        myIntercept = y0;
        if( x0 < x1 )
        {
            myStart = x0;
            myStop  = x1;
        }
        else
        {
            myStart = x1;
            myStop  = x0;
        }
    }
}
bool cCut::operator==( const cCut& other ) const
{
    if( myIsVertical != other.myIsVertical )
        return false;
    if( fabs ( myIntercept - other.myIntercept ) > 0.01 )
        return false;
    if( fabs ( myStart - other.myStart ) > 0.01 )
        return false;
    if( fabs ( myStop - other.myStop ) > 0.01 )
        return false;
    return true;
}

bool cCut::CanJoin( cCut& joined, const cCut& cut1, const cCut& cut2 )
{
    if( cut1 == cut2 )
    {
        joined = cut1;
        return true;
    }
    if( cut1.myIsVertical != cut2.myIsVertical )
    {
        // horizontal and vertical lines can never be joined
        return false;
    }
    if( fabs ( cut1.myIntercept - cut2.myIntercept ) > SMALL_DIFF )
        return false;
    if( fabs( cut1.myStop - cut2.myStart ) < SMALL_DIFF )
    {
        // cut2 continues cut1
        joined = cut1;
        joined.myStop = cut2.myStop;
        return true;
    }
    if( fabs( cut2.myStop - cut1.myStart ) < SMALL_DIFF )
    {
        // cut1 continues cut2
        joined = cut2;
        joined.myStop = cut1.myStop;
        return true;
    }
    if( cut1.myStart >= cut2.myStart && cut1.myStop <= cut2.myStop )
    {
        // cut1 is entirely inside cut2
        joined = cut2;
        return true;
    }
    if(  cut2.myStart >= cut1.myStart && cut2.myStop <= cut1.myStop)
    {
        // cut2 is entirely inside cut1
        joined = cut1;
        return true;
    }
    if( cut2.myStart < cut1.myStop )
    {
        joined = cut1;
        joined.myStop = cut2.myStop;
        return true;
    }
    if( cut1.myStart < cut2.myStop )
    {
        joined = cut2;
        joined.myStart = cut1.myStart;
        return true;
    }
    return false;
}



void cCutList::add( const cCut& cut )
{
    //cout << "Add " << cut.get() << "\n";

    // check if cut is along bin edge, so not needed
    if( cut.myIntercept < SMALL_DIFF )
        return;

    // check if cut already exists
    // this happens when two items with the same dimension
    // are aligned perfectly side by side
    if( std::find( myCut.begin(), myCut.end(), cut ) != myCut.end() )
        return;

    // add the cut
    myCut.push_back( cut );
}
void cCutList::join()
{
    bool found = true;
    while( found )
    {
        found = false;
        cCut cut;
        for( vector < cCut >::iterator it1 = myCut.begin();
                it1 != myCut.end(); it1++ )
        {
            for( vector < cCut >::iterator it2 = it1+1;
                    it2 != myCut.end(); it2++ )
            {
//                if( ( ! it1->myIsVertical ) && ( ! it2-> myIsVertical )) {
//                cout << "test join " << it1->sget() << " and "
//                                        << it2->sget() << "\n";
//                }
                if( cCut::CanJoin( cut, *it1, *it2 ) )
                {

//                    cout << "joining " << it1->sget() << " and "
//                        << it2->sget() << " making "
//                        << cut.sget() << endl;

                    /*  Erase the joined lines

                    Erase second line first
                    So as not to invalidate first iterator

                    */
                    myCut.erase( it2 );
                    myCut.erase( it1 );

                    // add joined line
                    myCut.push_back( cut );

                    found = true;
                    break;
                }
            }
            if( found )
                break;
        }
    }
}
string cCut::sget() const
{
    stringstream ss;
    if( myIsVertical )
    {
        ss << myIntercept << "," << myStart << " to ";
        ss << myIntercept << "," << myStop;
    }
    else
    {
        ss << myStart << "," << myIntercept << " to ";
        ss << myStop << "," << myIntercept;
    }
    return ss.str();
}
std::vector<int> cCut::get() const
{
    std::vector<int> ret;
    if( myIsVertical )
    {
        ret.push_back( myIntercept);
        ret.push_back( myStart);
        ret.push_back( myIntercept);
        ret.push_back( myStop);
    }
    else
    {
        ret.push_back( myStart);
        ret.push_back( myIntercept);
        ret.push_back( myStop);
        ret.push_back( myIntercept);
    }
    return ret;
}
string cCutList::sget() const
{
    stringstream ss;
    for( auto& c : myCut )
    {
        ss << myBin->userID() << ","<< c.sget() << endl;
    }
    return ss.str();
}
std::vector< std::vector<int> > cCutList::get() const
{
    std::vector< std::vector<int> > ret;
    for( auto& c : myCut )
    {
        std::vector<int> row;
        row.push_back( myBin->progID() );
        for( int v : c.get() )
            row.push_back( v );
        ret.push_back( row );
    }
    return ret;
}

void CutListBin( bin_t bin, cCutList& L )
{
    L.set( bin );
    for( item_t item : bin->contents() )
    {
        L.add( cCut(
                   item->locX(),
                   item->locY(),
                   item->locX(),
                   item->locY() + item->sizY()
               ));
        L.add( cCut(
                   item->locX(),
                   item->locY() + item->sizY(),
                   item->locX() + item->sizX(),
                   item->locY() + item->sizY()
               ));
        L.add( cCut(
                   item->locX() + item->sizX(),
                   item->locY() + item->sizY(),
                   item->locX() + item->sizX(),
                   item->locY()
               ));
        L.add( cCut(
                   item->locX() + item->sizX(),
                   item->locY(),
                   item->locX(),
                   item->locY()
               ));
    }
}
}


