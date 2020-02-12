#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include "wex.h"

using namespace std;

int main( int argc, char* argv[] )
{
    if( argc != 2 )
    {
        cout << "Usage: cutlistView <cutlist filename>\n";
        exit(1);
    }
    ifstream inf( argv[1] );
    if( ! inf.is_open() )
    {
        cout << "Cannot open " << argv[1] << "\n";
        exit(1);
    }
    string line;
    getline( inf, line );
    vector< vector< int > > cuts;
    while( getline( inf, line ) )
    {
        cout << line << "\n";
        char* p = (char*)line.c_str() + line.find(",") + 1;
        vector< int > v;
        for( int k = 0; k < 4; k++ )
        {
            v.push_back( strtod( p, &p ) );
            p += 1;
        }
        cuts.push_back( v );
    }

    double maxX, maxY;
    for( auto v : cuts )
    {
        for( double x : v )
            cout << x << " ";
        cout << "\n";
        if( v[0] > maxX )
            maxX = v[0];
        if( v[2] > maxX )
            maxX = v[2];
        if( v[1] > maxY )
            maxY = v[1];
        if( v[3] > maxY )
            maxY = v[3];
    }
    cout << maxX <<" "<< maxY << "\n";

    int scale = 2;
    vector< vector< int > > scaledcuts = cuts;
    for( auto& v : scaledcuts )
        for( int& x : v )
            x /= scale;

    int maxCut = 1;

    wex::gui& form = wex::maker::make();
    form.move( 50,50,maxX,maxY+100);
    form.text("Cutlist Viewer");
    wex::label& lb = wex::maker::make<wex::label>( form );
    lb.move( {300, maxY+20, 200, 30 } );

    form.events().draw([&]( PAINTSTRUCT& ps )
    {
        wex::shapes S( ps );
        int k = 0;
        for( auto v : scaledcuts )
        {
            if( k == maxCut-1 ) {
                S.penThick(3);
                S.color(255,0,0);
            }

            S.line( v );

//            for( double x : cuts[k] )
//                cout << x << " ";
//            cout << "\n";

            stringstream ss;
            ss << "cut " << k+1 <<": "<< cuts[k][0] << ", "
                << cuts[k][1] << " to "
                << cuts[k][2] << ", "<< cuts[k][3];
            lb.text( ss.str() );

            k++;
            if( k >= maxCut )
                break;
        }
        cout << "\n";
    });
    wex::button& btn = wex::maker::make<wex::button>( form );
    btn.move( {20, maxY+20, 100, 30 } );
    btn.text( "Next Cut" );
    btn.events().click([&]
    {
        maxCut++;
        if( maxCut == cuts.size()+1 )
            maxCut--;
        form.update();
    });
    wex::button& btn2 = wex::maker::make<wex::button>( form );
    btn2.move( {150, maxY+20, 100, 30 } );
    btn2.text( "Remove Cut" );
    btn2.events().click([&]
    {
        maxCut--;
        if( maxCut < 0 )
            maxCut++;
        form.update();
    });


    form.show();
    form.run();

    return 0;

}
