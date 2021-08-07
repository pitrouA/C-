
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
#include "vtkConeSource.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSetReader.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include <vtkContourFilter.h>
#include <vtkCutter.h>
#include <vtkPlane.h>

vtkContourFilter* FilterValues(vtkDataSetReader *reader){
  vtkContourFilter *cf = vtkContourFilter::New();
  cf->SetNumberOfContours(2);
  cf->SetValue(0,2.4);
  cf->SetValue(1,4.0);
  cf->SetInputConnection(reader->GetOutputPort());

  return cf;
}

vtkCutter* CutValues(vtkDataSetReader *reader, vtkPlane *plane){
  vtkCutter *cutter = vtkCutter::New();

  plane->SetNormal(0,1,1);
  plane->SetOrigin(0,0,0);

  cutter->SetCutFunction(plane);
  cutter->SetInputConnection(reader->GetOutputPort());

  return cutter;
}

void set2Windows(vtkRenderer *ren,vtkRenderer *ren2,vtkRenderWindow *renwin){
  ren->SetViewport(0.5, 0, 1, 1);
  ren2->SetViewport(0, 0, 0.5, 1);
  renwin->AddRenderer(ren2);
}

void setInteractor(vtkRenderWindow *renwin){
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renwin);
  iren->Start();
}

void setAnimator(vtkRenderWindow *renwin, vtkContourFilter *input,vtkPlane *plane, vtkRenderer *ren, vtkActor *actor2){
  //plan deplacement init
  ren->AddActor(actor2);
  input->Update();
  double bounds[6];
  vtkDataSet *polydata=input->GetOutput();
  polydata->GetBounds(bounds);
  std::cout  << "xmin: " << bounds[0] << " "
  << "xmax: " << bounds[1] << std::endl
  << "ymin: " << bounds[2] << " "
  << "ymax: " << bounds[3] << std::endl
  << "zmin: " << bounds[4] << " "
  << "zmax: " << bounds[5] << std::endl;
  double xrange = bounds[1]-bounds[0];
  double yrange = bounds[3]-bounds[2];
  double zrange = bounds[5]-bounds[4];

  //anim boucle
  int nbpas=1800;
  for (int i = 0 ; i < nbpas ; i++){
    //limites
    input->SetValue(0, 1.0 + (i/400.0));
    //plan
    plane->SetOrigin(bounds[0]+i*(xrange)/nbpas,bounds[2]+i*(yrange)/nbpas,bounds[4]+i*(zrange)/nbpas);
    renwin->Render();
  }
}

int main(int argc, char **argv)
{
  vtkContourFilter *input;
  vtkCutter *input2;
  vtkPlane *plane = vtkPlane::New();

  //-------lecture fichier-------
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName("../noise.vtk");

  //-------filtrage--------
  //input = reader;
  input = FilterValues(reader);
  input2 = CutValues(reader,plane);

  //-------mappers-------
  vtkDataSetMapper *mapper = vtkDataSetMapper::New();
  mapper->SetInputConnection(input->GetOutputPort());

  vtkDataSetMapper *mapper2 = vtkDataSetMapper::New();
  mapper2->SetInputConnection(input2->GetOutputPort());

  //-------colors--------
  vtkLookupTable *lut = vtkLookupTable::New();
  //double vals[4] = { 0.75, 0, 0.05, 1 };
  //lut->SetTableValue(0, vals);
  lut->SetNumberOfColors(256);
  for (int i = 0 ; i < 256 ; i++){
    double vals[4] = { (256.0 - i)/256.0, 0, (i / 256.0), 1 };
    lut->SetTableValue(i, vals);
  }
  mapper->SetScalarRange(2.4,6);
  mapper->SetLookupTable(lut);
  mapper2->SetScalarRange(1,6);
  mapper2->SetLookupTable(lut);
  lut->Build();

  //-------acteurs-------
  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);
  vtkActor *actor2 = vtkActor::New();
  actor2->SetMapper(mapper2);

  //-------renderers-------
  vtkRenderer *ren = vtkRenderer::New();
  vtkRenderer *ren2 = vtkRenderer::New();
  ren->AddActor(actor);
  //ren->AddActor(actor2);
  ren2->AddActor(actor2);

  //-------renderers windows-------
  vtkRenderWindow *renwin = vtkRenderWindow::New();
  renwin->SetSize(768, 768);
  renwin->AddRenderer(ren);
  set2Windows(ren,ren2,renwin);

  //-------renderers windows interactors-------
  //setInteractor(renwin);
  setAnimator(renwin,input,plane,ren,actor2);

  renwin->Render();
}
