#include "cellCombiner.h"

cGUI::cGUI()
    : cStarterGUI(
          "Starter",
          {50, 50, 1000, 500}),
      nextComboButton(wex::maker::make<wex::button>(fm)),
      lbCombo(wex::maker::make<wex::label>(fm))
{
    myGrid.ConstructTest1();

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

main()
{
    cGUI theGUI;
    return 0;
}
