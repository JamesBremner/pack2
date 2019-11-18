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
    auto b = pack2::bin_t( new pack2::cBin( "Bin1", 100, 100 ));
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
