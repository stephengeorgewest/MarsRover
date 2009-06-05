#include <qapplication.h>
#include "mdform.h"

QApplication* app = NULL;


/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */

int main(int argc, char ** argv)
{
    app = new QApplication(argc, argv);
    mdForm          w;
    w.show();
    app->connect(app, SIGNAL(lastWindowClosed()), app, SLOT(quit()));
    return app->exec();
}
