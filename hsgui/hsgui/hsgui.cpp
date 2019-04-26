#include <ui.h>

int on_closing(uiWindow *w, void *) {
    uiQuit();
    return 1;
}

int main() {
    uiInitOptions opts{};
    uiInit(&opts);

    uiWindow *w;
    w = uiNewWindow("Headsurgeon", 320, 240, false);
    uiWindowSetMargined(w, true);

    uiButton *slice_btn = uiNewButton("Slice");
    uiButton *join_btn = uiNewButton("Join");

    uiBox *hbox = uiNewHorizontalBox();
    uiBoxSetPadded(hbox, true);
    uiBoxAppend(hbox, uiControl(slice_btn), true);
    uiBoxAppend(hbox, uiControl(join_btn), true);

    uiBox *vbox = uiNewVerticalBox();
    uiBoxAppend(vbox, uiControl(hbox), false);
    uiWindowSetChild(w, uiControl(vbox));

    uiWindowOnClosing(w, on_closing, nullptr);
    uiControlShow(uiControl(w));
    uiMain();
}
