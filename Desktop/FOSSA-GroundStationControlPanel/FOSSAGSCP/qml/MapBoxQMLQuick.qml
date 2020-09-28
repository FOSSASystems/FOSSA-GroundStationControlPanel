import QtQuick 2.12
import QtQuick.Window 2.14
import QtPositioning 5.12
import QtLocation 5.12

Item {

    Map {
        plugin: Plugin {
            name: "osm" // "mapboxgl", "esri", ...

            PluginParameter {
                name: "osm.mapping.providersrepository.address"
                value: "http://maps-redirect.qt.io/osm/5.8/satellite"
            }

            // specify plugin parameters if necessary
            // PluginParameter {
            //     name:
            //     value:
            // }
        }

        anchors.fill: parent
        center: QtPositioning.coordinate(59.91, 10.75) // Oslo
        zoomLevel: 14
    }
}
