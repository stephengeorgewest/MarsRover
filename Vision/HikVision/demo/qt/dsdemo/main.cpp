#include <qapplication.h>
#include "dsform.h"

QApplication* app = NULL;

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */

int main(int argc, char ** argv)
{
    app = new QApplication(argc, argv);
    dsForm          w;
    w.setCaption("Demo for 4008HC ");
    app->setMainWidget(&w);
    w.show();
    app->connect(app, SIGNAL(lastWindowClosed()), app, SLOT(quit()));
    return app->exec();
}
