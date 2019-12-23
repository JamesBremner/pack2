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
       <<" " << sizX() <<"x"<< sizY()
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
            cBin* copy = new cBin( bin );
            e.add( bin_t( copy ));
        }
    }

    // add item to bin contents
    bin->add( item );

    // locate item relative to parent bin
    item->locate( bin->locX(), bin->locY() );

    // dimension of remaining space to right of inserted item
    int xs1 = bin->sizX() - item->sizX();
    int ys1 =  item->sizY();
    int max12 = xs1 * ys1;

    // dimension of remaining space below and to right of inserted item
    int xs2 = bin->sizX();
    int ys2 = bin->sizY() - item->sizY();
    if( xs2 * ys2 > max12 )
        max12 = xs2 * ys2;

    // dimension of remaining space to right and below inserted item
    int xs3 = bin->sizX() - item->sizX();
    int ys3 = bin->sizY();

    // dimension of remaining space below inserted item
    int xs4 = item->sizX();
    int ys4 = bin->sizY() - item->sizY();

    int max34 = xs3 * ys3;
    if( xs4 * ys4 > max34 )
        max34 = xs4 * ys4;

//    if( max12 >= max34 )
//    {
//        bin_t newbin = bin_t( new cBin( "", xs1, ys1 ));
//        newbin->locate( bin->locX() + item->sizX(), bin->locY() );
//        if( bin->isSub() )
//            newbin->parent( bin->parent() );
//        else
//            newbin->parent( bin );
//        e.add( newbin );
//        cout << "added1 " << newbin->text();
//        //MergeUnusedSpace( e, newbin );
//        newbin = bin_t( new cBin( "", xs2, ys2 ));
//        newbin->locate( bin->locX(), bin->locY() + item->sizY() );
//        if( bin->isSub() )
//            newbin->parent( bin->parent() );
//        else
//            newbin->parent( bin );
//        e.add( newbin );
//        cout << "added2 " << newbin->text();
//    }
//    else
//    {
    bin_t newbin = bin_t( new cBin( "", xs3, ys3 ));
    newbin->locate( bin->locX() + item->sizX(), bin->locY() );
    if( bin->isSub() )
        newbin->parent( bin->parent() );
    else
        newbin->parent( bin );
    e.add( newbin );
    //cout << "added3 " << newbin->text();
    newbin = bin_t( new cBin( "", xs4, ys4 ));
    newbin->locate( bin->locX(), bin->locY() + item->sizY() );
    if( bin->isSub() )
        newbin->parent( bin->parent() );
    else
        newbin->parent( bin );
    e.add( newbin );
    //cout << "added4 " << newbin->text();
