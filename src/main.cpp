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
    typedef std::vector<double> rect_ltrb_t;
    std::vector<double> myVert;
    std::vector<double> myHorz;
    std::vector<std::pair<int, int>> myForbidden;
    std::pair<int, int> myStartCell;
    std::pair<double, double> myMaxCombSize;
    std::vector<std::vector<std::pair<int, int>>> myListComb;

    void setVert(const std::vector<double> &v);

    void setHorz(const std::vector<double> &h);

    void setForbid(const std::vector<std::pair<int, int>> &f)
    {
        myForbidden = f;
    }
    void setStartCell(const std::pair<int, int> &s)
    {
        myStartCell = s;
    }
    void setMaxCombSize(const std::pair<double, double> &sz)
    {
        myMaxCombSize = sz;
    }

    void EnumerateCombinations();

    void draw(wex::shapes &S);

    /// @brief true if in completely inside out
    /// @param in
    /// @param out
    /// @return
    static bool isInside(
        rect_ltrb_t in,
        rect_ltrb_t out);

    bool isForbidden(
        const std::pair<int, int> &cell);

    static void drawRectangle(
        wex::shapes &S,
        double scale,
        const rect_ltrb_t &r);

private:
    rect_ltrb_t rectDim(const std::pair<int, int> &locationIndices);
};

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Starter",
              {50, 50, 1000, 500})
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
            });

        show();
        run();
    }

private:
    cGrid myGrid;
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

void cGrid::draw(wex::shapes &S)
{
    double scale = 40;
    for (auto h : myHorz)
    {
        S.line({0, (int)(scale * h), (int)(scale * myVert.back()), (int)(scale * h)});
    }
    for (auto v : myVert)
    {
        S.line({(int)(scale * v), 0, (int)(scale * v), (int)(scale * myHorz.back())});
    }

    for (auto &f : myForbidden)
    {
        auto fDim = rectDim(f);

        S.line({(int)(scale * fDim[0]), (int)(scale * fDim[1]),
                (int)(scale * fDim[2]), (int)(scale * fDim[3])});
        S.line({(int)(scale * fDim[2]), (int)(scale * fDim[1]),
                (int)(scale * fDim[0]), (int)(scale * fDim[3])});
    }

    // The SR rectangle
    auto startCellDim = rectDim(myStartCell);
    S.text("S",
           {(int)(scale * startCellDim[0] + 5),
            (int)(scale * startCellDim[1] + 5)});
    // std::pair<double, double> startCellCenter;
    // startCellCenter.first = 0.5 * (startCellDim[0] + startCellDim[2]);
    // startCellCenter.second = 0.5 * (startCellDim[1] + startCellDim[3]);
    // rect_ltrb_t SR(4);
    // SR[0] = startCellCenter.first - myMaxCombSize.first;
    // SR[1] = startCellCenter.second - myMaxCombSize.second;
    // SR[2] = SR[0] + 2 * myMaxCombSize.first;
    // SR[3] = SR[1] + 2 * myMaxCombSize.second;
    // S.color(0x0000FF);
    // drawRectangle(S, scale, SR);

    S.color(0xFF0000);
    int icomb = 0;
    for (auto &c : myListComb[icomb])
    {
        drawRectangle(S, scale, rectDim(c));
    }
}

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
