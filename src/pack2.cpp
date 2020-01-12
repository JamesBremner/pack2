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
       <<" at " << locX() <<" "<< locY();
    if( isPacked() )
        ss << " packed";
    ss << "\n";
    return ss.str();
}

bool CheckForOverlap( cPackEngine& e, bin_t newpack )
{
    for( bin_t packed : e.bins() )
    {
        if( ! packed->isSub() )
            continue;
        if( ! packed->isPacked() )
            continue;
        if( packed->origID() != newpack->origID() )
            continue;
        if( packed->progID() != newpack->progID() )
            continue;
        if( packed->isOverlap( *newpack.get() ) )
            return true;
    }
    return false;
}

void Add( cPackEngine& e, bin_t bin, item_t item )
{
#ifdef INSTRUMENT
    cout << "Adding item " << item->text() << " to bin " << bin->text() << "\n";
#endif // INSTRUMENT

    if( CheckForOverlap( e, bin ) )
        throw std::runtime_error("Add Adding an overlapped item");

    // add item to bin contents
    bin->add( item );

    // locate item relative to parent bin
    item->locate( bin->locX(), bin->locY() );

    item->pack();

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


    if( bin->isSub() )
    {
        // shrink space to hold item exactly
        bin->sizX( item->sizX() );
        bin->sizY( item->sizY() );
    }
    else
    {
        // construct space for item
        bin = bin_t ( new cBin( bin,
                                0, 0,
                                item->sizX(), item->sizY() ));
        bin->pack();
        e.add( bin );
    }

    //ConsumeSpace( e, bin );

    MergePairs( e, bin );

    //MergeTriple( e, bin );

    //MergeUnusedOnRight( e );

    //MergeUnusedFromBottomRight( e, bin );
}

void AddAtBottomRight( cPackEngine& e, bin_t parent, item_t item )
{
#ifdef INSTRUMENT
    cout << "AddAtBottomRight " << item->userID() << " to bin " << parent->progID() << "\n";
#endif // INSTRUMENT

    // add item to bin contents
    parent->add( item );

    // locate item relative to parent bin
    item->locate(
        parent->right() - item->sizX(),
        parent->bottom() - item->sizY() );

    bin_t newbin = bin_t( new cBin( "", item->sizX(), item->sizY() ));
    newbin->locate( item->locX(), item->locY() );
    newbin->parent( parent );
    newbin->pack();
    e.add( newbin );
    //std::cout << "Added at br " << newbin->text() << "\n";

    // reduce spaces that are consumed by packing item
    for( bin_t space : Spaces( e, parent ) )
    {
        space->subtract( *newbin.get() );
    }
    MergeAdjacentPairs( e, parent );
}

void ConsumeSpace( cPackEngine& e, bin_t add )
{
    for( bin_t space : Spaces( e, add->parent() ) )
        space->subtract( *add.get() );
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
        return ( a.first <= b.second && b.first <= a.second );
    }
};
void MergeTriple( cPackEngine& e,  bin_t bin )
{
    if( bin->parent() )
        bin = bin->parent();

    std::vector< bin_t > vspace;
    for( auto space : e.bins() )
    {
        if( ! space->isSub() )
            continue;
        if( space->isPacked() )
            continue;
        if( space->parent()->progID() != bin->progID() )
            continue;
        vspace.push_back( space );
    }
    if( (int) vspace.size() < 3 )
        return;

    bin_t bottom_right;
    for( auto space : vspace )
    {
        if( space->right() == bin->right() && space->bottom() == bin->bottom() )
        {
            bottom_right = space;
            break;
        }
    }
    if( ! bottom_right )
        return;

    std::cout << "bottom_right " << bottom_right->text();

    bin_t right;
    for( auto space : vspace )
    {
        if( space->progID() == bottom_right->progID() )
            continue;
        if( space->right() == bin->right() && space->bottom() == bottom_right->locY() )
        {
            right = space;
            break;
        }
    }
    bin_t left;
    for( auto space : vspace )
    {
        if( space->progID() == bottom_right->progID() )
            continue;
        if( space->right() == bottom_right->locX() && space->bottom() == bin->bottom() )
        {
            left = space;
            break;
        }
    }
    bin_t lr;
    if( left )
        lr = left;
    if( right )
        lr = right;
    if( ! lr )
        return;



}

