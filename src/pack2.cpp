#include <iostream>

#include "pack2.h"

//#define INSTRUMENT 1

using namespace std;

namespace pack2
{

cBin::cBin( bin_t old )
    : cShape( old->userID(), old->sizX(), old->sizY() )
    , myfCopy( true )
    , myCopyCount( old->myCopyCount+1)
    , myParent( NULL )
{
    std::string sid = userID() ;
    int p = sid.find("_cpy");
    if( p == -1 )
    {
        sid += "_cpy2";
    }
    else
    {
        sid = sid.substr(0,p+4) + std::to_string( myCopyCount);
    }
    userID( sid );
}

std::string cBin::text()
{
    std::stringstream ss;
    if( myParent )
        ss << parent()->userID();
    ss <<" "<<userID() <<" " << progID()
       <<" " << sizX() <<" "<< sizY()
       <<" at " << locX() <<" "<< locY() << "\n";
    return ss.str();
}

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
    MergeUnusedSpace( e, newbin );

    if( bin->sizY() - item->sizY() > 0 )
    {
        // create new bin below and to right of inserted item
        newbin = bin_t( new cBin( "", bin->sizX(), bin->sizY() - item->sizY() ));
        newbin->locate( bin->locX(), bin->locY() + item->sizY() );
        if( bin->isSub() )
            newbin->parent( bin->parent() );
        else
            newbin->parent( bin );
        e.add( newbin );
    }
}

void MergeUnusedSpace( cPackEngine& e, bin_t newbin )
{
    cout << "=> trying to merge " << newbin->text();

    int right = newbin->locX() + newbin->sizX();
    for( auto bin : e.bins() )
    {
        if( bin->progID() == newbin->progID() )
            continue;
        if( bin->isPacked() )
            continue;
        if( bin->parent() != newbin->parent() )
            continue;
        cout << bin->text();
        if( bin->locX() + bin->sizX() == right )
        {
            bool newAbove = false;
            if( bin->locY() < newbin->locY() )
            {
                if( bin->locY() + bin->sizY() != newbin->locY() )
                    continue;
                else
                {
                    newAbove = false;

                }
            }
            else
            {
                if( newbin->locY() + newbin->sizY() != bin->locY() )
                    continue;
                else
                    newAbove = true;
            }
            cout << "merge candidate\n";

            bool newWidest = ( newbin->locX() < bin->locX() );

            int mxs = right - bin->locX();
            if( mxs > right - newbin->locX() )
                mxs = right - newbin->locX();
            int mys = bin->sizY() + newbin->sizY();
            int mxl = newbin->locX();
            if( mxl < bin->locX() )
            {
                mxl = bin->locX();
            }
            int myl = bin->locY();
            if( newAbove )
                myl = newbin->locY();

            // construct merged bin
            bin_t mergebin = bin_t( new cBin( "", mxs, mys ));
            mergebin->locate( mxl, myl );
            if( bin->isSub() )
                mergebin->parent( bin->parent() );
            else
                mergebin->parent( bin );
            cout << mergebin->text();

            int exl, eyl, exs, eys;
            if( newWidest )
            {
                exl = newbin->locX();
                eyl = newbin->locY();
                      exs = exl - bin->locX();
                eys = newbin->sizY();
            }
            else
            {
                exl = bin->locX();
                eyl = bin->locY();
                      exs = exl - newbin->locX();
                eys = bin->sizY();
            }

            //  construct bin from left over space
            bin_t extrabin = bin_t( new cBin( "", exs, eys ));
            extrabin->locate( exl, eyl );
            if( bin->isSub() )
                extrabin->parent( bin->parent() );
            else
                extrabin->parent( bin );
            cout << extrabin->text();

            // check that the merge is an improvement
            int max_old_area = bin->size();
            if( newbin->size() > max_old_area )
                max_old_area = newbin->size();
            int max_new_area = mergebin->size();
            if( extrabin->size() > max_new_area )
                max_new_area = extrabin->size();
            if( max_old_area >= max_new_area )
                return;

            // remove the two merged bins
            int remID = newbin->progID();
            e.bins().erase(
                remove_if(
                    e.bins().begin(),
                    e.bins().end(),
                    [ remID ] ( bin_t b )
            {
                return( b->progID() == remID );
            } ),
            e.bins().end() );
            remID = bin->progID();
            e.bins().erase(
                remove_if(
                    e.bins().begin(),
                    e.bins().end(),
                    [ remID ] ( bin_t b )
            {
                return( b->progID() == remID );
            } ),
            e.bins().end() );

            // add the results of merging
            e.add( mergebin );
            e.add( extrabin );
        }
    }
}

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
        bool unpackedfound = false;

        // try fitting into the smaller spaces first
        SortBinsIntoIncreasingSize( e );

        // loop over items
        for( item_t item : e.items() )
        {
            if( item->isPacked() )
                continue;
            unpackedfound = true;

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

        if( ! unpackedfound )
        {
            cout << "all items successfully packed\n";
            break;
        }
        if( ! itemPacked )
        {
            cout << "no more items will fit\n";
            break;
        }
    }

    // remove any unused bins
    // and sub bins created from remaining space when item was packed into bin
    RemoveUnusedBins( e );
}



