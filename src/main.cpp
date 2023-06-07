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

    void incCurCombo();
    void drawCombo(
        wex::label &lbCombo,
        wex::shapes &S);

private:
    typedef std::vector<double> rect_ltrb_t;                  /// left, top, right, bottom of rectangle
    std::vector<double> myVert;                               /// vertical line spacing
    std::vector<double> myHorz;                               /// horizontal line spacing
    std::vector<std::pair<int, int>> myForbidden;             /// indices of forbidden cells
    std::pair<int, int> myStartCell;                          /// indices of start cell
    std::pair<double, double> myMaxCombSize;                  /// maximum valid combination size
    std::vector<std::vector<std::pair<int, int>>> myListComb; /// list of valid combinations
    int myCurComb;

    /// @brief left, top, right, bottom of cell
    /// @param locationIndices
    /// @return
    rect_ltrb_t rectDim(const std::pair<int, int> &locationIndices);

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
        lbCombo.move(650, 100, 200, 100);
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
    myCurComb++;
    if (myCurComb >= myListComb.size())
        myCurComb = 0;
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
    lbCombo.text(std::to_string(myCurComb + 1) + " of " + std::to_string(myListComb.size()));
    S.color(0xFF0000);
    for (auto &c : myListComb[myCurComb])
    {
        drawRectangle(S, scale(), rectDim(c));
    }
}

std::string textCombo();

void cGrid::EnumerateCombinations()
{
    auto startCellDim = rectDim(myStartCell);

    // loop E over cells
    for (int rowE = 0; rowE < myHorz.size(); rowE++)
    {
        for (int colE = 0; colE < myVert.size(); colE++)
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
            for (int row = 0; row < myHorz.size(); row++)
                for (int col = 0; col < myHorz.size(); col++)
                {
                    // if cell completely inside E and valid
                    auto cell = rectDim(std::make_pair(col, row));
                    if (isInside(cell, E) && (!isForbidden(std::make_pair(col, row))))
                    {
                        // add to current combination
                        comb.push_back(std::make_pair(col, row));
                    }
                }
            // add current combination to list
            myListComb.push_back(comb);
        }
    }
    myCurComb = 0;
    std::cout << myListComb.size() << " valid cell combinations found\n";
    for (auto &comb : myListComb)
    {
        for (auto &loc : comb)
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

cGrid::rect_ltrb_t cGrid::rectDim(const std::pair<int, int> &locationIndices)
{
    rect_ltrb_t ret(4);
    ret[0] = myVert[locationIndices.first];
    ret[1] = myHorz[locationIndices.second];
    ret[2] = myVert[locationIndices.first + 1];
    ret[3] = myHorz[locationIndices.second + 1];
    return ret;
}

main()
{
    cGUI theGUI;
    return 0;
}
