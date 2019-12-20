#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>

using namespace std;

#include "pack2.h"

int main()
{
    pack2::cPackEngine thePackEngine;
    pack2::bin_t b;

    thePackEngine.clear();
    b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
    pack2::bin_t space = pack2::bin_t( new pack2::cBin( "", 20, 20 ));
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
            if( b->contents().size() != 1 ) {
                std::cout << "Failed 3\n";
                return false;
            }
            if( b->contents()[0]->userID() != "Item2" ) {
                std::cout << "Failed 4\n";
                return false;
            }
            break;
        case 2:
              if( b->contents().size() != 1 ) {
                std::cout << "Failed 5\n";
                return false;
            }
            if( b->contents()[0]->userID() != "Item1" ) {
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
