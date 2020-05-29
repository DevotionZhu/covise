#pragma once

#include <PluginUtil/coVR3DTransRotInteractor.h>
#include <PluginUtil/coVR3DTransInteractor.h>

#include <osg/ShapeDrawable>
#include <osg/Geometry>

using namespace opencover;


class GridPoint;
class SensorPosition;

struct ZoneProperties
{

};
/*   * Vertices of a Zone 
     *    4*    *7
     *
     * 5*----*6
     *  |0*  |  *3
     *  |    |
     * 1*----*2
     * 
     * Coordinate System is centered at 2
     * Size interactor located at 4
     * width = 7-6 (y-direction)
     * length = 5-6 (x-direction)
     * height = 6-2 (z-direction)
*/

class Zone
{
public:
    Zone(osg::Matrix matrix,osg::Vec4 color);
    virtual ~Zone(){std::cout<<"Zone Destructor\n";};
    virtual void createGrid() = 0;

    virtual bool preFrame();
    
    void setPosition(osg::Matrix matrix);
    void setDistance(float distance);

    osg::ref_ptr<osg::MatrixTransform> getZone(){return m_LocalDCS;}
    int getNumberOfPoints()const{return m_GridPoints.size();}

    std::vector<osg::Vec3> getWorldPositionOfPoints();
protected:
    osg::ref_ptr<osg::MatrixTransform> m_LocalDCS;
    std::vector<GridPoint> m_GridPoints;
private:
    float m_Distance{2};
    float m_Length{10};
    float m_Width{5};
    float m_Height{3};
    osg::Vec4 m_Color;
    
    osg::ref_ptr<osg::Vec3Array> m_Verts;
    osg::ref_ptr<osg::Vec4Array> m_Colors;
    osg::ref_ptr<osg::Geometry> m_Geom;
    osg::ref_ptr<osg::Geode> m_Geode;


    std::unique_ptr<coVR3DTransRotInteractor> m_Interactor;
    std::unique_ptr<coVR3DTransInteractor> m_SizeInteractor;
    std::unique_ptr<coVR3DTransInteractor> m_DistanceInteractor;

    osg::Geode* draw();

    virtual void createGridPoints();
    osg::Vec3 calcSign()const;
    osg::Vec3 defineStartPointForInnerGrid()const;
    osg::Vec3 defineLimitsOfInnerGridPoints()const;
    void addPointToVec(osg::Vec3 point);
    void deleteGridPoints();
    void createInner3DGrid(const osg::Vec3& startPoint, const osg::Vec3& sign, const osg::Vec3& limit);
    void createOuter3DGrid(const osg::Vec3& sign );
    void updateGeometry(osg::Vec3& vec);

};

struct SafetyZoneProperties
{
    enum
    {
        PRIO1,
        PRIO2
    };

};

class SafetyZone : public Zone
{

public:
    SafetyZone(osg::Matrix matrix):Zone(matrix,osg::Vec4{1,0.5,0,1}){std::cout<<"Safety Zone created\n";}
    ~SafetyZone(){std::cout<<"SafetyZone Destructor\n";};
    void createGrid()override{};

private:
    osg::Vec4 m_ColorVisible;


};

struct SensorZoneProperties
{

};

class SensorZone : public Zone
{
public:
    SensorZone(osg::Matrix matrix);
    ~SensorZone(){std::cout<<"SensorZone Destructor\n";};
    bool preFrame() override;
    void createGrid() override{};
    std::unique_ptr<SensorPosition> addSensor();

private:
    std::vector<std::unique_ptr<SensorPosition>> m_Sensors;  
    osg::Vec3 getFreeSensorPosition()const;
};


class GridPoint
{
public:
    GridPoint(osg::Vec3 position,osg::Vec4& color);
    osg::ref_ptr<osg::MatrixTransform> m_LocalDCS;

    osg::ref_ptr<osg::MatrixTransform> getPoint()const{return m_LocalDCS.get();} //muss man hier ref_ptr übergeben?
    osg::Vec3 getPosition()const{return m_LocalDCS->getMatrix().getTrans();}

    void setColor(const osg::Vec4& color);
    void setOriginalColor();
private:
    osg::Vec4 m_Color;
    osg::ref_ptr<osg::Geode> m_Geode;
    osg::ref_ptr<osg::Sphere> m_Sphere;
    osg::ref_ptr<osg::ShapeDrawable> m_SphereDrawable;
};