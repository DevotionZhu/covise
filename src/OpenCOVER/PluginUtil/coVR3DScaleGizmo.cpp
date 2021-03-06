#include "coVR3DScaleGizmo.h"
#include <OpenVRUI/osg/mathUtils.h>
#include <osg/MatrixTransform>
#include <cover/coVRNavigationManager.h>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <cover/VRSceneGraph.h>
#include <cover/coVRConfig.h>
#include <cover/coVRPluginSupport.h>
#include <osg/io_utils>
#include <vrbclient/SharedState.h>

const float ArrowLength = 5.0f;

using namespace opencover;

coVR3DScaleGizmo::coVR3DScaleGizmo(osg::Matrix m, float s, coInteraction::InteractionType type, const char *iconName, const char *interactorName, coInteraction::InteractionPriority priority = Medium)
    :coVRIntersectionInteractor(s, type, iconName, interactorName, priority, true)
{
    if (cover->debugLevel(2))
    {
        fprintf(stderr, "new coVR3DScaleGizmo(%s)\n", interactorName);
    }

    createGeometry();

    coVR3DScaleGizmo::updateTransform(m);
}

coVR3DScaleGizmo::~coVR3DScaleGizmo()
{
    if (cover->debugLevel(2))
        fprintf(stderr, "\ndelete ~coVR3DScaleGizmo\n");
}

void
coVR3DScaleGizmo::createGeometry()
{
   if (cover->debugLevel(4))
        fprintf(stderr, "\ncoVR3DScaleGizmo::createGeometry\n");

    bool restrictedInteraction = true; 

    osg::ShapeDrawable *sphereDrawable, *xCylDrawable, *yCylDrawable, *zCylDrawable, *xSphereDrawable, *ySphereDrawable, *zSphereDrawable;

    osg::Vec3 origin(0, 0, 0), px(1, 0, 0), py(0, 1, 0), pz(0, 0, 1);
    osg::Vec3 yaxis(0, 1, 0);
    osg::Vec4 red(0.5, 0.2, 0.2, 1.0), green(0.2, 0.5, 0.2, 1.0), blue(0.2, 0.2, 0.5, 1.0), color(0.5, 0.5, 0.5, 1);
    
    axisTransform = new osg::MatrixTransform();
    axisTransform->setStateSet(VRSceneGraph::instance()->loadDefaultGeostate(osg::Material::AMBIENT_AND_DIFFUSE));
    geometryNode = axisTransform.get();
    scaleTransform->addChild(geometryNode.get());

    osg::Sphere *mySphere = new osg::Sphere(origin, 0.5);
    osg::TessellationHints *hint = new osg::TessellationHints();
    hint->setDetailRatio(0.5);
    sphereDrawable = new osg::ShapeDrawable(mySphere, hint);
    sphereDrawable->setColor(color);
    sphereGeode = new osg::Geode();
    sphereGeode->addDrawable(sphereDrawable);
    axisTransform->addChild(sphereGeode.get());

    //create axis
    auto cyl = new osg::Cylinder(origin, 0.15, ArrowLength);
    xCylDrawable = new osg::ShapeDrawable(cyl, hint);
    yCylDrawable = new osg::ShapeDrawable(cyl, hint);
    zCylDrawable = new osg::ShapeDrawable(cyl, hint);

    xCylDrawable->setColor(red);
    yCylDrawable->setColor(green);
    zCylDrawable->setColor(blue);

    xAxisTransform = new osg::MatrixTransform();
    xAxisTransform->setMatrix(osg::Matrix::rotate(osg::inDegrees(90.), 0, 1, 0)*osg::Matrix::translate(osg::Vec3(ArrowLength, 0, 0)*0.5));
    yAxisTransform = new osg::MatrixTransform();
    yAxisTransform->setMatrix(osg::Matrix::rotate(osg::inDegrees(-90.), 1, 0, 0)*osg::Matrix::translate(osg::Vec3(0, ArrowLength, 0)*0.5));
    zAxisTransform = new osg::MatrixTransform();
    zAxisTransform->setMatrix(osg::Matrix::translate(osg::Vec3(0, 0, ArrowLength)*0.5));

    scaleXaxisGeode = new osg::Geode;
    scaleXaxisGeode->addDrawable(xCylDrawable);
    xAxisTransform->addChild(scaleXaxisGeode);
    scaleYaxisGeode = new osg::Geode;
    scaleYaxisGeode->addDrawable(yCylDrawable);
    yAxisTransform->addChild(scaleYaxisGeode);
    scaleZaxisGeode = new osg::Geode;
    scaleZaxisGeode->addDrawable(zCylDrawable);
    zAxisTransform->addChild(scaleZaxisGeode);

    axisTransform->addChild(xAxisTransform);
    axisTransform->addChild(yAxisTransform);
    axisTransform->addChild(zAxisTransform);
    

    osg::Sphere *smallSphere = new osg::Sphere(origin, 0.5);
    xSphereDrawable = new osg::ShapeDrawable(smallSphere, hint);
    ySphereDrawable = new osg::ShapeDrawable(smallSphere, hint);
    zSphereDrawable = new osg::ShapeDrawable(smallSphere, hint);

    xSphereDrawable->setColor(red);
    ySphereDrawable->setColor(green);
    zSphereDrawable->setColor(blue);

    xSphereTransform = new osg::MatrixTransform();
    xSphereTransform->setMatrix(osg::Matrix::translate(osg::Vec3(ArrowLength, 0, 0)));
    ySphereTransform = new osg::MatrixTransform();
    ySphereTransform->setMatrix(osg::Matrix::translate(osg::Vec3(0, ArrowLength, 0)));
    zSphereTransform = new osg::MatrixTransform();
    zSphereTransform->setMatrix(osg::Matrix::translate(osg::Vec3(0, 0, ArrowLength)));

    scaleXSphereGeode = new osg::Geode;
    scaleXSphereGeode->addDrawable(xSphereDrawable);
    xSphereTransform->addChild(scaleXSphereGeode);
    scaleYSphereGeode = new osg::Geode;
    scaleYSphereGeode->addDrawable(ySphereDrawable);
    ySphereTransform->addChild(scaleYSphereGeode);
    scaleZSphereGeode = new osg::Geode;
    scaleZSphereGeode->addDrawable(zSphereDrawable);
    zSphereTransform->addChild(scaleZSphereGeode);

    axisTransform->addChild(xSphereTransform);
    axisTransform->addChild(ySphereTransform);
    axisTransform->addChild(zSphereTransform);

    //create temporary axis which show scale when interactor is active
    tempxAxisTransform = new osg::MatrixTransform();
    tempyAxisTransform = new osg::MatrixTransform();
    tempzAxisTransform = new osg::MatrixTransform();
    
    axisTransform->addChild(tempxAxisTransform);
    axisTransform->addChild(tempyAxisTransform);
    axisTransform->addChild(tempzAxisTransform);
}

