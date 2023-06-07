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
    cGUI()
        : cStarterGUI(
              "Starter",
              {50, 50, 1000, 500}),
          nextComboButton(wex::maker::make<wex::button>(fm)),
          lbCombo(wex::maker::make<wex::label>(fm))
    {
        myGrid.setVert({0.5, 1, 1.5, 2, 2.5, 3, 2.5, 2});
        myGrid.setHorz({0.5, 1, 1.5, 2, 2.5, 3, 2.5, 2});
        myGrid.setForbid({{5, 2}});
        myGrid.setStartCell({4, 4});
        myGrid.setMaxCombSize({6, 7});

        myGrid.EnumerateCombinations();

        fm.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                myGrid.draw(S);
                myGrid.drawCombo(lbCombo, S);
            });
        nextComboButton.move(700, 50, 100, 30);
        nextComboButton.text("Next Combo");
        nextComboButton.events().click(
            [&]
            {
                myGrid.incCurCombo();
                fm.update();
            });
        lbCombo.move(650, 100, 400, 100);
        lbCombo.text("");

        show();
        run();
    }

private:
    cGrid myGrid;
    wex::button &nextComboButton;
    wex::label &lbCombo;
};

void cGrid::setHorz(const std::vector<double> &h)
{
    double l = 0;
    for (double hv : h)
    {
        myHorz.push_back(l);
        l += hv;
    }
    myHorz.push_back(l);
}

void cGrid::setVert(const std::vector<double> &v)
{
    double l = 0;
    for (double vv : v)
    {
        myVert.push_back(l);
        l += vv;
    }
    myVert.push_back(l);
}

void cGrid::incCurCombo()
{
    myCurComb--;
    if (myCurComb < 0)
        myCurComb = myMapCombo.size() - 1;
}

void cGrid::draw(wex::shapes &S)
{
    double sc = scale();
    for (auto h : myHorz)
    {
        S.line({0, (int)(sc * h), (int)(sc * myVert.back()), (int)(sc * h)});
    }
    for (auto v : myVert)
    {
        S.line({(int)(sc * v), 0, (int)(sc * v), (int)(sc * myHorz.back())});
    }

    for (auto &f : myForbidden)
    {
        auto fDim = rectDim(f);

        S.line({(int)(sc * fDim[0]), (int)(sc * fDim[1]),
                (int)(sc * fDim[2]), (int)(sc * fDim[3])});
        S.line({(int)(sc * fDim[2]), (int)(sc * fDim[1]),
                (int)(sc * fDim[0]), (int)(sc * fDim[3])});
    }

    auto startCellDim = rectDim(myStartCell);
    S.text("S",
           {(int)(sc * startCellDim[0] + 5),
            (int)(sc * startCellDim[1] + 5)});
}

void cGrid::drawCombo(
    wex::label &lbCombo,
    wex::shapes &S)
{
    if (!myMapCombo.size())
        return;
    lbCombo.text(textCombo());
    S.color(0xFF0000);
    auto it = myMapCombo.cbegin();
    std::advance(it, myCurComb);
    for (auto &loc : it->second)
    {
        drawRectangle(S, scale(), rectDim(loc));
    }
}
std::string cGrid::textCombo()
{
    std::stringstream ss;
    ss << myCurComb+1 << " of " << myMapCombo.size() << "\n";
    auto it = myMapCombo.cbegin();
    std::advance(it, myCurComb);
    ss << "Area: " << it->first << " cells: ";
    for (auto &loc : it->second)
    {
        ss << loc.first << " " << loc.second << ", ";
    }
    return ss.str();
}

void cGrid::EnumerateCombinations()
{
    myMapCombo.clear();
    auto startCellDim = rectDim(myStartCell);

    // loop E over cells
    for (int rowE = 0; rowE < myHorz.size() - 1; rowE++)
    {
        for (int colE = 0; colE < myVert.size() - 1; colE++)
        {
            // create rectangle E of maximum combination size with E cell at top left
            auto Ecell = rectDim(std::make_pair(colE, rowE));
            rect_ltrb_t E{
                Ecell[0], Ecell[1],
                Ecell[0] + myMaxCombSize.first, Ecell[1] + myMaxCombSize.second};

            // check that E includes the start cell
            if (!isInside(startCellDim, E))
                continue;

            std::vector<std::pair<int, int>> comb;

            // loop over cells
            for (int row = 0; row < myHorz.size() - 1; row++)
                for (int col = 0; col < myHorz.size() - 1; col++)
                {
                    // if cell completely inside E and valid
                    auto pcr = std::make_pair(col, row);
                    if (isInside(rectDim(pcr), E) && (!isForbidden(pcr)))
                    {
                        // add to current combination
                        comb.push_back(pcr);
                    }
                }
            // add current combination to map
            double a = 0;
            for (auto &l : comb)
            {
                a += area(l);
            }
            myMapCombo.insert(std::make_pair(a, comb));
            std::cout << "insert " << myMapCombo.size() << "\n";
        }
    }

    myCurComb = myMapCombo.size() - 1;

    std::cout << myMapCombo.size() << " valid cell combinations found\n";
    for (auto &comb : myMapCombo)
    {
        for (auto &loc : comb.second)
        {
            std::cout << loc.first << " " << loc.second << ", ";
        }
        std::cout << "\n";
    }
}

void cGrid::drawRectangle(
    wex::shapes &S,
    double scale,
    const rect_ltrb_t &r)
{
    std::vector<int> pr(4);
    std::transform(r.begin(), r.end(), pr.begin(),
                   [&](double v)
                   {
                       return (int)(scale * v);
                   });
    pr[2] -= pr[0];
    pr[3] -= pr[1];
    S.rectangle(pr);
}

bool cGrid::isInside(
    rect_ltrb_t in,
    rect_ltrb_t out)
{
    return (out[0] <= in[0] && in[0] <= out[2] &&
            out[1] <= in[1] && in[1] <= out[3] &&
            out[0] <= in[2] && in[2] <= out[2] &&
            out[1] <= in[3] && in[3] <= out[3]);
}

bool cGrid::isForbidden(
    const std::pair<int, int> &cell)
{
    for (auto &f : myForbidden)
        if (f == cell)
            return true;
    return false;
}

cGrid::rect_ltrb_t cGrid::rectDim(const std::pair<int, int> &loc)
{
    if (0 > loc.first || loc.first > myVert.size() - 2 ||
        0 > loc.second || loc.second > myHorz.size() - 2)
        throw std::runtime_error(
            "cGrid::rectDim bad location");
    rect_ltrb_t ret(4);
    ret[0] = myVert[loc.first];
    ret[1] = myHorz[loc.second];
    ret[2] = myVert[loc.first + 1];
    ret[3] = myHorz[loc.second + 1];
    return ret;
}

double cGrid::area(const location_t &locationIndices)
{
    auto d = rectDim(locationIndices);
    return (d[2] - d[0]) * (d[3] - d[1]);
}

main()
{
    cGUI theGUI;
    return 0;
}
