#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

namespace pack2
{
class cBin;
typedef std::shared_ptr< cBin > bin_t;
class cItem;
typedef std::shared_ptr< cItem > item_t;
typedef std::vector<item_t> itemv_t;
typedef std::vector<bin_t> binv_t;
}

#include "cCut.h"

namespace pack2
{
class cShape
{
public:
    cShape( const std::string& id, int x, int y )
        : myUserID( id )
        , myX( x )
        , myY( y )
        , myfSpun( false )
        , myfCanSpin( false )
        , myProgID( ++myLastProgID )
        , myLocX( 0 )
        , myLocY( 0 )
        , myfPacked( false )
    {

    }
    cShape( int left, int top, int width, int height)
        : cShape( "", width, height )
    {
        myLocX = left;
        myLocY = top;
    }
    cShape()
    : cShape("",0,0)
    {

    }
    virtual int sizX() const
    {
        return myX;
    }
    void sizX( int sx )
    {
        myX = sx;
    }
    void sizY( int sy )
    {
        myY = sy;
    }
    virtual int sizY()
    {
        return myY;
    }
    virtual std::string userID() const
    {
        return myUserID;
    }
    void userID( const std::string& id )
    {
        myUserID = id;
    }
    int progID() const
    {
        return myProgID;
    }
    virtual void spinEnable( bool f = true )
    {
        myfCanSpin = f;
    }
    bool canSpin() const
    {
        return myfCanSpin;
    }
    /// rotate 90 degrees around Z axis
    void spin()
    {
        if( ! myfCanSpin )
            return;
        if( myfSpun )
            return;
        int temp = myX;
        myX = myY;
        myY = temp;
        myfSpun = true;
    }
    void unspin()
    {
        if( ! myfCanSpin )
            return;
        if( ! myfSpun )
            return;
        int temp = myX;
        myX = myY;
        myY = temp;
        myfSpun = false;
    }
    bool isSpun()
    {
        return myfSpun;
    }
    virtual void locate( int x, int y )
    {
        myLocX = x;
        myLocY = y;
    }
    int locX() const
    {
        return myLocX;
    }
    void locX( int lx )
    {
        myLocX = lx;
    }
    int locY() const
    {
        return myLocY;
    }
    void locY( int ly )
    {
        myLocY = ly;
    }
    int right() const
    {
        return myLocX + myX;
    }
    int bottom() const
    {
        return myLocY + myY;
    }
    void pack( bool f = true )
    {
        myfPacked = f;
    }
    bool isPacked() const
    {
        return myfPacked;
    }
    int size() const
    {
        return myX * myY;
    }
    bool isOverlap( const cShape& other ) const
    {
        return ( myLocX <= other.right() && other.locX() <= right()
                && myLocY <= other.bottom() && other.locY() <= bottom() );
    }
    /** Subtract overlap with other shape
        @param[in] other
        Assumes other is to left and below this
    */
    virtual void subtract( const cShape& other )
    {
        if( ! isOverlap( other ))
        {
            return;
        }
        if( myLocX < other.myLocX && myLocY < other.myLocY ) {
            myX = other.myLocX - myLocX;
            myY    = other.myLocY - myLocY;
        }
        else if( myLocY < other.myLocY ) {
            myY    = other.myLocY - myLocY;
            return;
        }
        else if( myLocX < other.myLocX ) {
            myX = other.myLocX - myLocX;
            return;
        }
        else
            throw std::runtime_error( "cShape::overlap");
    }

    bool operator==( const cShape& other ) const;

    std::string text() const;

private:
    std::string myUserID;
    int myX;
    int myY;
    bool myfSpun;               // true if item was rotated when packed
    bool myfCanSpin;
    int myProgID;
    static int myLastProgID;
    int myLocX;
    int myLocY;
    bool myfPacked;

};

class cItem : public cShape
{
public:
    cItem( const std::string& id, int x, int y )
        : cShape( id, x, y )
    {

    }

private:

};

class cBin : public cShape
{
public:
    cBin( const std::string& id, int x, int y )
        : cShape( id, x, y )
        , myfCopy( false )
        , myCopyCount( 1 )
        , myParent( NULL )
    {

    }
    cBin( bin_t parent, int left, int top, int width, int height)
        : cShape( left, top, width, height)
        , myfCopy( false )
        , myCopyCount( 1 )
        , myParent( parent )
    {

    }
    /// copy constructor
    cBin( bin_t old );

    void add( item_t item )
    {
        if( isSub() )
        {
            myParent->add( item );
        }
        else
        {
            myContent.push_back( item );
        }
        pack();
        item->pack();
    }
    void copyEnable( bool f = true )
    {
        myfCopy = true;
    }
    bool canCopy() const
    {
        return myfCopy;
    }

