#include "cellCombiner.h"

cGUI::cGUI()
    : cStarterGUI(
          "Starter",
          {50, 50, 1000, 700}),
      nextComboButton(wex::maker::make<wex::button>(fm)),
      lbCombo(wex::maker::make<wex::label>(fm)),
      myPG(wex::maker::make<wex::propertyGrid>(fm)),
      bnEnumerate(wex::maker::make<wex::button>(fm))
{
    myGrid.ConstructTest1();

    fm.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            myGrid.draw(S);
            myGrid.drawCombo(lbCombo, S);
        });

    myPG.move({650, 20, 300, 300});
    myPG.labelWidth(200);
    myPG.string("Starter cell ( col,row)", "4,4");
    myPG.string("Max size (width,height)", "6,7");

    bnEnumerate.move(700, 350, 100, 30);
    bnEnumerate.text("Enumerate");
    bnEnumerate.events().click(
        [&]
        {
            auto vs = myPG.value("Starter cell ( col,row)");
            int p = vs.find(",");
            myGrid.setStartCell(std::pair(
                atoi(vs.c_str()),
                atoi(vs.substr(p+1).c_str() )));
            vs = myPG.value("Max size (width,height)");
            p = vs.find(",");
            myGrid.setMaxCombSize(std::pair(
                atoi(vs.c_str()),
                atoi(vs.substr(p+1).c_str() )));
            myGrid.EnumerateCombinations();
            fm.update();
        });

    nextComboButton.move(700, 400, 100, 30);
    nextComboButton.text("Next Combo");
    nextComboButton.events().click(
        [&]
        {
            myGrid.incCurCombo();
            fm.update();
        });
    lbCombo.move(650, 500, 400, 100);
    lbCombo.text("");

    show();
    run();
}

main()
{
    cGUI theGUI;
    return 0;
}