bool MergeAdjacent( cPackEngine& e, bin_t sub1, bin_t sub2 )
{
    int dl, dr;
    int adj = sub1->isAdjacent( *sub2.get(), dl, dr );
    switch(  adj )
    {
    case 1:

        // sub2 is adjacent above sub1
        if( dl >= 0 && dr <= 0 )
        {
            // width of sub2 enclosed by sub1 width
            int mw = sub2->sizX();
            int mh = sub1->sizY() + sub2->sizY();
            int ma = mw * mh;
            if( ma > sub1->size() && ma > sub2->size() )
            {
                bin_t merge = bin_t( new cBin(
                                         sub1->parent(),
                                         sub2->locX(), sub2->locY(),
                                         mw, mh  ));
                e.add( merge );

                sub1->sizX( dl );

                bin_t rem1right = bin_t( new cBin(
                                             sub1->parent(),
                                             sub2->right(), sub1->locY(),
                                             -dr, sub1->sizY() ) );
                e.add( rem1right );

                // nothing left of sub2
                sub2->sizX( 0 );

#ifdef INSTRUMENT
                std::cout << "MargeAdjacent to " << merge->text()
                          << sub1->text() << sub2->text();
#endif // INSTRUMENT

                RemoveZeroBins(e);

                return true;
            }
        }
        break;
    default:
        break;
    }
    return false;
}
void MergeAdjacentPairs( cPackEngine& e,  bin_t bin )
{
    if( bin->parent() )
        bin = bin->parent();

    bool fmerged = true;
    while( fmerged )
    {
        fmerged = false;
        for( auto space1 : Spaces( e, bin ) )
        {
            for( auto space2 : Spaces( e, bin ) )
            {
                if( space2->progID() == space1->progID() )
                    continue;

                fmerged = MergeAdjacent( e, space1, space2 );
                if( fmerged )
                    break;
            }
            if( fmerged )
                break;
        }
    }
}

