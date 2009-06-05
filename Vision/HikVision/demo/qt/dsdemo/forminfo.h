
#ifndef FORMINFO_H
#define FORMINFO_H

#include <qvariant.h>
#include <qwidget.h>

class   QVBoxLayout;
class   QHBoxLayout;
class   QGridLayout;
class   QLabel;
class   QPushButton;

class FormInfo : public QWidget
{
    Q_OBJECT

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
    FormInfo(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~               FormInfo();

    QLabel*         textLabel;
    QPushButton*    pushButton;

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
protected:
    protected slots : virtual void languageChange();
};
#endif /* FORMINFO_H */
