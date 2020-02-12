#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <cutest.h>

using namespace std;

#include "pack2.h"

TEST( CutList )
{
    pack2::cPackEngine E;
    E.add( pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 )) );
    E.addItem( "Item1", 20, 20 );
    E.addItem( "Item2", 20, 20 );
    Pack( E );
    auto L = pack2::CutList( E );
//    for( auto& c : L )
//    {
//        for( int v : c )
//            std::cout << v << ", ";
//        std::cout << "\n";
//    }
    CHECK_EQUAL( 3, L.size() );
    CHECK_EQUAL( 20, L[2][1] );
    CHECK_EQUAL( 0, L[2][2] );
    CHECK_EQUAL( 20, L[2][3] );
    CHECK_EQUAL( 40, L[2][4] );
}

TEST( CutListJoin )
{
    pack2::cPackEngine E;
    E.add( pack2::bin_t( new pack2::cBin( "CutListJoin", 1000, 1000 )) );
    E.addItem( "Item1", 500, 200 );
    E.addItem( "Item2", 500, 200 );
    E.addItem( "Item3", 200, 20 );
    E.addItem( "Item4", 600, 20 );
    Pack( E );
    auto L = pack2::CutList( E );
        for( auto& c : L )
    {
        for( int v : c )
            std::cout << v << ", ";
        std::cout << "\n";
    }
    CHECK_EQUAL( 6, L.size() );
}

TEST( spin )
{
    pack2::cPackEngine E;
    auto b = pack2::bin_t( new pack2::cBin( "Bin1", 20, 100 ));
    E.add( b );
    E.addItem( "Item1", 100, 10 );
    E.addItem( "Item2", 100, 10 );
    Pack( E );
    CHECK_EQUAL( 0, BinCount( E ) );

    E.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 20, 100 ));
    E.add( b );
    E.addItem( "Item1", 100, 10 );
    E.addItem( "Item2", 100, 10 );
    E.items()[0]->spinEnable();
    E.items()[1]->spinEnable();
    Pack( E );
    CHECK_EQUAL( 1, BinCount( E ) );
}

TEST( CreateRemainingSpaces )
{
    pack2::cPackEngine E;
    pack2::bin_t b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    E.add( b );
    pack2::item_t item = pack2::item_t( new pack2::cItem("i1",0,0));
    item->sizX( 5 );
    item->sizY( 10 );

    CreateRemainingSpaces( E, b, item );

    CHECK( pack2::cShape(5,0,95,100) == *(pack2::cShape*)Spaces(E,b)[0].get() );
    CHECK( pack2::cShape(0,10,5,90) == *(pack2::cShape*)Spaces(E,b)[1].get() );

}


TEST( StrightThru )
{
    pack2::cPackEngine E;
    E.Algorithm().fThruCuts = true;
    pack2::bin_t b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    b->copyEnable();
    E.add( b );
    E.addItem( "Item50by40", 50, 40 );
    Pack( E );
    CHECK_EQUAL( 1, BinCount( E ));


}
TEST( pack1 )
{
    pack2::cPackEngine E;
    pack2::bin_t b = pack2::bin_t( new pack2::cBin( "Bin2", 100, 100 ));
    b->copyEnable();
    E.add( b );
    E.addItem( "Item50by40", 50, 40 );
    E.addItem( "Item60by20", 60, 20 );
    Pack( E );
    CHECK_EQUAL( 1, BinCount( E ));

    E.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin3", 100, 100 ));
    E.add( b );
    E.addItem( "Item90by80", 90, 80 );
    E.addItem( "Item80by20", 80, 20 );
    E.addItem( "Item5by100", 5, 100 );
    Pack( E );
    CHECK_EQUAL( 1, BinCount( E ));

}

TEST( subtract1 )
{
    pack2::cShape s1( "1",800,750);
    s1.locate( 1600,0 );
    pack2::cShape s2( "2", 1100, 300 );
    s2.locate( 1300, 700 );
    CHECK( s1.isOverlap( s2 ) );
    s1.subtract( s2 );
    CHECK_EQUAL( 1600, s1.locX());
    CHECK_EQUAL( 0, s1.locY());
    CHECK_EQUAL( 800, s1.sizX() );
    CHECK_EQUAL( 700, s1.sizY() );
}
TEST( subtract2 )
{
    pack2::cShape s1( "1",1600,600);
    s1.locate( 0,1000 );
    pack2::cShape s2( "2", 1100, 300 );
    s2.locate( 1300, 700 );
    CHECK( s1.isOverlap( s2 ) );
    s1.subtract( s2 );
    CHECK_EQUAL( 0, s1.locX());
    CHECK_EQUAL( 1000, s1.locY());
    CHECK_EQUAL( 1300, s1.sizX() );
    CHECK_EQUAL( 600, s1.sizY() );
}

TEST( subtract3 )
{
    pack2::cShape s1( "1",200,200);
    s1.locate( 1200,600 );
    pack2::cShape s2( "2", 1100, 300 );
    s2.locate( 1300, 700 );
    CHECK( s1.isOverlap( s2 ) );
    s1.subtract( s2 );
    CHECK_EQUAL( 1200, s1.locX());
    CHECK_EQUAL( 600, s1.locY());
    CHECK_EQUAL( 100, s1.sizX() );
    CHECK_EQUAL( 100, s1.sizY() );
}