//    }

    MergeUnusedOnRight( e );

    //MergeUnusedFromBottomRight( e, bin );
}
void MergeUnusedFromBottomRight( cPackEngine& e, bin_t bin )
{
    if( bin->parent() )
        bin = bin->parent();
    std::cout << "looking for merge in bin " << bin->text();

    std::vector<bin_t> spaces;
    for( bin_t b : e.bins() )
    {
        if( ! b->parent() )
            continue;
        if( b->parent() != bin )
            continue;
        if( b->isUsed() )
            continue;
        spaces.push_back( b );
    }
    bin_t br;
    for( bin_t b : spaces )
    {
        if( b->right() != bin->right() )
            continue;
        if( b->bottom() != bin->bottom() )
            continue;
        br = b;
        break;
    }
    if( ! br )
        return;
    std::cout << " bottom right " << br->text();

    // find merge candidates on left of bottom right
    std::vector<bin_t> cands;
    for( bin_t b : spaces )
    {
        if( b->right() == br->locX() )
        {
            if( br->locY() <= b->bottom() )
            {
                cands.push_back( b );
            }
        }
    }
    if( cands.size() )
    {

        // sort into increasing Y
        sort( cands.begin(), cands.end(),
              []( bin_t a, bin_t b )
        {
            return a->locY() < b->locY();
        });


        bool nobreak = true;
        bin_t prev;
        for( bin_t b : cands )
        {
            if( ! prev )
            {
                prev = b;
                continue;
            }
            if( prev->bottom() != b->locY() )
            {
                nobreak = false;
                break;
            }
            prev = b;
        }
        if( nobreak )
        {
            if( cands[0]->locY() <= br->locY() )
            {
                bin_t newbin = bin_t( new cBin(
                                          bin,
                                          cands[0]->locX(),
                                          cands[0]->locY(),
                                          cands[0]->sizX(),
                                          br->locY() - cands[0]->locY() ) );
                e.add( newbin );
                cands[0]->sizY( cands[0]->sizY()-newbin->sizY() );
                cands[0]->locY( br->locY() );
            }
            if( cands[0]->locY() == br->locY() )
            {
                int maxW = bin->sizX();
                for( bin_t b : cands )
                {
                    if( b->sizX() < maxW )
                        maxW = b->sizX();
                }

                // construct merged bin
                bin_t newbin = bin_t( new cBin(
                                          bin,
                                          br->locX()-maxW,
                                          cands[0]->locY(),
                                          maxW+br->sizX(), br->sizY() ));
                e.add( newbin );
                std::cout << "newbin " << newbin->text();

                for( bin_t b : cands )
                {
                    b->sizX( b->sizX()-maxW );
                }
                br->sizX( 0 );

                //remove bins that have no X dimension
                RemoveZeroBins( e );
            }
        }
    }
    // find merge candidates on top of bottom right
    cands.clear();
    for( bin_t b : spaces )
    {
        if( b->bottom() == br->locY() )
        {
            if( br->locX() <= b->locX() )
            {
                cands.push_back( b );
            }
        }
    }
    if( cands.size() )
    {
        // sort into increasing X
        sort( cands.begin(), cands.end(),
              []( bin_t a, bin_t b )
        {
            return a->locX() < b->locX();
        });
        bool nobreak = true;
        bin_t prev;
        for( bin_t b : cands )
        {
            if( ! prev )
            {
                prev = b;
                continue;
            }
            if( prev->right() != b->locX() )
            {
                nobreak = false;
                break;
            }
            prev = b;
        }
        if( nobreak )
        {
            if( cands[0]->locX() < br->locX() )
            {
                bin_t newbin = bin_t( new cBin(
                                          bin,
                                          cands[0]->locX(),
                                          cands[0]->locY(),
                                          cands[0]->sizX() - cands[0]->locX(),
                                          br->locY() ) );
                e.add( newbin );
                cands[0]->sizX( cands[0]->sizX()-newbin->sizX() );
                cands[0]->locX( br->locX() );
            }

            if( cands[0]->locX() == br->locX() )
            {
                int maxH = bin->sizY();
                for( bin_t b : cands )
                {
                    if( b->sizY() < maxH )
                        maxH = b->sizY();
                }
                // construct merged bin
                bin_t newbin = bin_t( new cBin(
                                          bin,
                                          br->locY()-maxH,
                                          cands[0]->locX(),
                                          br->sizX(), br->sizY()+maxH ));
                e.add( newbin );
                std::cout << "newbin " << newbin->text();

                for( bin_t b : cands )
                {
                    b->sizY( b->sizY()-maxH );
                }
                br->sizX( 0 );

                //remove bins that have no X dimension
                RemoveZeroBins( e );
            }
        }
    }
}
/** check for overlap
// https://stackoverflow.com/a/3269471/16582
// [x1:x2] and [y1:y2],
// x1 <= y2 && y1 <= x2
*/
static std::pair<int,int> isOverlap( int ra1, int ra2, int rb1, int rb2 )
{
    if( ! ( ra1 <= rb2 && rb1 < ra2 ) )
        return std::make_pair(-1,-1);
    return std::make_pair(
               std::max( ra1, rb1 ),
               std::min( ra2, rb2 ));

}
class cRange
{
public:
    int first;
    int second;
    bool valid;
    cRange()
        : valid( false )
    {

    }
    cRange( int a, int b )
        : first( a )
        , second( b )
        , valid( a < b )
    {

    }
};
class cRangeOverlap
{
public:
    cRange a;   // first input range
    cRange b;   // second input range
    cRange o;   // overlap, -1,-1 if no overlap
    cRange ae;  // extra in first range
    cRange be;  // extra in second range


