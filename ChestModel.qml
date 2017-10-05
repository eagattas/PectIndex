import QtQuick 2.6
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0


Entity {
    id: root
    property string scanSource
    property color modelColor

    // Render from the mainCamera
    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                id: renderer
                camera: mainCamera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    Camera {
        id: mainCamera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 22.5
        position: Qt.vector3d( 0.19195, 0.180172, 2.01201 )
        viewCenter: Qt.vector3d(7.04184, 8.03178, -99.4669)
    }

    OrbitCameraController {
        camera: mainCamera
    }


    Entity {
        id: chestObject

        property Material material: PhongMaterial {
            ambient: root.modelColor
            diffuse: Qt.rgba( 0.6, 0.6, 0.6, 1.0 )
        }

        components: [ mesh, chestObject.material ]

        Mesh {
            id: mesh
            source: root.scanSource
        }
    }
}