TEST( tid11 )
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b = pack2::bin_t( new pack2::cBin( "Bin1", 2400,1200 ));
    b->copyEnable();
    thePackEngine.add( b );
    //thePackEngine.addItem( "621_1", 914,316 );
    thePackEngine.addItem( "621_2", 1100,500 );
    thePackEngine.addItem( "621_3", 1600,500 );
    //thePackEngine.addItem( "621_4", 1600,250 );
    thePackEngine.addItem( "621_5", 1200,250 );
    Pack( thePackEngine );

}

TEST( SortBinsIntoIncreasingSize )
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    pack2::bin_t b2 = pack2::bin_t( new pack2::cBin( b ));
    thePackEngine.add( b2 );

    thePackEngine.add( pack2::bin_t( new pack2::cBin( b2, 50, 50, 3, 3 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 50, 10, 10 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 60, 60, 5, 5 )));

    SortBinsIntoIncreasingSize( thePackEngine );

//    for( pack2::bin_t b : thePackEngine.bins() )
//        std::cout << b->text();

    CHECK_EQUAL( 5, thePackEngine.bins()[0]->sizX() );
    CHECK_EQUAL( 10, thePackEngine.bins()[1]->sizX() );
    CHECK_EQUAL( 3, thePackEngine.bins()[3]->sizX() );

}


/*
merge from BWPCENPLY16_GREY_21091HGL_cpy2  100 3174x5292 at 7484 6908
BWPCENPLY16_GREY_21091HGL_cpy2  99 7484x1824 at 0 10376
to BWPCENPLY16_GREY_21091HGL_cpy2  101 10658x1824 at 0 10376
BWPCENPLY16_GREY_21091HGL_cpy2  100 3174x3468 at 7484 6908
BWPCENPLY16_GREY_21091HGL_cpy2  99 7484x0 at 0 10376
*/
TEST( MergePairs8 )
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;
    b = pack2::bin_t( new pack2::cBin( "Bin1", 24400,12200 ));
    thePackEngine.add( b );
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 7484, 6908, 3174, 5292 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 0, 10376, 7484, 1824 )));
    MergePairs( thePackEngine, b );

//    for( pack2::bin_t b : thePackEngine.bins() )
//        std::cout << b->text();
}

TEST( MergePairs1 )
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    // equal spaces adjacent left/right even
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 50, 20, 20 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 30, 50, 20, 20 )));
    MergePairs( thePackEngine, b );
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
    MergePairs( thePackEngine, b );

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
    MergePairs( thePackEngine, b );

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

    // equal spaces adjacentup/down even
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 50, 20, 20 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 30, 20, 20 )));
    MergePairs( thePackEngine, b );

//    for( pack2::bin_t b : thePackEngine.bins() )
//        std::cout << b->text();

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

    // equal spaces adjacent up/down top to left
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 50, 20, 20 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 49, 30, 20, 20 )));
    MergePairs( thePackEngine, b );

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

TEST( MergePairs6 )
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    // equal spaces adjacent up/down top to right
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 50, 20, 20 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 51, 30, 20, 20 )));
    MergePairs( thePackEngine, b );

//    for( pack2::bin_t b : thePackEngine.bins() )
//        std::cout << b->text();

    CHECK_EQUAL( 4, (int)thePackEngine.bins().size() );
    CHECK_EQUAL(50, thePackEngine.bins()[1]->locX());
    CHECK_EQUAL(50, thePackEngine.bins()[1]->locY());
    CHECK_EQUAL(1, thePackEngine.bins()[1]->sizX());
    CHECK_EQUAL(20, thePackEngine.bins()[1]->sizY());
    CHECK_EQUAL(70, thePackEngine.bins()[2]->locX());
    CHECK_EQUAL(30, thePackEngine.bins()[2]->locY());
    CHECK_EQUAL(1, thePackEngine.bins()[2]->sizX());
    CHECK_EQUAL( 20, thePackEngine.bins()[2]->sizY());
    CHECK_EQUAL(51, thePackEngine.bins()[3]->locX());
    CHECK_EQUAL(30, thePackEngine.bins()[3]->locY());
    CHECK_EQUAL(19, thePackEngine.bins()[3]->sizX());
    CHECK_EQUAL(40, thePackEngine.bins()[3]->sizY());
}

TEST( MergePairs7 )
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    // equal spaces adjacent up/down top to right
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    thePackEngine.add( b );
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 51, 30, 20, 20 )));
    thePackEngine.add( pack2::bin_t( new pack2::cBin( b, 50, 50, 20, 20 )));
    MergePairs( thePackEngine, b );

//    for( pack2::bin_t b : thePackEngine.bins() )
//        std::cout << b->text();
}


int main()
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    raven::set::UnitTest::RunAllTests();


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
    //cout << CSV( thePackEngine );

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
    //cout << CSV( thePackEngine );

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

    //cout << "Tests passed\n";
    return 0;
}
