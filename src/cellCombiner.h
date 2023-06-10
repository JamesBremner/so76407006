#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include <propertyGrid.h>
#include "cStarterGUI.h"

class cGrid
{
public:
    void ConstructTest1();

    void readFile( const std::string& fname );

    /// @brief set spaciong of vertical grid lines
    /// @param v vector of distances between vertical grid lines
    void setVert(const std::vector<double> &v);

    /// @brief set spacing of horizontal grid lines
    /// @param v vector of distances between horizontal grid lines
    void setHorz(const std::vector<double> &h);

    /// @brief set forbidden cells
    /// @param f vector of col, row zero-based indices locating the forbidden cells
    void setForbid(const std::vector<std::pair<int, int>> &f);

    /// @brief set start cell
    /// @param s col,row sero-based indices of start cell
    void setStartCell(const std::pair<int, int> &s)
    {
        myStartCell = s;
    }

    /// @brief set maximum size of cell combination
    /// @param sz width,height
    void setMaxCombSize(const std::pair<double, double> &sz)
    {
        myMaxCombSize = sz;
    }

    /// @brief enumerate the valid cell combinations
    void EnumerateCombinations();

    /// @brief  draw the grid
    /// @param S shape doing the drawings
    void draw(wex::shapes &S);

    /// @brief move to next smaller combo
    void incCurCombo();

    /// @brief display current combo
    /// @param lbCombo
    /// @param S
    void drawCombo(
        wex::label &lbCombo,
        wex::shapes &S);

    /// @brief text description of current combo
    /// @return
    std::string textCombo();

private:
    typedef std::vector<double> rect_ltrb_t;              /// left, top, right, bottom of rectangle in user units
    typedef std::pair<int, int> location_t;               /// col, row location indices
    std::vector<double> myVert;                           /// vertical line spacing in user units
    std::vector<double> myHorz;                           /// horizontal line spacing in user units
    std::vector<location_t> myForbidden;                  /// indices of forbidden cells
    location_t myStartCell;                               /// indices of start cell
    std::pair<double, double> myMaxCombSize;              /// maximum valid combination size
    std::map<double, std::vector<location_t>> myMapCombo; /// map of valid cell combinations, keyed by total area
    int myCurComb;                                        /// the current cell sombination

    /// @brief left, top, right, bottom of cell in user units from indices
    /// @param locationIndices
    /// @return
    rect_ltrb_t rectDim(const location_t &locationIndices) const;

    /// @brief topleft, bottomright cell indices completely inside rectangle in user indices
    /// @param rect 
    /// @return 
    std::pair<location_t,location_t>
    rectIndices( const  rect_ltrb_t& rect) const;

    /// @brief area of a cell
    /// @param locationIndices
    /// @return
    double area(const location_t &locationIndices);

    /// @brief true if cell is forbidden
    /// @param cell
    /// @return
    bool isForbidden(
        const std::pair<int, int> &cell) const;

    /// @brief True if combination is connected
    /// @param comb combination
    /// @param E enclosing rectangle, topright, bottomleft cell indices
    /// @return 
    bool isConnected( 
        const std::vector<location_t>& comb,
        const std::pair<location_t,location_t>& E ) const;

    /// @brief true if in completely inside out
    /// @param in
    /// @param out
    /// @return
    static bool isInside(
        rect_ltrb_t in,
        rect_ltrb_t out);

    /// @brief draw a rectangle
    /// @param S
    /// @param scale
    /// @param r
    static void drawRectangle(
        wex::shapes &S,
        double scale,
        const rect_ltrb_t &r);

    /// @brief get valid neighbours to right and below inside enclosing rectangle
    /// @param cell find neighbours of this cell
    /// @param leftTop of enclosing rectangle
    /// @param rightBottom of enclosing rectangle
    /// @return vector of neighbour indices
    std::vector<location_t>
    getOrthoNeighboursRightBelow(
        const location_t& cell,
        const location_t& leftTop,
        const location_t& rightBottom ) const;

    /// @brief count of cell moving along rows
    /// @param loc cell locations row, col
    /// @return count of cell
    int getCellCount( const location_t& loc ) const
    {
        return 
        loc.second* myVert.size() + loc.first;
    }

    static double scale()
    {
        return 40;
    }
};

class cGUI : public cStarterGUI
{
public:
    cGUI();

private:
    cGrid myGrid;
    wex::button& bnEnumerate;
    wex::button &nextComboButton;
    wex::label &lbCombo;
    wex::propertyGrid& myPG;
    wex::button& bnRead;
    wex::editbox& ebFile;
};
