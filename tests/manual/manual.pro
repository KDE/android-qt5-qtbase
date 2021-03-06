TEMPLATE=subdirs

SUBDIRS = bearerex \
gestures \
inputmethodhints \
keypadnavigation \
lance \
network_remote_stresstest \
network_stresstest \
qcursor \
qdesktopwidget \
qgraphicsitem \
qgraphicsitemgroup \
qgraphicslayout/flicker \
qhttpnetworkconnection \
qimagereader \
qlocale \
qnetworkaccessmanager/qget \
qnetworkconfigurationmanager \
qnetworkreply \
qssloptions \
qtabletevent \
qtbug-8933 \
qtouchevent \
qwidget_zorder \
repaint \
socketengine \
textrendering \
widgets/itemviews/delegate \
windowflags \
windowgeometry \
windowmodality \
widgetgrab

!contains(QT_CONFIG, openssl):!contains(QT_CONFIG, openssl-linked):SUBDIRS -= qssloptions

# disable some tests on wince because of missing dependencies
wince*:SUBDIRS -= \
    lance windowmodality \
    network_remote_stresstest network_stresstest
