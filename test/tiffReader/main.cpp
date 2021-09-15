#include <QDebug>


#include "../../src/core/decoder.h"
#include "../../src/core/tiffreader.h"

//#include <QCoreApplication>

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);

//    return a.exec();
    TiffReader tiffr;
    tiffr.open("/home/art/m/qarctic/QArcticViewer/test/arcticdem_mosaic_1km_v3.0.tif");

    tiffr.setRowsCacheSize(100);
    for (int i = 0; i < tiffr.height(); i++)
    {
        ds1 = tiffr.getRow(i);
    }
    exit(0);
}
