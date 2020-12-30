import Qt3D.Core 2.15
import Qt3D.Render 2.15
import Qt3D.Extras 2.15
import Qt3D.Input 2.15

//OrbitCameraController {
FirstPersonCameraController {
    property alias baseCamer: camera
    //    property alias hits: screenRayCaster.hits
    function setPos(pos) {
        scrn.position = pos
    }
    linearSpeed: 300

    camera: Camera {

        //    id: mainCamera
        //    projectionType: CameraLens.PerspectiveProjection
        //    fieldOfView: 22.5
        //    //    aspectRatio: _window.width / _window.height
        //    nearPlane: 0.01
        //    farPlane: 1000.0
        //    viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
        //    upVector: Qt.vector3d(0.0, 1.0, 0.0)
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 60
        nearPlane: 0.1
        farPlane: 10000.0
        position: Qt.vector3d(0.0, 500.0, 0.0)
//        upVector: Qt.vector3d(0.0, 1.0, 0.0)

        //        viewCenter: Qt.vector3d(10.0, 0.0, 10.0)
        //    acceleration: 0.1
        //    deceleration: 1.0
    }

//    RayCaster {
//        id: raycaster
//        origin: camera.position
//        direction: camera.upVector
//        length: 5000

//        //        onHitsChanged: printHits("Model hits", hits)
//    }
}