    void Calculate()
    {
        o.valid = false;
        ae.valid = false;
        be.valid = false;

        if( ! ( a.valid && b.valid ) )
            return;

        if( ! isOverlap() )
            return;

        // calculate the overlap range
        o.first  = std::max( a.first, b.first );
        o.second = std::min( a.second, b.second );
        o.valid  = true;

        if( a.first < o.first )
        {
            ae.first = a.first;
            ae.second = o.first;
            ae.valid = true;
        }
        if( a.second > o.second )
        {
            ae.first = o.second;
            ae.second = a.second;
            ae.valid = true;
        }
        if( b.first < o.first )
        {
            be.first = b.first;
            be.second = o.first;
            be.valid = true;
        }
        if( b.second > o.second )
        {
            be.first = o.second;
            be.second = b.second;
            be.valid = true;
        }

    }
private:
    bool isSane()
    {
        if( a.first >= a.second )
            return false;
        if( b.first >= a.second )
            return false;
        return true;
    }
    /** https://stackoverflow.com/a/3269471/16582
    // [x1:x2] and [y1:y2],
    // x1 <= y2 && y1 <= x2
    */
    bool isOverlap()
    {
        return ( a.first <= b.second && b.first < a.second );
    }
};

void MergePairs( cPackEngine& e )
{
    bool fmerged = true;
    while( fmerged )
    {
        fmerged = false;
        for( auto sub1 : e.bins() )
        {
            if( ! sub1->isSub() )
                continue;
            if( sub1->isUsed() )
                continue;

            for( auto sub2 : e.bins() )
            {
                if( ! sub2->isSub() )
                    continue;
                if( sub2->isUsed() )
                    continue;
                if( sub1->parent()->progID() != sub2->parent()->progID() )
                    continue;
                if( sub1->progID() == sub2->progID() )
                    continue;

                fmerged = MergePair( e, sub1, sub2 );
                if( fmerged )
                    break;
            }
            if( fmerged )
                break;
        }
    }
}

bool MergePair( cPackEngine& e, bin_t sub1, bin_t sub2 )
{
    if( sub2->right() == sub1->locX() )
    {
        // sub2 to right of sub1
        // check for overlap

        cRangeOverlap overlap;
        overlap.a = cRange( sub1->locY(), sub1->bottom() );
        overlap.b = cRange( sub2->locY(), sub2->bottom() );
        overlap.Calculate();
        if( overlap.o.valid )
        {
            // will merging give a larger space
            int mwidth = sub1->sizX()+sub2->sizX();
            int mheight = overlap.o.second - overlap.o.first;
            int ma = mwidth * mheight;
            if(  ma > sub1->size() && ma > sub2->size() )
            {
                bin_t merge = bin_t( new cBin(
                                         sub1->parent(),
                                         sub2->locX(), overlap.o.first,
                                         mwidth, mheight  ));
                //std::cout << "merge " << merge->text();
                e.add( merge );

                if( overlap.ae.valid )
                {
                    sub1->locY( overlap.ae.first );
                    sub1->sizY( overlap.ae.second - overlap.ae.first );
                }
                else
                    sub1->sizY( 0 );

                if( overlap.be.valid )
                {
                    sub2->locY( overlap.be.first );
                    sub2->sizY( overlap.be.second - overlap.be.first );
                }
                else
                    sub2->sizY( 0 );

                RemoveZeroBins( e );

                return true;

            }
        }
    }
    if( sub2->bottom() == sub1->locY() )
    {
        // sub2 above sub1
        // check for overlap

        cRangeOverlap overlap;
        overlap.a = cRange( sub1->locX(), sub1->right() );
        overlap.b = cRange( sub2->locX(), sub2->right() );
        overlap.Calculate();
        if( overlap.o.valid )
        {
            // will merging give a larger space
            int mwidth = overlap.o.second - overlap.o.first;
            int mheight = sub1->sizY() + sub2->sizY();
            int ma = mwidth * mheight;
            if(  ma > sub1->size() && ma > sub2->size() )
            {
                bin_t merge = bin_t( new cBin(
                                         sub1->parent(),
                                         overlap.o.first, sub2->locY(),
                                         mwidth, mheight  ));
                //std::cout << "merge " << merge->text();
                e.add( merge );

                if( overlap.ae.valid )
                {
                    sub1->locX( overlap.ae.first );
                    sub1->sizX( overlap.ae.second - overlap.ae.first );
                }
                else
                    sub1->sizX( 0 );

                if( overlap.be.valid )
                {
                    sub2->locX( overlap.be.first );
                    sub2->sizX( overlap.be.second - overlap.be.first );
                }
                else
                    sub2->sizX( 0 );

                RemoveZeroBins( e );

                return true;
            }
        }
    }

    return false;
}

