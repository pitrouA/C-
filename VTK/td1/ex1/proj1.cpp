
#include <vtkPolyData.h>
#include <vtkPLYReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSetReader.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>


#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTerrain.h>
#include <vtkInteractorStyleJoystickCamera.h>
#include <vtkInteractorStyleFlight.h>


#include <vtkElevationFilter.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

#include "vtkCamera.h"
#include "vtkProperty.h"

#include "vtkSphereSource.h"

int main(int argc, char **argv)
{
  printf("hhh");

  vtkSphereSource *sphere = vtkSphereSource::New();
    sphere->SetPhiResolution(12); sphere->SetThetaResolution(12);

    vtkDataObject* input =sphere->GetOutput();

  vtkDataSetMapper *mapper = vtkDataSetMapper::New();


     mapper->SetInputConnection(sphere->GetOutputPort());

  vtkActor *actor = vtkActor::New();
    actor->SetMapper(mapper);

vtkRenderer *renderer = vtkRenderer::New();
  renderer->AddActor(actor);
  renderer->SetBackground(0,0,0);

	vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);
	renWin->SetSize(450,450);

  //renWin->Render();
  renWin->Start();

int i;
 for (i = 0; i < 200; ++i)
     {
     // render the image
     renWin->Render();

   }

  // Clean up
  renderer->Delete();
  renWin->Delete();

  sphere->Delete();

  mapper->Delete();
  actor->Delete();
}

//sudo /sbin/ldconfig -v
// cmake -DVTK_DIR:PATH=/home/pitrou/projects/VTK-build ..