//wozu brauche ich das ?
void coVR3DScaleGizmo::updateSharedState()
{
    if (auto st = static_cast<SharedMatrix *>(m_sharedState.get()))
    {
        *st = _oldInteractorXformMat_o;
    }
}
void coVR3DScaleGizmo::startInteraction()
{
    if (cover->debugLevel(5))
        fprintf(stderr, "\ncoVR3DScaleGizmo::startInteraction\n");
    
    osg::Vec3 origin(0, 0, 0);
    osg::Vec3 yaxis(0, 1, 0);
    osg::Matrix o_to_w = cover->getBaseMat();
    osg::Matrix w_to_o = cover->getInvBaseMat();
    osg::Matrix currHandMat = getPointerMat();
    osg::Matrix currHandMat_o = currHandMat * w_to_o;

    osg::Vec3 lp1_o = origin * currHandMat_o;
    osg::Vec3 lp2_o = yaxis * currHandMat_o; 
    osg::Vec3 pointerDir_o = lp2_o - lp1_o;
    pointerDir_o.normalize();   
    // get hand pos in object coords
    osg::Vec3 currHandPos_o = currHandMat_o.getTrans();  
    _startInterPos = currHandPos_o + pointerDir_o * _distance;// + _diff; //here don't use diff

    
    _oldHandMat = currHandMat;
    _invOldHandMat_o.invert(currHandMat_o); // store the inv hand matrix
    _oldInteractorXformMat_o = _interMat_o;
    osg::Matrix interMat = _interMat_o * o_to_w;
    osg::Vec3 interPos = getMatrix().getTrans();

    // get diff between intersection point and sphere center
    _diff = interPos - _hitPos;
    _distance = (_hitPos - currHandMat_o.getTrans()).length(); 
    osg::Vec3 hitPos_o = _hitPos* w_to_o;

    // std::cout<<"diff: "<<_diff.x()<< " " << _diff.y() << "" <<_diff.z() <<std::endl;
    // std::cout<< "distance: "<< _distance<<std::endl;
    // std::cout<<"hitPos"<<_hitPos<< " hitPos_o"<<hitPos_o<<std::endl;
    // std::cout<<"interMat " << interMat.getTrans() <<"interMat_o "<<_interMat_o.getTrans(); 
    //stt::cout <<"own"<<_oldInteractorXformMat_o.getTrans()-

    _scaleXonly =_hitNode == scaleXSphereGeode;// (_hitNode == scaleXaxisGeode) | (_hitNode == scaleXSphereGeode); // OR operation
    _scaleYonly =_hitNode == scaleYSphereGeode;// (_hitNode == scaleYaxisGeode) | (_hitNode == scaleYSphereGeode);
    _scaleZonly =_hitNode == scaleZSphereGeode;// (_hitNode == scaleZaxisGeode) | (_hitNode == scaleZSphereGeode);
    _scaleAll =_hitNode == sphereGeode;

    //create Cylinders which show the scale
    osg::ShapeDrawable *tempCylDrawable;
    auto cyl = new osg::Cylinder(osg::Vec3(0,0,0), 0.3, ArrowLength); 
    osg::TessellationHints *hint = new osg::TessellationHints();
    hint->setDetailRatio(0.5);
    tempCylDrawable = new osg::ShapeDrawable(cyl, hint);    
    tempCylDrawable->setColor(osg::Vec4(0.7,0.5,0.5,1));

    osg::Geode* tmpGeode = new osg::Geode();
    tmpGeode->addDrawable(tempCylDrawable);

    
    if(_scaleXonly)
    {
        tempxAxisTransform->setMatrix(osg::Matrix::rotate(osg::inDegrees(90.), 0, 1, 0)*osg::Matrix::translate(osg::Vec3(ArrowLength, 0, 0)*0.5));
        tempxAxisTransform->addChild(tmpGeode);
        _startxAxisMatrix = tempxAxisTransform->getMatrix();
    }
    else if(_scaleYonly)
    {
        tempyAxisTransform->setMatrix(osg::Matrix::rotate(osg::inDegrees(-90.), 1, 0, 0)*osg::Matrix::translate(osg::Vec3(0, ArrowLength, 0)*0.5));
        tempyAxisTransform->addChild(tmpGeode);
        _startyAxisMatrix = tempyAxisTransform->getMatrix();
    }
    else if(_scaleZonly)
    {
        tempzAxisTransform->setMatrix(osg::Matrix::translate(osg::Vec3(0, 0, ArrowLength)*0.5));;
        tempzAxisTransform->addChild(tmpGeode);
        _startzAxisMatrix = tempzAxisTransform->getMatrix();
    }
    else if(_scaleAll) // allow scale in all directions
    {
        tempxAxisTransform->setMatrix(osg::Matrix::rotate(osg::inDegrees(90.), 0, 1, 0)*osg::Matrix::translate(osg::Vec3(ArrowLength, 0, 0)*0.5));
        tempyAxisTransform->setMatrix(osg::Matrix::rotate(osg::inDegrees(-90.), 1, 0, 0)*osg::Matrix::translate(osg::Vec3(0, ArrowLength, 0)*0.5));
        tempzAxisTransform->setMatrix(osg::Matrix::translate(osg::Vec3(0, 0, ArrowLength)*0.5));

        tempxAxisTransform->addChild(tmpGeode);
        tempyAxisTransform->addChild(tmpGeode);
        tempzAxisTransform->addChild(tmpGeode);
        _startxAxisMatrix = tempxAxisTransform->getMatrix();
        _startyAxisMatrix = tempyAxisTransform->getMatrix(); 
        _startzAxisMatrix = tempzAxisTransform->getMatrix();
    }
   
    /* wie setze ich das hier um, brauch man das ? ###################################
    if (!_rotateOnly && !_translateOnly)
    {
        _translateOnly = is2D();
    }
    */

    coVRIntersectionInteractor::startInteraction();
}
void coVR3DScaleGizmo::doInteraction()
{
    if (cover->debugLevel(5))
        fprintf(stderr, "\ncoVR3DScaleGizmo::move\n");
    
    osg::Vec3 origin(0, 0, 0);
    osg::Vec3 yaxis(0, 1, 0);
    osg::Matrix currHandMat = getPointerMat();

    // forbid translation in y-direction if traverseInteractors is on ############## wozu brauche ich das ? 
    if (coVRNavigationManager::instance()->getMode() == coVRNavigationManager::TraverseInteractors && coVRConfig::instance()->useWiiNavigationVisenso())
    {
        osg::Vec3 trans = currHandMat.getTrans();
        trans[1] = _oldHandMat.getTrans()[1];
        currHandMat.setTrans(trans);
    }

    osg::Matrix o_to_w = cover->getBaseMat();
    osg::Matrix w_to_o = cover->getInvBaseMat();
    osg::Matrix currHandMat_o = currHandMat * w_to_o;
   
   // translate from interactor to hand and back
    osg::Matrix transToHand_o, revTransToHand_o;
    transToHand_o.makeTranslate(currHandMat_o.getTrans() - _oldInteractorXformMat_o.getTrans());
    revTransToHand_o.makeTranslate(_oldInteractorXformMat_o.getTrans() - currHandMat_o.getTrans());
    osg::Matrix relHandMoveMat_o = _invOldHandMat_o * currHandMat_o;
    osg::Matrix interactorXformMat_o = _oldInteractorXformMat_o;

    osg::Vec3 lp1_o = origin * currHandMat_o;
    osg::Vec3 lp2_o = yaxis * currHandMat_o; 
    osg::Vec3 pointerDir_o = lp2_o - lp1_o;
    pointerDir_o.normalize();   
    // get hand pos in object coords
    osg::Vec3 currHandPos_o = currHandMat_o.getTrans();  
    osg::Vec3 interPos = currHandPos_o + pointerDir_o * _distance;// + _diff;
    float sizingFactor = 0.08;
    if(_scaleXonly)
    {
        //float scaleFactor = std::abs(interPos.x() / _startInterPos.x());
        float scaleFactor = 1 + (interPos.x() - _startInterPos.x())*sizingFactor / getScale();

        tempxAxisTransform->setMatrix(_startxAxisMatrix * osg::Matrix::scale(scaleFactor,1,1));  
        // std::cout<< "startInterPos"<<_startInterPos<<std::endl;
        // std::cout << "x scale Factor: "<< scaleFactor << std::endl;
       // std::cout <<"HandMat diff" << _startHandMat_o.getTrans().x() - currHandMat_o.getTrans().x()<<std::endl; // Unterschied Hand und Pointer Mat
    }
    else if(_scaleYonly)
    {
        //float scaleFactor = std::abs(interPos.y() / _startInterPos.y());
        float scaleFactor = 1 + (interPos.y() - _startInterPos.y())*sizingFactor / getScale();

        tempyAxisTransform->setMatrix(_startyAxisMatrix * osg::Matrix::scale(1, scaleFactor, 1));
        //std::cout << "y scale Factor: "<< scaleFactor << std::endl;

    }
    else if(_scaleZonly)
    {
        //float scaleFactor = std::abs(interPos.z() / _startInterPos.z());
        float scaleFactor = 1 + (interPos.z() - _startInterPos.z())*sizingFactor / getScale();

        tempzAxisTransform->setMatrix(_startzAxisMatrix * osg::Matrix::scale(1, 1, scaleFactor));
       // std::cout<< "interPos z" << interPos.z() <<std::endl;
       // std::cout<< "Start interPos z" << _startInterPos.z() <<std::endl;
       // std::cout << "z scale Factor: "<< scaleFactor << std::endl;

    }
    else if(_scaleAll)// allow scale in all directions
    {
        // float scaleFactorX = (interPos.x() - _startInterPos.x())*sizingFactor+1;
        // float scaleFactorY = (interPos.y() - _startInterPos.y())*sizingFactor+1;
        // float scaleFactorZ = (interPos.z() - _startInterPos.z())*sizingFactor+1;
        float distance = (interPos - _startInterPos).length();
        float scale = distance / getScale();// + 1;
       // std::cout <<"distance" <<distance<<std::endl;
        tempxAxisTransform->setMatrix(_startxAxisMatrix * osg::Matrix::scale(scale, 1, 1));
        tempyAxisTransform->setMatrix(_startyAxisMatrix * osg::Matrix::scale(1, scale, 1));
        tempzAxisTransform->setMatrix(_startzAxisMatrix * osg::Matrix::scale(1, 1, scale));

    }
    // save old transformation
    //_oldInteractorXformMat_o = interactorXformMat_o;

    _oldHandMat = currHandMat; 
    _invOldHandMat_o.invert(currHandMat_o);

    if (cover->restrictOn())
    {
        // restrict to visible scene
        osg::Vec3 pos_o, restrictedPos_o;
        pos_o = interactorXformMat_o.getTrans();
        restrictedPos_o = restrictToVisibleScene(pos_o);
        interactorXformMat_o.setTrans(restrictedPos_o);
    }

    if (coVRNavigationManager::instance()->isSnapping())
    {
        if (coVRNavigationManager::instance()->isDegreeSnapping())
        {
            // snap orientation
            snapToDegrees(coVRNavigationManager::instance()->snappingDegrees(), &interactorXformMat_o);
        }
        else
        {
            // snap orientation to 45 degree
            snapTo45Degrees(&interactorXformMat_o);
        }
    }

    // and now we apply it
   // updateTransform(interactorXformMat_o);
}

