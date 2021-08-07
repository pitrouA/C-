
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
#include "vtkConeSource.h"
#include <vtkCommand.h>
#include <vtkBoxWidget.h>
//#include <vtkSmartPointer.h>
//#include <vtkNamedColors.h>

class vtkMyCallback : public vtkCommand{
  public: static vtkMyCallback *New(){ return new vtkMyCallback; }

  virtual void Execute(vtkObject *caller, unsigned long, void*){
    vtkTransform *t = vtkTransform::New();
    vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
    widget->GetTransform(t);
    widget->GetProp3D()->SetUserTransform(t);
    t->Delete();
  }
 };

int main(int argc, char **argv)
{
  //printf("hhh");
  //vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

  //-------objets-------
  vtkConeSource *cone = vtkConeSource::New();
  cone->SetHeight( 3.0 );
  cone->SetRadius( 1.0 );
  cone->SetResolution( 10 );

  vtkDataObject* input =cone->GetOutput();

  //-------mappers-------
  vtkDataSetMapper *mapper = vtkDataSetMapper::New();
  mapper->SetInputConnection(cone->GetOutputPort());

  //-------acteurs-------
  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(0,0,100);
  //actor->GetProperty()->SetColor(colors->GetColor3d("Cornsilk").GetData());

  vtkActor *actor2 = vtkActor::New();
  actor2->SetMapper(mapper);
  actor2->GetProperty()->SetColor(100,0,100);
  actor2->SetPosition(0,0,2.5);

  //-------renderers-------
  vtkRenderer *renderer = vtkRenderer::New();
  renderer->AddActor(actor);
  renderer->AddActor(actor2);
  renderer->SetBackground(0,0,0);
  renderer->GetActiveCamera()->SetPosition(5,10,10);
  renderer->GetActiveCamera()->SetFocalPoint(0,1,0);
  //renderer->GetActiveCamera()->Azimuth(180);

  //-------renderers windows-------
	vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(renderer);
	renWin->SetSize(450,450);
  //renWin->Render();
  renWin->Start();
  /*int i;
   for (i = 0; i < 500; ++i)
       {
       // render the image
       renWin->Render();
       renderer->GetActiveCamera()->Azimuth(2.0);//i*0.1
     }*/

  //-------box widget-------
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  vtkBoxWidget *boxWidget = vtkBoxWidget::New();
  boxWidget->SetInteractor(iren);
  boxWidget->SetPlaceFactor(1.25);
  boxWidget->SetProp3D(actor);
  boxWidget->PlaceWidget();
  vtkMyCallback *callback = vtkMyCallback::New();
  boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);
  boxWidget->On();

  //-------renderers windows interactors-------
  iren->SetRenderWindow(renWin);
  iren->Initialize();
  iren->Start();

  // Clean up
  renderer->Delete();
  renWin->Delete();
  cone->Delete();
  mapper->Delete();
  actor->Delete();
}

//sudo /sbin/ldconfig -v
// cmake -DVTK_DIR:PATH=/home/pitrou/projects/VTK-build ..
