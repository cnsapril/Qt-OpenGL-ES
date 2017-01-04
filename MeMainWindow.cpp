#include <QtWidgets\qvboxlayout>
#include <QtWidgets\qhboxlayout>
#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qsizepolicy.h>
#include "MeMainWindow.h"

MeMainWindow::MeMainWindow()
{
	setWindowTitle("Touch Iphone 7 Plus");
	QHBoxLayout *mainLayout;
	QWidget* centralWidget;
	MeGlWindow* glWindow;
	setCentralWidget(centralWidget = new QWidget);
	centralWidget->setLayout(mainLayout = new QHBoxLayout);
	mainLayout->addWidget(glWindow = new MeGlWindow);
	QSizePolicy sizePolicy;
	sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
	sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
	glWindow->setSizePolicy(sizePolicy);

	QPushButton *myButton = new QPushButton("Quit", this);
	mainLayout->addWidget(myButton);
	connect(myButton, SIGNAL(clicked()), this, SLOT(close()));
}