void coVR3DScaleGizmo::stopInteraction()
{
    tempxAxisTransform->removeChildren(0,tempxAxisTransform->getNumChildren());
    tempyAxisTransform->removeChildren(0,tempyAxisTransform->getNumChildren());
    tempzAxisTransform->removeChildren(0,tempzAxisTransform->getNumChildren());

    coVRIntersectionInteractor::stopInteraction();

}

void coVR3DScaleGizmo::updateTransform(osg::Matrix m)
{
    if (cover->debugLevel(5))
        fprintf(stderr, "coVR3DScaleGizmo:setMatrix\n");
    _interMat_o = m;
    moveTransform->setMatrix(m);
    if (m_sharedState)
    {
        if (auto st = static_cast<SharedMatrix *>(m_sharedState.get()))
        {
            *st = m;
        }
    }
}
void coVR3DScaleGizmo::setShared(bool shared)
{
    if (shared)
    {
        if (!m_sharedState)
        {
            m_sharedState.reset(new SharedMatrix("interactor." + std::string(_interactorName), _oldInteractorXformMat_o));
            m_sharedState->setUpdateFunction([this]() {
                m_isInitializedThroughSharedState = true;
                osg::Matrix interactorXformMat_o = *static_cast<SharedMatrix *>(m_sharedState.get());
                if (cover->restrictOn())
                {
                    // restrict to visible scene
                    osg::Vec3 pos_o, restrictedPos_o;
                    pos_o = interactorXformMat_o.getTrans();
                    restrictedPos_o = restrictToVisibleScene(pos_o);
                    interactorXformMat_o.setTrans(restrictedPos_o);
                }

                if (coVRNavigationManager::instance()->isSnapping())
                {
                    if (coVRNavigationManager::instance()->isDegreeSnapping())
                    {
                        // snap orientation
                        snapToDegrees(coVRNavigationManager::instance()->snappingDegrees(), &interactorXformMat_o);
                    }
                    else
                    {
                        // snap orientation to 45 degree
                        snapTo45Degrees(&interactorXformMat_o);
                    }
                }
                updateTransform(interactorXformMat_o);
            });
        }
    }
    else
    {
        m_sharedState.reset(nullptr);
    }
}

