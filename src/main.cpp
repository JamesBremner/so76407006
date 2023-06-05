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
    std::vector<double> myVert;
    std::vector<double> myHorz;
    std::vector<std::pair<int, int>> myForbidden;

    void setVert(const std::vector<double> &v);

    void setHorz(const std::vector<double> &h);

    void setForbid(const std::vector<std::pair<int, int>> &f)
    {
        myForbidden = f;
    }
    void draw(wex::shapes &S);
};

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Starter",
              {50, 50, 1000, 500})
    {
        myGrid.setVert({0.5, 1.3, 0.9});
        myGrid.setHorz({2.75, 1, 2});
        myGrid.setForbid({{1,1}});

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
    double l = 1;
    for (double hv : h)
    {
        myHorz.push_back(l);
        l += hv;
    }
    myHorz.push_back(l);
}

void cGrid::setVert(const std::vector<double> &v)
{
    double l = 1;
    for (double vv : v)
    {
        myVert.push_back(l);
        l += vv;
    }
    myVert.push_back(l);
}

void cGrid::draw(wex::shapes &S)
{
    double scale = 50;
    for (auto h : myHorz)
    {
        S.line({0, (int)(scale * h), 500, (int)(scale * h)});
    }
    for (auto v : myVert)
    {
        S.line({(int)(scale * v), 0, (int)(scale * v), 500});
    }
    for (auto &f : myForbidden)
    {
        int l = scale * myVert[f.first-1];
        int t = scale * myHorz[f.second-1];
        int r = scale * myVert[f.first];
        int b = scale * myHorz[f.second];
        S.line( { l, t, r, b } );
        S.line( { r, t, l, b});
    }
}

main()
{
    cGUI theGUI;
    return 0;
}
