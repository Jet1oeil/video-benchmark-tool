#include <QApplication>

#include "View/QMainView.h"

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	view::QMainView mainView;
	mainView.show();

	return application.exec();
}
