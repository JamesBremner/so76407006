#include "cellCombiner.h"
#include "graphTheory.h"

void cGrid::ConstructTest1()
{
    setVert({0.5, 1, 1.5, 2, 2.5, 3, 2.5, 2});
    setHorz({0.5, 1, 1.5, 2, 2.5, 3, 2.5, 2});
    setForbid({{5, 2}});
    setStartCell({4, 4});
    setMaxCombSize({6, 7});

    //    EnumerateCombinations();
}

static std::vector<std::string> parseSSV(
    const std::string &l)
{
    std::vector<std::string> token;
    std::stringstream sst(l);
    std::string a;
    while (getline(sst, a, ' '))
        token.push_back(a);

    return token;
}

void cGrid::readFile(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw(
            "Cannot open input file");
    myForbidden.clear();
    std::string line;
    while (getline(ifs, line))
    {
        auto vtoken = parseSSV(line);
        std::vector<double> vspace;
        switch (vtoken[0][0])
        {

        case 'h':
        case 'H':
            for (int k = 1; k < vtoken.size(); k++)
            {
                vspace.push_back(atof(vtoken[k].c_str()));
            }
            setHorz(vspace);
            break;

        case 'v':
        case 'V':
            for (int k = 1; k < vtoken.size(); k++)
            {
                vspace.push_back(atof(vtoken[k].c_str()));
            }
            setVert(vspace);
            break;

        case 'f':
        case 'F':
            setForbid({std::make_pair(
                atoi(vtoken[1].c_str()),
                atoi(vtoken[2].c_str()))});
            break;
        }
    }
}

void cGrid::setHorz(const std::vector<double> &h)
{
    myHorz.clear();
    double l = 0;
    for (double hv : h)
    {
        myHorz.push_back(l);
        l += hv;
    }
    myHorz.push_back(l);
}
void cGrid::setForbid(const std::vector<std::pair<int, int>> &f)
{
    myForbidden.insert(
        myForbidden.end(),
        f.begin(), f.end());
}

void cGrid::setVert(const std::vector<double> &v)
{
    myVert.clear();
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

    try
    {
        auto startCellDim = rectDim(myStartCell);
        S.text("S",
               {(int)(sc * startCellDim[0] + 5),
                (int)(sc * startCellDim[1] + 5)});
    }
    catch (...)
    {
    }
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
    ss << myCurComb + 1 << " of " << myMapCombo.size() << "\n";
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
    if (isForbidden(myStartCell))
        return;
    rect_ltrb_t startCellDim;
    try
    {
        startCellDim = rectDim(myStartCell);
    }
    catch (...)
    {
        return;
    }

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
            {
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
            }

            // check that combination is connected TID2
            if (!isConnected(comb, rectIndices(E)))
                continue;

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
    const std::pair<int, int> &cell) const
{
    for (auto &f : myForbidden)
        if (f == cell)
            return true;
    return false;
}

bool cGrid::isConnected(
    const std::vector<location_t> &comb,
    const std::pair<location_t, location_t> &E) const
{
    // loop over cells in combo, linking them to valid neighbours to right and below
    // no links to left and above because links are bidirectional
    raven::graph::cGraph g;
    for (auto &dst : comb)
    {
        int dstCount = getCellCount(dst);
        for (auto indexNeighbor : getOrthoNeighboursRightBelow(
                 dst,
                 E.first, E.second))
        {
            g.add(
                dstCount,
                getCellCount(indexNeighbor));
        }
    }

    // check that every cell is reachable from start cell
    int startCount = getCellCount(myStartCell);
    for (auto &dst : comb)
    {
        if (!path(
                 g,
                 startCount,
                 getCellCount(dst))
                 .first.size())
            return false;
    }

    return true;
}

cGrid::rect_ltrb_t cGrid::rectDim(const std::pair<int, int> &loc) const
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

std::pair<cGrid::location_t, cGrid::location_t>
cGrid::rectIndices(const rect_ltrb_t &rect) const
{
    std::pair<cGrid::location_t, cGrid::location_t> ret;
    ret.first.first = INT_MAX;
    ret.first.second = INT_MAX;
    ret.second.first = 0;
    ret.second.first = 0;
    for (int row = 0; row < myHorz.size() - 1; row++)
    {
        for (int col = 0; col < myVert.size() - 1; col++)
        {
            if (isInside(rectDim(std::make_pair(col, row)), rect))
            {
                if (col < ret.first.first)
                    ret.first.first = col;
                if (row < ret.first.second)
                    ret.first.second = row;
                if (col > ret.second.first)
                    ret.second.first = col;
                if (row > ret.second.second)
                    ret.second.second = row;
            }
        }
    }
    return ret;
}

double cGrid::area(const location_t &locationIndices)
{
    auto d = rectDim(locationIndices);
    return (d[2] - d[0]) * (d[3] - d[1]);
}

std::vector<cGrid::location_t>
cGrid::getOrthoNeighboursRightBelow(
    const location_t &cell,
    const location_t &leftTop,
    const location_t &rightBottom) const
{
    std::vector<location_t> ret;
    location_t n;
    n.first = cell.first + 1;
    n.second = cell.second;
    if (!isForbidden(n))
    {
        if (n.first >= leftTop.first &&
            n.second >= leftTop.second &&
            n.first <= rightBottom.first &&
            n.second <= rightBottom.second)
        {
            ret.push_back(n);
        }
    }
    n.first = cell.first;
    n.second = cell.second + 1;
    if (!isForbidden(n))
    {
        if (n.first >= leftTop.first &&
            n.second >= leftTop.second &&
            n.first <= rightBottom.first &&
            n.second <= rightBottom.second)
        {
            ret.push_back(n);
        }
    }
    n.first = cell.first + 1;
    n.second = cell.second + 1;
    if (isForbidden(n))
    {
        if (n.first >= leftTop.first &&
            n.second >= leftTop.second &&
            n.first <= rightBottom.first &&
            n.second <= rightBottom.second)
        {
            ret.push_back(n);
        }
    }
    return ret;
}
