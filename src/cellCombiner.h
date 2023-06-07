#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

class cGrid
{
public:
    void ConstructTest1();

    /// @brief set spaciong of vertical grid lines
    /// @param v vector of distances between vertical grid lines
    void setVert(const std::vector<double> &v);

    /// @brief set spacing of horizontal grid lines
    /// @param v vector of distances between horizontal grid lines
    void setHorz(const std::vector<double> &h);

    /// @brief set forbidden cells
    /// @param f vector of col, row zero-based indices locating the forbidden cells
    void setForbid(const std::vector<std::pair<int, int>> &f)
    {
        myForbidden = f;
    }
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
    typedef std::vector<double> rect_ltrb_t;              /// left, top, right, bottom of rectangle
    typedef std::pair<int, int> location_t;               /// col, row location indices
    std::vector<double> myVert;                           /// vertical line spacing
    std::vector<double> myHorz;                           /// horizontal line spacing
    std::vector<std::pair<int, int>> myForbidden;         /// indices of forbidden cells
    std::pair<int, int> myStartCell;                      /// indices of start cell
    std::pair<double, double> myMaxCombSize;              /// maximum valid combination size
    std::map<double, std::vector<location_t>> myMapCombo; /// map of valid cell combinations, keyed by total area
    int myCurComb;                                        /// the current cell sombination

    /// @brief left, top, right, bottom of cell
    /// @param locationIndices
    /// @return
    rect_ltrb_t rectDim(const location_t &locationIndices);

    /// @brief area of a cell
    /// @param locationIndices
    /// @return
    double area(const location_t &locationIndices);

    /// @brief true if cell is forbidden
    /// @param cell
    /// @return
    bool isForbidden(
        const std::pair<int, int> &cell);

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
    wex::button &nextComboButton;
    wex::label &lbCombo;
};
