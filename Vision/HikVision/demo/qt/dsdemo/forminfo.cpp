

#include "forminfo.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 * Constructs a FormInfo as a child of 'parent', with the name 'name' and widget
 * flags set to 'f'.
 */
FormInfo::FormInfo(QWidget * parent, const char *name, WFlags fl) :
    QWidget(parent, name, fl)
{
    if(!name)
        setName("FormInfo");

    textLabel = new QLabel(this, "textLabel");
    textLabel->setGeometry(QRect(20, 20, 180, 31));

    pushButton = new QPushButton(this, "pushButton");
    pushButton->setGeometry(QRect(50, 70, 110, 29));
    languageChange();
    resize(QSize(212, 114).expandedTo(minimumSizeHint()));
    connect(pushButton, SIGNAL(clicked()), this, SLOT(close()));
}

/*
 =======================================================================================================================
 * Destroys the object and frees any allocated resources
 =======================================================================================================================
 */
FormInfo::~FormInfo()
{
    /* no need to delete child widgets, Qt does it all for us */
}

/*
 =======================================================================================================================
 * Sets the strings of the subwidgets using the current language.
 =======================================================================================================================
 */
void FormInfo::languageChange()
{
    setCaption(tr("Information"));
    textLabel->setText(tr("The Space of disk isn't enough!\n    Begin overwrite files!"));
    pushButton->setText(tr("OK"));
}
