#include "tidecompare.h"
#include <QApplication>
#include<QMetaEnum>
#include<QDebug>
#include<QObject>
#include<Eigen>
using namespace Eigen;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//Eigen::MatrixXd f(26,8500),f1(8500,26),k(8500,26),j(26,26);
//j=f1.transpose()*k;


    tideCompare w;
    w.show();


    return a.exec();
}
