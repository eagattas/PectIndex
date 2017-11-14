import QtQuick 2.6
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0


Entity {
    id: root
    property string scanSource
    property color modelColor
    property Camera mainCamera

    RenderSettings {
        id: renderSettings
        activeFrameGraph: ForwardRenderer {
            id: renderer
            camera: root.mainCamera
        }
        pickingSettings.pickMethod: PickingSettings.TrianglePicking
        pickingSettings.faceOrientationPickingMode: PickingSettings.FrontAndBackFace
    }

    InputSettings {
        id: inputSettings
    }

    components: [renderSettings, inputSettings]

    Entity {
        id: chestObject

        property Material material: PhongMaterial {
            ambient: root.modelColor
            diffuse: Qt.rgba( 0.6, 0.6, 0.6, 1.0 )
        }

        ObjectPicker {
            id: chestPicker

            onPressed: {
                if(boundsModeButton.checked){
                    if(myProcessor.getFirstClickPressed()){
                        myProcessor.setFirstClickPressed(false)
                        myProcessor.calculateIntersection(myProcessor.getFirstClickLocation())
                        myProcessor.drawLineSegments()
                    }
                    else{
                        myProcessor.setFirstClickLocation(pick.localIntersection.y)
                        myProcessor.setFirstClickPressed(true)
                    }
                }
                else{
                    myProcessor.calculateIntersection(pick.localIntersection.y)
                    myProcessor.drawLineSegments()
                }
            }

        }

        Transform {
            id: chestTransform
            rotation: fromEulerAngles( viewerContainer.yRot,
                                       viewerContainer.xRot,
                                       0 )

            translation: Qt.vector3d( -1 *(viewerContainer.xCamera / 100),
                                     viewerContainer.yCamera / 100,
                                     0)

        }

        components: [ mesh, chestObject.material, chestTransform, chestPicker]

        Mesh {
            id: mesh
            source: root.scanSource
        }
    }
}
