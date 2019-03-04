#include "vtk8.1.2.h"
#include "vtkRenderWindow.h"
#include "vtkSmartPointer.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCylinderSource.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLight.h>
#include <vtkCamera.h>
#include <vtkActor2D.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkSTLReader.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkDelaunay3D.h>
#include <vtkVersion.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkRendererCollection.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkTransform.h>
#include <vtkLinearTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkObjectFactory.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkOBBTree.h>
#include <vtkTree.h>
#include <vtkPolyDataWriter.h>
#include <vtkLine.h>
#include <vtkIdList.h>
#include <vtkCell.h>
#include <vtkCellType.h>
#include <vtkDiskSource.h>
#include <vtkLineSource.h>
#include <vtkPolygon.h>
#include <vtkTriangle.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkCallbackCommand.h>
#include <vtkSeedRepresentation.h>
#include <vtkSeedWidget.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkPlaybackWidget.h>
#include <vtkSetGet.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include "vtkInteractionStyleModule.h" // For export macro
#include "vtkInteractorStyle.h"
#include <vtkJPEGReader.h>
#include <vtkPlaneSource.h>
#include <vtkBMPReader.h>
#include <vtkTexturedActor2D.h>
#include <vtkDelaunay2D.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTransformTextureCoords.h>
#include <vtkQuadraticQuad.h>
#include <vtkQuadraticPolygon.h>
#include <vtkAxesActor.h>
#include "vtkCaptionActor2D.h"
#include "vtkProperty2D.h"
#include <vtkDoubleArray.h>