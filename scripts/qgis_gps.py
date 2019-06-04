#!/usr/bin/env python

# Lib
import numpy as np
import rospy
import urllib
import urllib2
import time

# ROS msgs
from sensor_msgs.msg import NavSatFix


class QGIS_GPS:
    def __init__(self, name):
        self.name = name
        layer_name = 'GPS_fix'

        # QGIS
        self.vectorLyr = QgsVectorLayer('Point?crs=epsg:4326', layer_name, "memory")
        self.vpr = self.vectorLyr.dataProvider()
        self.qmr = QgsMapLayerRegistry.instance()
        self.firstPt = True

        # Subscribers
        rospy.Subscriber('/sensors/gps_buoy_emlid_raw', NavSatFix, self.cb_gps, queue_size=1)


    def gps_callback(self, gps_msg):
        rospy.loginfo('GPS measure received: ' + str(gps_msg.latitude) + ", " + str(gps_msg.longitude))
        lat = gps_msg.latitude
        lon = gps_msg.longitude

        pnt = QgsGeometry.fromPoint(QgsPoint(lon, lat))

        if firstPt:
            self.firstPt = False
            f = QgsFeature()
            f.setGeometry(pnt)
            self.vpr.addFeatures([f])
            self.qmr.addMapLayer(self.vectorLyr)
        else:
            print lon, lat
            self.vectorLyr.startEditing()
            self.vectorLyr.changeGeometry(1, pnt)
            self.vectorLyr.commitChanges()

        self.vectorLyr.setCacheImage(None)
        self.vectorLyr.updateExtents()
        self.vectorLyr.triggerRepaint()
























if __name__ == '__main__':
    try:
        rospy.init_node('qgis_gps')
        QGIS_GPS(rospy.get_name())

        rospy.spin()
    except rospy.ROSInterruptException:
        pass



