#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>

namespace pack2
{
class cBin;
typedef std::shared_ptr< cBin > bin_t;

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
    virtual int sizX()
    {
        return myX;
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
    bool isSpun()
    {
        return myfSpun;
    }
    virtual void locate( int x, int y )
    {
        myLocX = x;
        myLocY = y;
    }
    int locX()
    {
        return myLocX;
    }
    int locY()
    {
        return myLocY;
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
    std::string text() const
    {
        std::stringstream ss;
        ss << myUserID <<" "<< myProgID <<" "<< myX <<" x " << myY;
        return ss.str();
    }
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
typedef std::shared_ptr< cItem > item_t;

class cBin : public cShape
{
public:
    cBin( const std::string& id, int x, int y )
        : cShape( id, x, y )
        , myfCopy( false )
        , myCopyCount( 0 )
        , myParent( NULL )
    {

    }
    cBin( bin_t old )
        : cShape( old->userID(), old->sizX(), old->sizY() )
        , myfCopy( true )
        , myCopyCount( old->myCopyCount+1)
        , myParent( NULL )
    {
        userID( userID() + "_cpy" + std::to_string( myCopyCount ) );
    }
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
    bool isSub()
    {
        return ( myParent != NULL );
    }
    void spinEnable( bool f = true )
    {
        throw std::runtime_error("Bins cannot be rotated");
    }

private:
    bool myfCopy;                    ///< true if endless supply available
    int myCopyCount;
    std::vector< item_t > myContent;
    bin_t myParent;
};


/// true if item fits inside bin
bool Fits( item_t item, bin_t bin );

class cPackEngine
{
public:
    void addBin( const std::string& id, int x, int y )
    {
        myBin.push_back( bin_t( new cBin(  id, x, y ) ) );
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
private:
    std::vector< item_t > myItem;
    std::vector< bin_t > myBin;
};



/// pack item into bin
void Add( cPackEngine& e, bin_t bin, item_t item );

/// Remove unused and sub bins
void RemoveUnusedBins( cPackEngine& e );


void SortItemsIntoDecreasingSize( cPackEngine& e );

void SortBinsIntoIncreasingSize( cPackEngine& e );


/// Pack items into bins
void Pack( cPackEngine& e );

/// Number of bins used
int BinCount( cPackEngine& e);

/// Packing result as a CSV string
std::string CSV( cPackEngine& e );

/// CSV item ids that could not be fittted
std::string Unpacked( cPackEngine& e );

std::string CutList( cPackEngine& e );

std::string DrawList( cPackEngine& e );
std::string DrawList( bin_t bin );
std::string DrawList( item_t item );
}


