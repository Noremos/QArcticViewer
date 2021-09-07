
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

    for (int i = 0; i < tiffr.height(); i++)
    {
        tiffr.MODE = 1;
        rowptr ds1 = tiffr.getRowData(i);
        tiffr.MODE = 2;
        rowptr ds2 = tiffr.getRowData(i);
        int r = memcmp(ds1, ds2, tiffr.widght());
        if (r != 0)
            qDebug() << "line: " << i << " - " << r;
        delete [] ds1;
        delete [] ds2;
    }

    for (int i = 0; i < tiffr.height(); i++)
    {
        tiffr.MODE = 1;
        rowptr ds1 = tiffr.getRowData(i);
        tiffr.MODE = 2;
        rowptr ds2 = tiffr.getRowData(i);
        int r = memcmp(ds1, ds2, tiffr.widght());
        if (r != 0)
            qDebug() << "line: " << i << " - " << r;
        delete [] ds1;
        delete [] ds2;
    }


    exit(0);
}
