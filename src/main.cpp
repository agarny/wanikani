/*******************************************************************************

Copyright Alan Garny

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

//==============================================================================
// Main
//==============================================================================

#include "dialog.h"

//==============================================================================

#include <QFileInfo>

//==============================================================================

#include <QtSingleApplication>

//==============================================================================

int main(int pArgC, char *pArgV[])
{
    // Create our application and make sure that it's not already running

    QtSingleApplication app(QFileInfo(pArgV[0]).baseName(), pArgC, pArgV);

    if (app.isRunning())
        return 0;

    // Customise our application

    app.setApplicationName("WaniKani");
    app.setOrganizationName("Hellix");
    app.setQuitOnLastWindowClosed(false);

    // Create and (initially) hide our dialog

    Dialog dialog;

    dialog.hide();

    return app.exec();
}

//==============================================================================
// End of file
//==============================================================================
