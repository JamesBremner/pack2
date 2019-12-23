#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <cutest.h>

using namespace std;

#include "pack2.h"

TEST( MergePairs1 )
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    // equal spaces adjacent left/right even
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 50, 20, 20 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 30, 50, 20, 20 )));
    MergePairs( thePackEngine );
//    for( pack2::bin_t b : thePackEngine.bins() )
//        std::cout << b->text();

    CHECK_EQUAL( 2, (int)thePackEngine.bins().size() );
    CHECK_EQUAL(30, thePackEngine.bins()[1]->locX());
    CHECK_EQUAL(50, thePackEngine.bins()[1]->locY());
    CHECK_EQUAL(40, thePackEngine.bins()[1]->sizX());
    CHECK_EQUAL(20, thePackEngine.bins()[1]->sizY());
}

TEST( MergePairs2 )
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    // equal spaces adjacent left/right left higher
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 50, 20, 20 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 30, 49, 20, 20 )));
    MergePairs( thePackEngine );

//    for( pack2::bin_t b : thePackEngine.bins() )
//        std::cout << b->text();

    CHECK_EQUAL( 4, (int)thePackEngine.bins().size() );
    CHECK_EQUAL(50, thePackEngine.bins()[1]->locX());
    CHECK_EQUAL(69, thePackEngine.bins()[1]->locY());
    CHECK_EQUAL(20, thePackEngine.bins()[1]->sizX());
    CHECK_EQUAL( 1, thePackEngine.bins()[1]->sizY());
    CHECK_EQUAL(30, thePackEngine.bins()[2]->locX());
    CHECK_EQUAL(49, thePackEngine.bins()[2]->locY());
    CHECK_EQUAL(20, thePackEngine.bins()[2]->sizX());
    CHECK_EQUAL( 1, thePackEngine.bins()[2]->sizY());
    CHECK_EQUAL(30, thePackEngine.bins()[3]->locX());
    CHECK_EQUAL(50, thePackEngine.bins()[3]->locY());
    CHECK_EQUAL(40, thePackEngine.bins()[3]->sizX());
    CHECK_EQUAL(19, thePackEngine.bins()[3]->sizY());
}

TEST( MergePairs3 )
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    // equal spaces adjacent left/right left lower
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 50, 20, 20 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 30, 51, 20, 20 )));
    MergePairs( thePackEngine );

//    for( pack2::bin_t b : thePackEngine.bins() )
//        std::cout << b->text();

    CHECK_EQUAL( 4, (int)thePackEngine.bins().size() );
    CHECK_EQUAL(50, thePackEngine.bins()[1]->locX());
    CHECK_EQUAL(50, thePackEngine.bins()[1]->locY());
    CHECK_EQUAL(20, thePackEngine.bins()[1]->sizX());
    CHECK_EQUAL( 1, thePackEngine.bins()[1]->sizY());
    CHECK_EQUAL(30, thePackEngine.bins()[2]->locX());
    CHECK_EQUAL(70, thePackEngine.bins()[2]->locY());
    CHECK_EQUAL(20, thePackEngine.bins()[2]->sizX());
    CHECK_EQUAL( 1, thePackEngine.bins()[2]->sizY());
    CHECK_EQUAL(30, thePackEngine.bins()[3]->locX());
    CHECK_EQUAL(51, thePackEngine.bins()[3]->locY());
    CHECK_EQUAL(40, thePackEngine.bins()[3]->sizX());
    CHECK_EQUAL(19, thePackEngine.bins()[3]->sizY());
}

TEST( MergePairs4 )
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    // equal spaces adjacent left/right even
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 50, 20, 20 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 30, 20, 20 )));
    MergePairs( thePackEngine );

    for( pack2::bin_t b : thePackEngine.bins() )
        std::cout << b->text();

    CHECK_EQUAL( 2, (int)thePackEngine.bins().size() );
    CHECK_EQUAL(50, thePackEngine.bins()[1]->locX());
    CHECK_EQUAL(30, thePackEngine.bins()[1]->locY());
    CHECK_EQUAL(20, thePackEngine.bins()[1]->sizX());
    CHECK_EQUAL(40, thePackEngine.bins()[1]->sizY());
}