bool Fits( item_t item, bin_t bin )
{
#ifdef INSTRUMENT
    std::cout << "Trying to fit item " << item->progID() <<" "<< item->userID()
              <<" " << item->sizX() <<" "<< item->sizY()
              << " into bin ";
    if( bin->parent() )
        std::cout << bin->parent()->userID();
    std::cout <<" "<<bin->userID() <<" " << bin->progID()
              <<" " << bin->sizX() <<" "<< bin->sizY()
              <<" at " << bin->locX() <<" "<< bin->locY() << "\n";
#endif // INSTRUMENT

    if ( item->sizX() <= bin->sizX() && item->sizY() <= bin->sizY() )
        return true;
    if( item->canSpin() )
        item->spin();
    else
        return false;
    if ( item->sizX() <= bin->sizX() && item->sizY() <= bin->sizY() )
        return true;
    item->unspin();
    return false;
}

void RemoveUnusedBins( cPackEngine& e )
{
    std::vector< bin_t >& bins = e.bins();
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
    std::vector<item_t>& items = e.items();
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
        // return true if a smaller than b and in lower or same copy count bin
        if( a->size() < b->size() )
        {
            if( a->copyCount() <= b->copyCount() )
                return true;
        }
        return false;

    });

#ifdef INSTRUMENT
    for( auto bin : e.bins() )
    {
        std::cout << "bin ";
        if( bin->parent() )
            std::cout << bin->parent()->userID();
        std::cout <<" "<<bin->userID() <<" " << bin->progID()
                  <<" " << bin->sizX() <<" "<< bin->sizY()
                  <<" at " << bin->locX() <<" "<< bin->locY() << "\n";
    }
#endif // INSTRUMENT

}
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
std::string CSV( cPackEngine& e )
{
    std::stringstream ss;
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

std::string Unpacked( cPackEngine& e )
{
    std::stringstream ss;
    for( item_t item : e.items() )
    {
        if( ! item->isPacked() )
            ss << item->userID() << ",";
    }
    return ss.str();
}
std::string CutList( cPackEngine& e )
{
    std::stringstream ss;

    for( bin_t b : e.bins() )
    {
        cCutList L;
        CutListBin( b, L );
        ss << L.get();
    }

    return ss.str();
}
std::string DrawList( cPackEngine& e )
{
    std::stringstream ss;
    for( bin_t bin : e.bins() )
    {
        ss << DrawList( bin );
    }
    return ss.str();
}
std::string DrawList( bin_t bin )
{
    std::stringstream ss;

    ss << "S.bin,"<<bin->userID()<<"\n";
    ss << "S.color( 0xFF0000 );\n";
    ss << "S.rectangle( { "
       << 0 <<", "
       << 0 << ", "
       << (int)( bin->sizX() )  << ", "
       << (int)( bin->sizY() ) << " } );\n";
    ss << "S.color( 0x00FF );\n";

    for( item_t item : bin->contents() )
    {
        ss << DrawList( item );
    }
    return ss.str();
}
std::string DrawList( item_t item )
{
    const float scale = 1;
    float left = item->locX() / scale;
    float top  = item->locY() / scale;
    float w = item->sizX() / scale;
    float h = item->sizY() / scale;

    std::stringstream ss;
    ss << fixed /* << setprecision(1) */;
    ss << "S.rectangle( { "
       << left <<", "
       << top << ", "
       << w << ", "
       << h << " } );\n";
    ss << "S.text(\"" << item->userID() <<"\", {"
       << left+5 <<", "
       << top+5 <<", 50,30 } );\n";

    return ss.str();
}

int cShape::myLastProgID = -1;

}
