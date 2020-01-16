#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <cutest.h>

using namespace std;

#include "pack2.h"

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
    pack2::cPackEngine E;
    //E.Algorithm().FitSliderRez = 100;
    pack2::bin_t b = pack2::bin_t( new pack2::cBin( "Bin1", 2400,1200 ));
    E.add( b );
    E.add( pack2::bin_t( new pack2::cBin( b, 2000, 1000, 400, 200 )));
    auto packed_item = pack2::item_t( new pack2::cItem( "packed", 400, 200 ));
    E.add( packed_item );
    packed_item->locate( 2000, 1000 );
    b->contents().push_back( packed_item );
    b->pack();
    pack2::item_t item = pack2::item_t( new pack2::cItem( "test", 20, 20 ));
    CHECK( pack2::FitSlider( E, item, b ) );

    //std::cout << E.text();

    CHECK_EQUAL( 1980, item->locX() );
    CHECK_EQUAL( 1180, item->locY() );

  }

TEST( Pack1 )
{
    pack2::cPackEngine E;
    pack2::bin_t b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    b->copyEnable();
    E.add( b );
    E.addItem( "Item50by40", 50, 40 );
    E.addItem( "Item60by20", 60, 20 );

    Pack( E );

//    std::cout << "\nPack1 res1\n";
//    for( auto i : E.items() )
//        std::cout << i->text();
//    for( pack2::bin_t b : E.bins() )
//        std::cout << b->text();
//    std::cout << "=====================\n";

    CHECK_EQUAL( 1, E.bins().size() );
    CHECK( E.items()[0]->isPacked() );
    CHECK( E.items()[1]->isPacked() );
    CHECK_EQUAL( 0, E.items()[0]->locX() );
    CHECK_EQUAL( 0, E.items()[0]->locY() );
    CHECK_EQUAL( 0, E.items()[1]->locX() );
    CHECK_EQUAL( 40, E.items()[1]->locY() );

    E.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin2", 100, 100 ));
    b->copyEnable();
    E.add( b );
    E.addItem( "Item90x90", 90, 90 );
    E.addItem( "Item20x20", 20, 20 );
    Pack( E );

//    std::cout << "\nPack1 res2\n";
//    for( auto i : E.items() )
//        std::cout << i->text();
//    for( pack2::bin_t b : E.bins() )
//        std::cout << b->text();
//    std::cout << "=====================\n";

    CHECK_EQUAL( 2, E.bins().size() );
    CHECK( E.items()[0]->isPacked() );
    CHECK( E.items()[1]->isPacked() );
    CHECK_EQUAL( 0, E.items()[0]->locX() );
    CHECK_EQUAL( 0, E.items()[0]->locY() );
    CHECK_EQUAL( 0, E.items()[1]->locX() );
    CHECK_EQUAL( 0, E.items()[1]->locY() );
}

//TEST( MergeAdjacent )
//{
//    pack2::cPackEngine E;
//    pack2::bin_t b = pack2::bin_t( new pack2::cBin( "Bin1", 2400,1200 ));
//    E.add( b );
//    E.add( pack2::bin_t( new pack2::cBin( b, 10, 0, 20, 20 )));
//    E.add( pack2::bin_t( new pack2::cBin( b, 10, 21, 20, 20 )));
//    pack2::MergeAdjacentPairs( E, b );
//    CHECK_EQUAL( 3, (int)E.bins().size() );
//    CHECK_EQUAL( 10, E.bins()[1]->locX() );
//    CHECK_EQUAL( 0, E.bins()[1]->locY() );
//    CHECK_EQUAL( 20, E.bins()[1]->sizX() );
//    CHECK_EQUAL( 20, E.bins()[1]->sizY() );
//    CHECK_EQUAL( 10, E.bins()[2]->locX() );
//    CHECK_EQUAL( 21, E.bins()[2]->locY() );
//    CHECK_EQUAL( 20, E.bins()[2]->sizX() );
//    CHECK_EQUAL( 20, E.bins()[2]->sizY() );
//
//    E.clear();
//    b = pack2::bin_t( new pack2::cBin( "Bin1", 2400,1200 ));
//    E.add( b );
//    E.add( pack2::bin_t( new pack2::cBin( b, 10, 0, 20, 20 )));
//    E.add( pack2::bin_t( new pack2::cBin( b, 10, 20, 20, 20 )));
//    pack2::MergeAdjacentPairs( E, b );
////    for( pack2::bin_t b : E.bins() )
////        std::cout << b->text();
//    CHECK_EQUAL( 2, (int)E.bins().size() );
//    CHECK_EQUAL( 10, E.bins()[1]->locX() );
//    CHECK_EQUAL( 0, E.bins()[1]->locY() );
//    CHECK_EQUAL( 20, E.bins()[1]->sizX() );
//    CHECK_EQUAL( 40, E.bins()[1]->sizY() );
//
//    E.clear();
//    b = pack2::bin_t( new pack2::cBin( "Bin1", 2400,1200 ));
//    E.add( b );
//    E.add( pack2::bin_t( new pack2::cBin( b, 12, 0, 14, 20 )));
//    E.add( pack2::bin_t( new pack2::cBin( b, 10, 20, 20, 20 )));
//    pack2::MergeAdjacentPairs( E, b );
////    for( pack2::bin_t b : E.bins() )
////        std::cout << b->text();
//    CHECK_EQUAL( 4, (int)E.bins().size() );
//    CHECK_EQUAL( 10, E.bins()[1]->locX() );
//    CHECK_EQUAL( 20, E.bins()[1]->locY() );
//    CHECK_EQUAL( 2, E.bins()[1]->sizX() );
//    CHECK_EQUAL( 20, E.bins()[1]->sizY() );
//    CHECK_EQUAL( 12, E.bins()[2]->locX() );
//    CHECK_EQUAL( 0, E.bins()[2]->locY() );
//    CHECK_EQUAL( 14, E.bins()[2]->sizX() );
//    CHECK_EQUAL( 40, E.bins()[2]->sizY() );
//    CHECK_EQUAL( 26, E.bins()[3]->locX() );
//    CHECK_EQUAL( 20, E.bins()[3]->locY() );
//    CHECK_EQUAL( 4, E.bins()[3]->sizX() );
//    CHECK_EQUAL( 20, E.bins()[3]->sizY() );
//
//}

TEST( adjacent )
{
    int dl, dr;
    pack2::cShape s1( 10, 10, 20, 20 );
    pack2::cShape s2( 10, 100, 20, 20 );
    CHECK( ! s2.isAdjacent( s1, dl, dr ));
    CHECK( ! s1.isAdjacent( s2, dl, dr ));

    pack2::cShape s3( 0, 30, 50, 20 );
    CHECK( ! s1.isAdjacent( s3, dl, dr ));
    CHECK_EQUAL( 1, s3.isAdjacent( s1, dl, dr ));
    CHECK_EQUAL( 10, dl );
    CHECK_EQUAL( -20, dr );

    pack2::cShape s4( 30, 300, 20, 20 );
    CHECK( ! s4.isAdjacent( s1, dl, dr ));
    CHECK( ! s1.isAdjacent( s4, dl, dr ));

    pack2::cShape s5( 30, 20, 20, 20 );
    CHECK( ! s1.isAdjacent( s5, dl, dr ));
    CHECK_EQUAL( 2, s5.isAdjacent( s1, dl, dr ));
    CHECK_EQUAL( -10, dl );
    CHECK_EQUAL( -10, dr );

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

    cout << "Tests passed\n";
    return 0;
}