TEST( MergePairs5 )
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    // equal spaces adjacent left/right top to left
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 50, 20, 20 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 49, 30, 20, 20 )));
    MergePairs( thePackEngine );

//    for( pack2::bin_t b : thePackEngine.bins() )
//        std::cout << b->text();

    CHECK_EQUAL( 4, (int)thePackEngine.bins().size() );
    CHECK_EQUAL(69, thePackEngine.bins()[1]->locX());
    CHECK_EQUAL(50, thePackEngine.bins()[1]->locY());
    CHECK_EQUAL(1, thePackEngine.bins()[1]->sizX());
    CHECK_EQUAL(20, thePackEngine.bins()[1]->sizY());
    CHECK_EQUAL(49, thePackEngine.bins()[2]->locX());
    CHECK_EQUAL(30, thePackEngine.bins()[2]->locY());
    CHECK_EQUAL(1, thePackEngine.bins()[2]->sizX());
    CHECK_EQUAL( 20, thePackEngine.bins()[2]->sizY());
    CHECK_EQUAL(50, thePackEngine.bins()[3]->locX());
    CHECK_EQUAL(30, thePackEngine.bins()[3]->locY());
    CHECK_EQUAL(19, thePackEngine.bins()[3]->sizX());
    CHECK_EQUAL(40, thePackEngine.bins()[3]->sizY());
}