    bool isUsed()
    {
        return (bool) myContent.size();
    }
    std::vector< item_t >& contents()
    {
        return myContent;
    }
    void parent( bin_t p )
    {
        myParent = p;
    }
    bin_t parent()
    {
        return myParent;
    }
    /** true if this bin has a parent
        which means that is not one of the original or copied physical bins
        but a sub-bin created to hold a packed item or unused space left when an item was packed
    */
    bool isSub()
    {
        return ( myParent != NULL );
    }
    /// Program ID of parent, or self if no parent
    int origID()
    {
        if( ! isSub() )
            return progID();
        return myParent->progID();
    }
    void spinEnable( bool f = true )
    {
        throw std::runtime_error("Bins cannot be rotated");
    }
    /// get copy count of root bin
    int copyCount()
    {
        int ret;
        if( myParent )
            ret = myParent->copyCount();
        else
            ret = myCopyCount;
        //std::cout << "copycount " << progID() <<" " << ret << "\n";
        return ret;
    }
    std::string text();


private:
    bool myfCopy;                    ///< true if endless supply available
    int myCopyCount;
    std::vector< item_t > myContent;
    bin_t myParent;
};

/// Algorithm configuration
struct sAlgorithm
{
    bool fTryEveryItemFirst;
    bool fMultipleFit;
    bool fThruCuts;
    int MergeOnRightCandMinWidth;
};

class cPackEngine
{
public:
    cPackEngine()
    {
        myAlgorithm.fTryEveryItemFirst = false;
        myAlgorithm.fMultipleFit = false;
        myAlgorithm.fThruCuts = false;
        myAlgorithm.MergeOnRightCandMinWidth = 0;
    }
    bin_t addBin( const std::string& id, int x, int y )
    {
        bin_t b = bin_t( new cBin(  id, x, y ) );
        myBin.push_back( b );
        return b;
    }
    void addItem( const std::string& id, int x, int y)
    {
        myItem.push_back( item_t( new cItem( id, x, y )) );
    }
    void add( bin_t bin )
    {
        myBin.push_back( bin );
    }
    void add( item_t item )
    {
        myItem.push_back( item );
    }
    void clear()
    {
        myItem.clear();
        myBin.clear();
    }
    std::vector< bin_t > & bins()
    {
        return myBin;
    }
    std::vector< item_t > & items()
    {
        return myItem;
    }
    sAlgorithm& Algorithm()
    {
        return myAlgorithm;
    }
private:
    std::vector< item_t > myItem;
    std::vector< bin_t > myBin;
    sAlgorithm myAlgorithm;
};

/// true if item fits inside bin
bool Fits( item_t item, bin_t bin );
bool FitsInMultipleSpaces( cPackEngine& e, item_t item, bin_t bin );

/// pack item into bin
void Add( cPackEngine& e, bin_t bin, item_t item );
void AddAtBottomRight( cPackEngine& e, bin_t parent, item_t item );
bool CheckForOverlap( cPackEngine& e, bin_t space );
void CreateRemainingSpaces( cPackEngine& e, bin_t bin, item_t item );

void MergeUnusedSpace( cPackEngine& e, bin_t mewbin );
void MergeUnusedOnRight( cPackEngine& e );
void Merge( cPackEngine& e, bin_t above, bin_t below );
void MergeUnusedFromBottomRight( cPackEngine& e,  bin_t bin );
void MergePairs( cPackEngine& e,  bin_t bin );
bool MergePair( cPackEngine& e, bin_t space1, bin_t space2 );
void MergeTriple( cPackEngine& e,  bin_t bin );

/// Remove unused and sub bins
void RemoveUnusedBins( cPackEngine& e );
void RemoveZeroBins( cPackEngine& e );

void SortItemsIntoDecreasingSize( cPackEngine& e );
void SortItemsIntoDecreasingAwkward( cPackEngine& e );
void SortItemsDecreasingSquaredDim( cPackEngine& e );

/// sort bins into increasing size AND copy count
void SortBinsIntoIncreasingSize( cPackEngine& e );

/// Vector of spaces in bin
binv_t Spaces( cPackEngine& e, bin_t bin );

/// Sort and pack items into bins,
void Pack( cPackEngine& e );

/// Pack pre-sorted items into bins
void PackSortedItems( cPackEngine& e );

/// Number of bins used
int BinCount( cPackEngine& e);

/// Packing result as a CSV string
std::string CSV( cPackEngine& e );

/// CSV item ids that could not be fittted
std::string Unpacked( cPackEngine& e );

std::string DrawList( cPackEngine& e );
std::string DrawList( bin_t bin );
std::string DrawList( item_t item );

/// Human readable cut list
std::string CutListString( cPackEngine& e );

/// Vector of cut endpoints
std::vector< std::vector<int> > CutListEndPoints( cPackEngine& e );

/// Vector of cuts
std::vector< cCutList > CutList( cPackEngine& e );
}