void Merge( cPackEngine& e, bin_t above, bin_t below )
{
    std::cout << "Merge " << above->text();
    std::cout << "And " << below->text();

    int mxl, myl, mxs, mys;
    int m2xl, m2yl, m2xs, m2ys;
    if( above->sizX() >= below->sizX() )
    {
        // below is narrower
        mxl = below->locX();
        myl = above->locY();
        mxs = below->sizX();
        mys = above->sizY() + below->sizY();
        m2xl = above->locX();
        m2yl = above->locY();
        m2xs = above->sizX() - below->sizX();
        m2ys = below->locY() - above->locY();
    }
    else
    {
        // above is narrower
        mxl = above->locX();
        myl = above->locY();
        mxs = above->sizX();
        mys = above->sizY() + below->sizY();
    }
    bin_t mergebin1 = bin_t( new cBin( "", mxs, mys ));
    mergebin1->locate( mxl, myl );
    mergebin1->parent( above->parent() );
    bin_t mergebin2 = bin_t( new cBin( "", m2xs, m2ys ));
    mergebin2->locate( m2xl, m2yl );
    mergebin2->parent( above->parent() );
    std::cout << " gives " << mergebin1->text();
    std::cout << "       " << mergebin2->text();
}
static vector< bin_t > UnusedBinsOnRight( cPackEngine& e, bin_t base )
{
    vector< bin_t > ret;

    for( auto sub : e.bins() )
    {
        if( ! sub->isSub() )
            continue;
        if( sub->parent()->progID() != base->progID() )
            continue;
        if( sub->isPacked() )
            continue;
        if( sub->right() != base->right() )
            continue;
        if(  sub->sizX() < e.Algorithm().MergeOnRightCandMinWidth )
            continue;

        ret.push_back( sub );

#ifdef INSTRUMENT
        std::cout << "candidate " << sub->text();
#endif
    }
    return ret;
}
void MergeUnusedOnRight( cPackEngine& e )
{
    vector< bin_t > vmerges;
    for( auto base : e.bins() )
    {
        if( base->isSub() )
            continue;
        if( ! base->isUsed() )
            continue;

#ifdef INSTRUMENT
        std::cout << "Looking for merge on right in " << base->text();
#endif // INSTRUMENT

        vector< bin_t > vcan = UnusedBinsOnRight( e, base );

        // where at least 2 condidates found?
        if( vcan.size() < 2 )
            continue;

        int narrowest, topmost, bottommost, biggest;
        bool first = true;
        for( auto sub : vcan )
        {
            if( first )
            {
                narrowest = sub->sizX();
                topmost   = sub->locY();
                bottommost= sub->bottom();
                biggest   = sub->size();
                first = false;
            }
            else
            {
                if( sub->sizX() < narrowest )
                    narrowest = sub->sizX();
                if( sub->locY() < topmost )
                    topmost = sub->locY();
                if( sub->locY() > bottommost )
                    bottommost = sub->bottom();
                if( sub->size() > biggest )
                    biggest = sub->size();
            }

        }

        ///  check that no breaks among candidates in Y direction
        sort( vcan.begin(), vcan.end(),
              []( bin_t a, bin_t b )
        {
            return ( a->locY() < b->locY() );
        });
        bool fOK = true;
        bin_t prev_in_y;
        for( auto b : vcan )
        {
            if( prev_in_y == NULL )
            {
                prev_in_y = b;
                continue;
            }
            if( prev_in_y->bottom() != b->locY() )
            {
                //std::cout << "break\n" << prev_in_y->text() << b->text() <<"\n";
                fOK = false;
                break;
            }
            prev_in_y = b;
        }
        if( ! fOK )
            continue;

        int mxs = narrowest;
        int mys = bottommost - topmost;

#ifdef INSTRUMENT
        std::cout << "narrowest " << narrowest
                  << " biggest " << biggest
                  << " mxs " << mxs << " mys " << mys << "\n";
#endif // INSTRUMENT

        if( mxs * mys < biggest )
            return;

        int mxl = base->right() - narrowest;
        for( auto b : vcan )
        {
            b->sizX( mxl - b->locX() );
            std::cout << "modified " << b->text();
        }

        bin_t mergebin = bin_t( new cBin( "",
                                          narrowest,
                                          bottommost - topmost ));
        mergebin->locate( mxl, topmost );
        mergebin->parent( base );
        vmerges.push_back( mergebin );
        std::cout << "merge1 " << mergebin->text();
    }
    for( auto mb : vmerges )
        e.add( mb );
}
void MergeUnusedSpace( cPackEngine& e, bin_t newbin )
{
    int right = newbin->right();

    cout << "=> trying to merge " << newbin->text() <<" right " << right << "\n";

    for( auto bin : e.bins() )
    {
        if( bin->progID() == newbin->progID() )
            continue;
        if( bin->isPacked() )
            continue;
        if( bin->parent() != newbin->parent() )
            continue;
        //cout << bin->text();
        if( bin->right() == right )
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
            cout << "merge candidate " << bin->text() << "\n";

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
                exs = exl - bin->sizX();
                eys = newbin->sizY();
            }
            else
            {
                exl = bin->locX();
                eyl = bin->locY();
                exs = exl - newbin->sizX();
                eys = bin->sizY();
            }

            //  construct bin from left over space
            bin_t extrabin = bin_t( new cBin( "", exs, eys ));
            extrabin->locate( exl, eyl );
            if( bin->isSub() )
                extrabin->parent( bin->parent() );
            else
                extrabin->parent( bin );

            // check that the merge is an improvement
//            int max_old_area = bin->size();
//            if( newbin->size() > max_old_area )
//                max_old_area = newbin->size();
//            int max_new_area = mergebin->size();
//            if( extrabin->size() > max_new_area )
//                max_new_area = extrabin->size();
//            if( max_old_area >= max_new_area )
//                return;

            std::cout << "merging!\n";
            std::cout << mergebin->text();
            std::cout << extrabin->text();

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
    //SortItemsIntoDecreasingSize( e );

    SortItemsIntoDecreasingAwkward( e );

    PackSortedItems( e );

    if( e.Algorithm().fTryEveryItemFirst )
    {
        int bestBinCount = BinCount( e );

        vector<item_t> sortedItems = e.items();

        for( int firstItem = 1; firstItem < (int)e.items().size(); firstItem++ )
        {
            e.items().clear();
            e.items().push_back( sortedItems[ firstItem ] );
            for( auto i : sortedItems )
            {
                if( i->progID() == sortedItems[ firstItem ]->progID() )
                    continue;
                e.items().push_back( i );
            }

            PackSortedItems( e );

            if( BinCount( e ) < bestBinCount )
                return;
        }

        e.items() = sortedItems;
        PackSortedItems( e );
    }
}

