/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

//! [0]
#include <QtTest/QtTest>
#include <math.h>
#include <cmath>

/// https://stackoverflow.com/questions/10198985/calculating-the-distance-between-2-latitudes-and-longitudes-that-are-saved-in-a
namespace stackoverflow {
    #define earthRadiusKm 6371.0
    // This function converts decimal degrees to radians
    double deg2rad(double deg) {
      return (deg * M_PI / 180);
    }
    //  This function converts radians to decimal degrees
    double rad2deg(double rad) {
      return (rad * 180 / M_PI);
    }

    /**
     * Returns the distance between two points on the Earth.
     * Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
     * @param lat1d[纬度] Latitude of the first point in degrees
     * @param lon1d[经度] Longitude of the first point in degrees
     * @param lat2d[纬度] Latitude of the second point in degrees
     * @param lon2d[经度] Longitude of the second point in degrees
     * @return The distance between the two points in kilometers
     */
    double distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d) {
      double lat1r, lon1r, lat2r, lon2r, u, v;
      lat1r = deg2rad(lat1d);
      lon1r = deg2rad(lon1d);
      lat2r = deg2rad(lat2d);
      lon2r = deg2rad(lon2d);
      u = sin((lat2r - lat1r)/2);
      v = sin((lon2r - lon1r)/2);
      return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
    }
}


class TestGOEDistance: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase(){
    }
    void cleanupTestCase(){
    }
    void testStackoverflow(){
        qDebug() << QStringLiteral("距离测试开始...");
        double dis = 0;
        double error = 0;

        dis = stackoverflow::distanceEarth(12,12,12,13);
        error = abs(dis - 108.69385143004575);
        QVERIFY(error < 1);

        dis = stackoverflow::distanceEarth(39.92,116.46,31.22,121.48);
        error = abs(dis - 1068.1053332675);
        QVERIFY(error < 2);

        dis = stackoverflow::distanceEarth(45.4968382,123.0932320,45.4188973,122.5623663);
        error = abs(dis - 42.300494319506);
        QVERIFY(error < 1);

        dis = stackoverflow::distanceEarth(42.4607491,124.0545289,44.3518226,123.2012340);
        error = abs(dis - 221.273560511);
        QVERIFY(error < 1);

        dis = stackoverflow::distanceEarth(45.4967693,123.0931545,42.4607491,124.0545289);
        error = abs(dis - 346.21904232326);
        QVERIFY(error < 1);

        qDebug() << QStringLiteral("距离测试完成");
    }
};

QTEST_MAIN(TestGOEDistance)
#include "testgpsdistance.moc"

