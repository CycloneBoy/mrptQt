#include "mrptOne.h"

#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QAction>

mrptOne::mrptOne()
{
    QLabel* label = new QLabel( this );
    label->setText( "Hello World!" );
    setCentralWidget( label );
    QAction* action = new QAction(this);
    action->setText( "Quit" );
    connect(action, SIGNAL(triggered()), SLOT(close()) );
    menuBar()->addMenu( "File" )->addAction( action );
}

mrptOne::~mrptOne()
{}

//#include "mrptOne.moc"
