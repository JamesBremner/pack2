#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cRunWatch.h"

/// A one dimenasional packing engine
class cPackEngine1D
{
public:
    /** Add bin to hold items
        @param[in] x capacity of bin
    */
    void AddBin( int x )
    {
        bin = x;
    }
    /** Add item to fit into bin
        @param[in] x wight of item
    */
    void AddItem( int x )
    {
        vItem.push_back( x );
    }
    /** Use 0-1 packing ( each item can be used only one or not at all )

    If not called, unbounded packing will be used,
    where the items can be freely duplicated
     */
    void algo01()
    {
        f01 = true;
    }
    /** Pack items to a total that is closest to bin capacity, possibly above the capacity

    If not called, items will be packed to a total that is closest but less than the bin capacity
    */
    void algoClosest()
    {
        fclosest = true;
    }
    /** read input file specified on command line
    */
    void Input(
        int argc, char * argv[] );

    /// pack using specified algorithm
    void pack();

    /// Unbounded packing
    void packUB();

    /// 0-1 packing
    std::vector< std::vector<int>> pack01();

    // Closest packing
    void packClosest();

    /// Display packed items
    void DisplayItemsPacked();

private:
    int bin;
    std::vector< int > vItem;
    std::vector< std::vector<int> > itemsPacked;
    bool f01;
    bool fclosest;
};

void cPackEngine1D::pack()
{
    raven::set::cRunWatch( "pack" );

    itemsPacked.resize(bin+1);

    packClosest();

    if( ! f01 )
        packUB();
    else
        pack01();
}
void cPackEngine1D::packClosest()
{
    if( !fclosest )
        return;

        bin *= 2;
        fclosest = false;
        itemsPacked.resize(bin+1);
        auto m = pack01();
        bin /= 2;
        fclosest = true;
        int close = 1e9;
        int closest = 0;
        for( int n = 1; n <= bin*2; n++ )
        {
            int x = m[vItem.size()][n];
            if( fabs( x - bin ) < close )
            {
                close = fabs( x - bin );
                closest = n;
            }
        }
        //std::cout << "closest " << closest << " " << m[vItem.size()][closest]  << "\n";

        bin = closest;
        pack01();

}
void cPackEngine1D::packUB()
{

    std::vector<int> P;          // calculated discretization points

    std::vector<int> c( bin+1 );

    for( int i = 0; i < (int)vItem.size(); i++ )
    {
        for( int j = vItem[i]; j <= bin; j++ )
        {
            if( c[ j ] < c[ j - vItem[i] ] + vItem[ i ] )
            {
                c[ j ] = c[ j - vItem[i] ] + vItem[ i ];
                itemsPacked[j] = itemsPacked[ j - vItem[i]];
                itemsPacked[j].push_back( i );
            }
        }
    }
}

std::vector< std::vector<int>>
                            cPackEngine1D::pack01()
{


    std::vector< std::vector<int>> m( vItem.size()+1, std::vector<int>( bin+1 ) );

    for( int i = 1; i <= (int)vItem.size(); i++ )
    {
        for( int j = 0; j <= bin; j++ )
        {
            if( vItem[i] > j )
            {
                m[i][j] = m[i-1][j];
            }
            else
            {
                int t1 = m[i-1][j];
                int t2 = m[i-1][j-vItem[i]] + vItem[i];
                if( t1 > t2 )
                {
                    m[i][j] = t1;
                }
                else
                {
                    m[i][j] = t2;
                    itemsPacked[j] = itemsPacked[j-vItem[i]];
                    itemsPacked[j].push_back( i );
                }
            }
        }
    }
    //std::cout << m[vItem.size()][bin] << "\n";

    return m;

}

void cPackEngine1D::DisplayItemsPacked()
{
    std::cout << "items_packed  ";
    for( int i : itemsPacked[bin] )
        std::cout << vItem[i] << " ";
    std::cout << "\n";
}

void cPackEngine1D::Input(
    int argc, char * argv[] )
{
    if( argc != 2 )
    {
        std::cout << "Usage: pack1d <filename>\n ";
        exit(1);
    }
    std::ifstream f( argv[1] );
    if( ! f.is_open() )
    {
        std::cout << "Cannot open " << argv[1] << "\n";
        exit(1);
    }
    // loop over lines in file
    std::string line;
    while( std::getline( f, line ) )
    {
        //std::cout << line << "\n";
        if( line.find("//") == 0 )
            continue;
        std::vector< std::string > token;
        std::stringstream sst(line);
        std::string a;
        while( getline( sst, a, '\t' ) )
            token.push_back( a );
        if( token.size() < 2 )
            continue;
        if( token[0] == "bin")
        {
            AddBin( atoi( token[1].c_str()));
        }
        if( token[0] == "item")
        {
            AddItem( atoi( token[1].c_str()));
        }
        if( token[0] == "algorithm")
        {
            if( token[1] == "01" )
                algo01();
            if( token[1] == "closest" )
                algoClosest();
        }
    }
}


int main(int argc, char * argv[])
{
    raven::set::cRunWatch::Start();

    cPackEngine1D PE;
    PE.Input( argc, argv );
    PE.pack();
    PE.DisplayItemsPacked();

    raven::set::cRunWatch::Report();

    return 0;
}
