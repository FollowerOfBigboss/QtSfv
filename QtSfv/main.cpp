#include <iostream>
#include <QApplication>
#include "appwindow.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	QtSfvWindow window;
	window.show();
	return app.exec();
}