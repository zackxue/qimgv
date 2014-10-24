#include "opendialog.h"

OpenDialog::OpenDialog() {
}

void OpenDialog::setParent(QWidget* _parent) {
    parent = _parent;
}

QString OpenDialog::getOpenFileName() {
    const QString imagesFilter = tr("Images (*.png *.jpg *jpeg *bmp *gif)");
    return dialog.getOpenFileName(parent,
                                  tr("Open image"),
                                  globalSettings->s.value("lastDir",".").toString(),
                                  imagesFilter,
                                  0);
}
