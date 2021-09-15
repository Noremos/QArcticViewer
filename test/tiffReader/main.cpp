#include <QDebug>


#include "../../src/core/decoder.h"
#include "../../src/core/tiffreader.h"

//#include <QCoreApplication>

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);

//    return a.exec();
    TiffReader tiffr;
    tiffr.open("D:/Learning/BAR/Moscow/50_59_1_2_2m_v3.0");

    tiffr.setRowsCacheSize(100);
    for (int i = 0; i < tiffr.height(); i++)
    {
        auto ds1 = tiffr.getRow(i);
    }
    exit(0);
}