void MergePairs( cPackEngine& e, bin_t bin )
{
    if( bin->parent() )
        bin = bin->parent();

    bool fmerged = true;
    while( fmerged )
    {
        fmerged = false;
        for( auto space1 : e.bins() )
        {
            if( ! space1->isSub() )
                continue;
            if( space1->isPacked() )
                continue;
            if( space1->parent()->progID() != bin->progID() )
                continue;

            for( auto space2 : e.bins() )
            {
                if( ! space2->isSub() )
                    continue;
                if( space2->isPacked() )
                    continue;
                if( space1->parent()->progID() != space2->parent()->progID() )
                    continue;
                if( space1->progID() == space2->progID() )
                    continue;

                fmerged = MergePair( e, space1, space2 );
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
                e.add( merge );

#ifdef INSTRUMENT
                std::cout << "merge from\n" << sub1->text() << sub2->text();
                std::cout << sub1->size() <<" "<< sub2->size() <<" "<< ma << "\n";
#endif // INSTRUMENT

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

#ifdef INSTRUMENT
                std::cout << "to " << merge->text();
                std::cout << sub1->text();
                std::cout << sub2->text() << "\n";
#endif // INSTRUMENT

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
#ifdef INSTRUMENT
                std::cout << "above merge from\n" << sub1->text() << sub2->text();
                std::cout << sub1->size() <<" "<< sub2->size() <<" "<< ma << "\n";
#endif // INSTRUMENT

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

#ifdef INSTRUMENT
                std::cout << "to " << merge->text();
                std::cout << sub1->text();
                std::cout << sub2->text() << "\n";
#endif // INSTRUMENT

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
//            cout << mergebin->text();

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
    /* try packing larger items first
     so the smaller may fit into odd remaining spaces
     Use a a measure of size the sum of squares of the individual width snd length
     so that long thin items are packed first becuase such items are awkward to fit into odd spaces
    */
    SortItemsDecreasingSquaredDim( e );

    PackSortedItems( e );

    if( e.Algorithm().fTryEveryItemFirst )
    {
        // if requested try fitting every item first
        // perhaps a better fit may be found!

        int bestBinCount = BinCount( e );

        vector<item_t> sortedItems = e.items();

        bool improved = false;

        // loop over items
        for( int firstItem = 1; firstItem < (int)e.items().size(); firstItem++ )
        {
            // arrange for each item in turn to be fitted first
            e.items().clear();
            e.items().push_back( sortedItems[ firstItem ] );
            for( auto i : sortedItems )
            {
                if( i->progID() == sortedItems[ firstItem ]->progID() )
                    continue;
                e.items().push_back( i );
            }

            // try packing this item first
            PackSortedItems( e );

            // check for an improvement
            if( BinCount( e ) < bestBinCount )
            {
                improved = true;
                break;
            }
        }

        if( ! improved )
        {
            // no improvement, so redo the original pack
            e.items() = sortedItems;
            PackSortedItems( e );
        }
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
                if( bin->isSub() && bin->isPacked() )
                    continue;

#ifdef INSTRUMENT
                std::cout << "\n \nPackSortedItems try bin " << bin->text();
#endif

                if( Fits( item, bin ) )
                {
                    // item fits into space
                    Add( e, bin, item );
                    itemPacked = true;
                }
                else if( FitSlider( e, item, bin ) )
                {
                    // fits into multiple spaces
                    bin_t itemholder = bin_t( new cBin( bin,
                                                        item->locX(), item->locY(),
                                                        item->sizX(), item->sizY() ));
                    Add( e, itemholder, item );
                    itemPacked = true;
                }
                else if( FitFirstItem( e, item, bin) )
                {
                    // first item in new bin
                    itemPacked = true;
                }
                // packing the item invalidates iterators
                // so we must restart the loops
                if( itemPacked )
                    break;
            }
            if( itemPacked )
                break;
        }

        if( ! unpackedfound )
        {
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
    if( ! bin->isSpace() )
        return false;

#ifdef INSTRUMENT
    std::cout << "Trying to fit item " << item->progID() <<" "<< item->userID()
              <<" " << item->sizX() <<"x"<< item->sizY()
              << " into bin ";
    std::cout <<" "<<bin->text();
#endif // INSTRUMENT

    bool ret = false;
    if ( item->sizX() <= bin->sizX() && item->sizY() <= bin->sizY() )
    {
        ret = true;
    }
    else if( item->canSpin() )
    {
        item->spin();
        if ( item->sizX() <= bin->sizX() && item->sizY() <= bin->sizY() )
            ret = true;
        else
            item->unspin();
    }
#ifdef INSTRUMENT
    if( ret )
        std::cout << "item fits in space " << item->text() << bin->text() << "\n";
    else
        std::cout << "no fit\n";
#endif

    return ret;
}

bool FitsInMultipleSpaces( cPackEngine& e, item_t item, bin_t bin )
{
    if( ! e.Algorithm().fMultipleFit )
        return false;

    bin_t test( new cBin( bin,
                          bin->right() - item->sizX(), bin->bottom() - item->sizY(),
                          item->sizX(), item->sizY() ));
#ifdef INSTRUMENT
    std::cout << "=>FitsInMultipleSpaces\n";
    std::cout << "bin " << bin->text();
    std::cout << "item " << item->text() << "\n";
    std::cout << "test " << test->text();
#endif // INSTRUMENT


    for( bin_t pack : e.bins() )
    {
        if( pack->origID() != bin->origID() )
            continue;
        if( ! pack->isPacked() )
            continue;
        if( ! pack->isSub() )
            continue;
        if( pack->isOverlap( *test.get() ) )
        {
            //std::cout << "Overlapping\n" << pack->text() << test->text();
            return false;
        }
    }
    //std:: cout << "OK\n";
    return true;
}

bool FitFirstItem( cPackEngine& e, item_t item, bin_t bin )
{
    // check that bin is empty root
    if( bin->isSub() || bin->isUsed() )
        return false;
//#ifdef INSTRUMENT
    std::cout << "first item in bin " << item->text() << bin->text() << "\n";
//#endif
    bin->add( item );
    bin_t itemholder = bin_t( new cBin( bin,
                                        item->locX(), item->locY(),
                                        item->sizX(), item->sizY() ));
    itemholder->pack();
    item->pack();
    e.add( itemholder );
    e.add( bin_t( new cBin( bin,
                            item->right(), 0,
                            bin->sizX()-item->sizX(), item->sizY())));
    e.add( bin_t( new cBin( bin,
                            0, item->bottom(),
                            bin->sizX(), bin->sizY() - item->sizY())));
    RemoveZeroBins( e );

    // if endless supply available
    // make a new copy ready for next time
    if( bin->canCopy() )
        e.add( bin_t( new cBin( bin ) ));

    return true;
}

bool FitSlider( cPackEngine& e, item_t item, bin_t bin )
{
    const int increment = 20;

    if( ( ! bin->isPacked() ) || bin->isSub() )
        return false;

    // locate at bottom right
    item->locX( bin->sizX() - item->sizX() );
    item->locY( bin->sizY() - item->sizY() );

    while( true )
    {
        // overlap with any item packed into bin
        bool fOverlap = false;
        for( item_t content : bin->contents() )
        {
            if( content->isOverlap( *item.get() ) )
            {
                fOverlap = true;
                break;
            }
        }
        if( ! fOverlap )
        {
#ifdef INSTRUMENT
            std::cout << "FitSlider item " << item->text() << " into "<< bin->text() << "\n";
#endif // INSTRUMENT
            return true;
        }

        // Slide
        int x = item->locX() - increment;
        int y = item->locY();
        if( x < 0 )
        {
            x = bin->sizX() - item->sizX();
            y = item->locY() - increment;
            if( y < 0 )
                break;
        }
        item->locX( x );
        item->locY( y );
    }

    // std::cout << "FitSlider fail\n";
    return false;
}

binv_t Spaces( cPackEngine& e, bin_t bin )
{
    binv_t v;
    if( bin->isSub() )
        return v;
    for( bin_t space : e.bins() )
    {
        if( space->origID() != bin->origID() )
            continue;
        if( ! space->isSub() )
            continue;
        if( space->isPacked() )
            continue;
        v.push_back( space );
    }
    return v;
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
void SortItemsDecreasingSquaredDim( cPackEngine& e )
{
    std::vector<item_t>& items = e.items();
    sort( items.begin(), items.end(),
          []( item_t a, item_t b )
    {
        return ( a->sizX() * a->sizX() + a->sizY() * a->sizY() ) >
               ( b->sizX() * b->sizX() + b->sizY() * b->sizY() );
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

    RemoveZeroBins(e);

    sort( bins.begin(), bins.end(),
          []( bin_t a, bin_t b )
    {
        // always sort spaces first from lower copy counts
        int ac = a->copyCount();
        int bc = b->copyCount();
        if( ac != bc )
        {
            return ac < bc;
        }
        // sort speces first that have a smaller area
        if( a->size() <= b->size() )
        {
            return true;
        }
        return false;
    });

#ifdef INSTRUMENT
    for( auto bin : e.bins() )
    {
        if( ! bin->isPacked() )
        {
            std::cout << "sorted space ";
            std::cout <<" "<<bin->text();
        }
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

int cShape::isAdjacent(
    const cShape& other,
    int& dl, int& dr ) const
{
#ifdef INSTRUMENT
    std::cout << "isAdjacent " << text() << " | " << other.text() << "\n";
#endif // INSTRUMENT

    int ret = 0;
    if( myLocY == other.bottom() )
    {
        bool fOverlap = false;
        if( myLocX <= other.myLocX )
        {
            if( right() >= other.myLocX )
            {
                fOverlap = true;
            }
        }
        else
        {
            if( myLocX <= other.right() )
                fOverlap = true;
        }
        if( fOverlap )
        {
            // other is adjacent above
            ret = 1;
            dl = other.myLocX - myLocX;
            dr = other.right() - right();
        }
    }
    else if( myLocX == other.right() )
    {
        bool fOverlap = false;
        if( myLocY <= other.myLocY )
        {
            if( bottom() >= other.myLocY )
                fOverlap = true;
        }
        else
        {
            if( myLocY <= other.bottom() )
                fOverlap = true;
        }
        if( fOverlap )
        {
            // other is adjacent on left
            ret = 2;
            dl = other.myLocY - myLocY;
            dr = other.bottom() - bottom();
        }
    }
    return ret;
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


int cShape::myLastProgID = -1;

}