int main()
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    raven::set::UnitTest::RunAllTests();

    thePackEngine.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    b->add( pack2::item_t( new pack2::cItem("i1",0,0)));
    pack2::bin_t space = pack2::bin_t( new pack2::cBin( b, 80, 80, 20, 20 ));
    thePackEngine.add( space );
    space = pack2::bin_t( new pack2::cBin( b, 95, 60, 5, 80 ));
    thePackEngine.add( space );
    MergeUnusedOnRight( thePackEngine );
    for( pack2::bin_t b : thePackEngine.bins() )
        std::cout << b->text();

    thePackEngine.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    space = pack2::bin_t( new pack2::cBin( "", 20, 20 ));
    space->locate( 80, 80 );
    space->parent( b );
    thePackEngine.add( space );
    space = pack2::bin_t( new pack2::cBin( b, 80, 60, 20, 20 ));
    thePackEngine.add( space );
    MergeUnusedFromBottomRight( thePackEngine, b );
    for( pack2::bin_t b : thePackEngine.bins() )
        std::cout << b->text();


    thePackEngine.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    space = pack2::bin_t( new pack2::cBin( "", 20, 20 ));
    space->locate( 80, 80 );
    space->parent( b );
    thePackEngine.add( space );
    space = pack2::bin_t( new pack2::cBin( "", 10, 20 ));
    space->locate( 70, 70 );
    space->parent( b );
    thePackEngine.add( space );
    space = pack2::bin_t( new pack2::cBin( "", 20, 10 ));
    space->locate( 60, 90 );
    space->parent( b );
    thePackEngine.add( space );
    MergeUnusedFromBottomRight( thePackEngine, b );
    for( pack2::bin_t b : thePackEngine.bins() )
        std::cout << b->text();
    if( thePackEngine.bins().size() != 3 )
    {
        std::cout << "Failed 10\n";
        return 1;
    }
    if( thePackEngine.bins()[0]->size() != 100 )
    {
        std::cout << "Failed 11\n";
        return 1;
    }
    if( thePackEngine.bins()[1]->size() != 100 )
    {
        std::cout << "Failed 12\n";
        return 1;
    }
    if( thePackEngine.bins()[2]->size() != 600 )
    {
        std::cout << "Failed 13\n";
        return 1;
    }

    thePackEngine.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    space = pack2::bin_t( new pack2::cBin( "", 20, 20 ));
    space->locate( 80, 80 );
    space->parent( b );
    thePackEngine.add( space );
    space = pack2::bin_t( new pack2::cBin( "", 10, 10 ));
    space->locate( 70, 80 );
    space->parent( b );
    thePackEngine.add( space );
    space = pack2::bin_t( new pack2::cBin( "", 20, 10 ));
    space->locate( 60, 90 );
    space->parent( b );
    thePackEngine.add( space );
    MergeUnusedFromBottomRight( thePackEngine, b );
    for( pack2::bin_t b : thePackEngine.bins() )
        std::cout << b->text();
    if( thePackEngine.bins().size() != 2 )
    {
        std::cout << "Failed 2-1\n";
        return 1;
    }
    if( thePackEngine.bins()[0]->size() != 100 )
    {
        std::cout << "Failed 2-2\n";
        return 1;
    }
    if( thePackEngine.bins()[1]->size() != 600 )
    {
        std::cout << "Failed 2-3\n";
        return 1;
    }

    thePackEngine.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    b->copyEnable();
    thePackEngine.add( b );
    thePackEngine.addItem( "Item50by40", 50, 40 );
    thePackEngine.addItem( "Item60by20", 60, 20 );
    Pack( thePackEngine );

    thePackEngine.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    thePackEngine.addItem( "Item90by80", 90, 80 );
    thePackEngine.addItem( "Item80by20", 80, 20 );
    thePackEngine.addItem( "Item5by100", 5, 100 );
    Pack( thePackEngine );
    if( BinCount( thePackEngine ) != 1 )
    {
        std::cout << "Failed 7\n";
        return 1;
    }
    cout << CSV( thePackEngine );


    thePackEngine.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    b->copyEnable();
    thePackEngine.add( b );
    thePackEngine.addItem( "Item1", 10, 10 );
    thePackEngine.addItem( "Item2", 10, 10 );
    thePackEngine.addItem( "Item3", 10, 10 );
    thePackEngine.addItem( "Item4", 10, 10 );
    Pack( thePackEngine );
    if( BinCount( thePackEngine ) != 1 )
    {
        std::cout << "Failed 1\n";
        return 1;
    }
    cout << CSV( thePackEngine );

    thePackEngine.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    b->copyEnable();
    thePackEngine.add( b );
    thePackEngine.addItem( "Item1", 10, 10 );
    thePackEngine.addItem( "Item2", 100, 100 );
    Pack( thePackEngine );
    if( BinCount( thePackEngine ) != 2 )
    {
        std::cout << "Failed 2\n";
        return 1;
    }
    cout << CSV( thePackEngine );

    int p = 0;
    for( pack2::bin_t b : thePackEngine.bins() )
    {
        p++;
        switch( p )
        {
        case 1:
            if( b->contents().size() != 1 )
            {
                std::cout << "Failed 3\n";
                return false;
            }
            if( b->contents()[0]->userID() != "Item2" )
            {
                std::cout << "Failed 4\n";
                return false;
            }
            break;
        case 2:
            if( b->contents().size() != 1 )
            {
                std::cout << "Failed 5\n";
                return false;
            }
            if( b->contents()[0]->userID() != "Item1" )
            {
                std::cout << "Failed 4\n";
                return false;
            }
            break;
        }
    }

    thePackEngine.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 20, 100 ));
    thePackEngine.add( b );
    thePackEngine.addItem( "Item1", 100, 10 );
    thePackEngine.addItem( "Item2", 100, 10 );
    Pack( thePackEngine );
    cout << CSV( thePackEngine );
    if( BinCount( thePackEngine ) != 0 )
    {
        std::cout << "Failed 5\n";
        return false;
    }

    thePackEngine.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 20, 100 ));
    thePackEngine.add( b );
    auto item = pack2::item_t( new pack2::cItem( "Item1", 100, 10));
    item->spinEnable();
    thePackEngine.add( item );
    item = pack2::item_t( new pack2::cItem( "Item2", 100, 10));
    item->spinEnable();
    thePackEngine.add( item );
    Pack( thePackEngine );
    cout << CSV( thePackEngine );
    if( BinCount( thePackEngine ) != 1 )
    {
        std::cout << "Failed 6\n";
        return false;
    }

    cout << "Tests passed\n";
    return 0;
}
