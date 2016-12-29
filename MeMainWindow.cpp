#include <QtWidgets\qvboxlayout>
#include <QtWidgets\qhboxlayout>
#include <QtWidgets\qpushbutton.h>
#include "MeMainWindow.h"

MeMainWindow::MeMainWindow()
{
	QHBoxLayout *mainLayout;
	setLayout(mainLayout = new QHBoxLayout);
	mainLayout->addWidget(new MeGlWindow);
	QPushButton *myButton = new QPushButton("Quit", this);
	mainLayout->addWidget(myButton);
	connect(myButton, SIGNAL(clicked()), this, SLOT(close()));
}