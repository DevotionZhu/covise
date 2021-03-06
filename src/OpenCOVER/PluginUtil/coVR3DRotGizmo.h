#ifndef _CO_VR_3D_ROT_GIZMO
#define _CO_VR_3D_ROT_GIZMO
#include <cover/coVRIntersectionInteractor.h>
#include <vrbclient/SharedStateSerializer.h>
#include <cover/MatrixSerializer.h>
#include <net/tokenbuffer.h>


namespace opencover
{

class PLUGIN_UTILEXPORT coVR3DRotGizmo : public coVRIntersectionInteractor
{
private:
    bool _rotateXonly{false}, _rotateYonly{false}, _rotateZonly{false};
    const float _radius{3};
    float _distance{0.0f};
    osg::Vec3 _diff;
    osg::Matrix _interMat_o, _oldHandMat;
    osg::Matrix _invOldHandMat_o;
    osg::Matrix _oldInteractorXformMat_o;

    enum class RotationAxis { Z = 0, X, Y };

    osg::ref_ptr<osg::MatrixTransform> _axisTransform; // all the Geometry
    osg::ref_ptr<osg::Geode> _sphereGeode;
    osg::ref_ptr<osg::Group> _xRotCylGroup;
    osg::ref_ptr<osg::Group> _zRotCylGroup;
    osg::ref_ptr<osg::Group> _yRotCylGroup;


    osg::Geode* circles( RotationAxis axis, int approx, osg::Vec4 color );                                    // draw circles with osg::DrawArrays (not intersectable)
    osg::Group* circlesFromCylinders( RotationAxis axis, int approx, osg::Vec4 color, float cylinderLength ); // draw circles with cylinders
    osg::Vec3Array* circleVerts(RotationAxis axis, int approx);                                               // calc verts for circles

    osg::Matrix calcRotation(osg::Vec3 rotationAxis, osg::Vec3 cylinderDirectionVector);
    bool rotateAroundSpecificAxis(osg::Group *group)const;


protected:
    void createGeometry() override;
    void updateSharedState() override;
    typedef vrb::SharedState<osg::Matrix> SharedMatrix;

public:
    coVR3DRotGizmo(osg::Matrix m, float s, coInteraction::InteractionType type, const char *iconName, const char *interactorName, coInteraction::InteractionPriority priority);
    
    // delete scene graph
    virtual ~coVR3DRotGizmo();

    // start the interaction (grab pointer, set selected hl, store dcsmat)
    virtual void startInteraction() override;
    virtual void doInteraction() override;
    virtual void preFrame() override;

    virtual void updateTransform(osg::Matrix m);

    //const osg::Matrix &getMatrix() const
    //{
    //    return _interMat_o;
    //}
    void setShared(bool state) override;

};

}
#endif
