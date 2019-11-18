#define INSTRUMENT 1

namespace pack2
{
class cBin;
typedef shared_ptr< cBin > bin_t;

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
    virtual string userID() const
    {
        return myUserID;
    }
    void userID( const string& id )
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
    string text() const
    {
        stringstream ss;
        ss << myUserID <<" "<< myProgID <<" "<< myX <<" x " << myY;
        return ss.str();
    }
private:
    string myUserID;
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

int cShape::myLastProgID = -1;

class cItem : public cShape
{
public:
    cItem( const std::string& id, int x, int y )
        : cShape( id, x, y )
    {

    }

private:

};
typedef shared_ptr< cItem > item_t;

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
        userID( userID() + "_cpy" + to_string( myCopyCount ) );
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
    vector< item_t >& contents()
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
        throw runtime_error("Bins cannot be rotated");
    }

private:
    bool myfCopy;                    ///< true if endless supply available
    int myCopyCount;
    vector< item_t > myContent;
    bin_t myParent;
};


/// true if item fits inside bin
bool Fits( item_t item, bin_t bin )
{
#ifdef INSTRUMENT
    cout << "Trying to fit item " << item->progID()
         << " into bin " <<bin->userID() <<" " << bin->progID()
         <<" " << bin->sizX() <<" "<< bin->sizY()<< "\n";
#endif // INSTRUMENT

    if ( item->sizX() <= bin->sizX() && item->sizY() <= bin->sizY() )
        return true;
    if( item->canSpin() )
        item->spin();
    return ( item->sizX() <= bin->sizX() && item->sizY() <= bin->sizY() );
}


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
    vector< bin_t > & bins()
    {
        return myBin;
    }
    vector< item_t > & items()
    {
        return myItem;
    }
private:
    vector< item_t > myItem;
    vector< bin_t > myBin;
};



/// pack item into bin
void Add( cPackEngine& e, bin_t bin, item_t item )
{
#ifdef INSTRUMENT
    cout << "Adding item " << item->progID() << " to bin " << bin->progID() << "\n";
#endif // INSTRUMENT

    // if adding first item to root bin
    // and there is an endless supply available
    // make a new copy ready for next time
    if( ! bin->isSub() )
    {
        if( bin->canCopy() )
        {
            e.add( bin_t( new cBin( bin ) ));
        }
    }

    // add item to bin contents
    bin->add( item );

    // locate item relative to parent bin
    item->locate( bin->locX(), bin->locY() );

    // create new bin from remaining space to right of inserted item
    bin_t newbin = bin_t( new cBin( "", bin->sizX() - item->sizX(), item->sizY() ));
    newbin->locate( bin->locX() + item->sizX(), bin->locY() );
    if( bin->isSub() )
        newbin->parent( bin->parent() );
    else
        newbin->parent( bin );
    e.add( newbin );

    // create new bin below and to right of inserted item
    newbin = bin_t( new cBin( "", bin->sizX(), bin->sizY() - item->sizY() ));
    newbin->locate( bin->locX(), bin->locY() + item->sizY() );
    if( bin->isSub() )
        newbin->parent( bin->parent() );
    else
        newbin->parent( bin );
    e.add( newbin );
}
/// Remove unused and sub bins
void RemoveUnusedBins( cPackEngine& e )
{
    vector< bin_t >& bins = e.bins();
    bins.erase(
        remove_if(
            bins.begin(),
            bins.end(),
            [ ] ( bin_t b )
    {
        if( b->isSub() )
            return true;
        return ( ! b->isUsed() );

    } ),
    bins.end() );
}

void SortItemsIntoDecreasingSize( cPackEngine& e )
{
    vector<item_t>& items = e.items();
    sort( items.begin(), items.end(),
          []( item_t a, item_t b )
    {
        return a->size() > b->size();
    });
}
void SortBinsIntoIncreasingSize( cPackEngine& e )
{
    auto& bins = e.bins();
    sort( bins.begin(), bins.end(),
          []( bin_t a, bin_t b )
    {
        return a->size() < b->size();
    });
}

/// Pack items into bins
void Pack( cPackEngine& e )
{
    // try packing larger items first
    // so the smaller may fit into odd remaining spaces
    SortItemsIntoDecreasingSize( e );

    // loop until no more items can be packed
    while( 1 )
    {
        // true if an item was successfully packed
        bool itemPacked = false;

        // try fitting into the smaller spaces first
        SortBinsIntoIncreasingSize( e );

        // loop over items
        for( item_t item : e.items() )
        {
            if( item->isPacked() )
                continue;

            // item is waiting to be packed

            // loop over bins
            for( bin_t bin : e.bins() )
            {
                if( bin->isPacked() )
                    continue;

                // bin is empty

                if( Fits( item, bin ) )
                {
                    // item fits into bin

                    Add( e, bin, item );
                    itemPacked = true;

                    // packing the item invalidates iterators
                    // so we must restart the loops
                    break;
                }
            }
            if( itemPacked )
                break;
        }

        // give up if no item found that fit anywhere
        if( ! itemPacked )
        {
#ifdef INSTRUMENT
            cout << "no more items will fit\n";
#endif // INSTRUMENT
            break;
        }
    }

    // remove any unused bins
    // and sub bins created from remaining space when item was packed into bin
    RemoveUnusedBins( e );
}



/// Number of bins used
int BinCount( cPackEngine& e)
{
    int count = 0;
    for( bin_t bin : e.bins() )
    {
        if( bin->isUsed() )
            count++;
    }
    return count;
}
/// Packing result as a CSV string
std::string CSV( cPackEngine& e )
{
    stringstream ss;
    ss << "bin_name,item_name,item_left,item_top,rotated\n";
    for( bin_t bin : e.bins() )
    {
        for( item_t item : bin->contents() )
        {
            ss << bin->userID() << "," << item->userID()
               << "," << item->locX() <<"," << item->locY() << ",";
            if( item->isSpun() )
                ss << "rotated";
            else
                ss << "fixed";
            ss << "\n";
        }
    }
    return ss.str();
}
}

