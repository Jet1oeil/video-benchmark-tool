#include <QApplication>

#include "Controller/QBenchmarkConfigController.h"

#include "View/QMainView.h"

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);

	// Views
	view::QMainView mainView;
	mainView.show();

	// Controllers
	ctrl::QBenchmarkConfigController benchmarkConfigController(mainView);

	return application.exec();
}