void PackSortedItems( cPackEngine& e )
{
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
            cout << "all items successfully packed in "
                 << BinCount( e ) << " bins\n";
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
              <<" " << item->sizX() <<"x"<< item->sizY()
              << " into bin ";
    if( bin->parent() )
        std::cout << bin->parent()->userID();
    std::cout <<" "<<bin->text();
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
    //std::cout << "unused spaces\n";
    std::vector< bin_t >& bins = e.bins();
    bins.erase(
        remove_if(
            bins.begin(),
            bins.end(),
            [ ] ( bin_t b )
    {
        if( b->isSub() )
        {
            //std::cout << b->text();
            return true;
        }
        return ( ! b->isUsed() );

    } ),
    bins.end() );
}
void RemoveZeroBins( cPackEngine& e )
{
    std::vector< bin_t >& bins = e.bins();
    bins.erase(
        remove_if(
            bins.begin(),
            bins.end(),
            [ ] ( bin_t b )
    {
        return ( (! b->sizX()) || (! b->sizY() ) );
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
void SortItemsIntoDecreasingAwkward( cPackEngine& e )
{
    itemv_t most_awkward;
    itemv_t unsorted;
    for( item_t item : e.items() )
    {
        if( item->sizX() > e.bins()[0]->sizX() ||
                item->sizX() > e.bins()[0]->sizY() ||
                item->sizY() > e.bins()[0]->sizX() ||
                item->sizY() > e.bins()[0]->sizY()
          )
            most_awkward.push_back( item );
        else
            unsorted.push_back( item );
    }
    sort( unsorted.begin(), unsorted.end(),
          []( item_t a, item_t b )
    {
        return a->size() > b->size();
    });
    e.items().clear();
    for( item_t item : most_awkward )
    {
        e.items().push_back( item );
    }
    for( item_t item : unsorted )
    {
        e.items().push_back( item );
    }
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
//            if( ( a->isPacked()) && ( ! b->isPacked() ) )
//                return false;
            if( a->copyCount() <= b->copyCount() )
                return true;
        }
        return false;
    });

//#ifdef INSTRUMENT
//    for( auto bin : e.bins() )
//    {
//        std::cout << "sorted bin ";
//        if( bin->parent() )
//            std::cout << bin->parent()->userID();
//        std::cout <<" "<<bin->text()
//                  << " packed "<<bin->isPacked()
//                  << "\n";
//    }
//#endif // INSTRUMENT

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
