
#include <iostream>
#include <fstream>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSetReader.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include <vtkRectilinearGrid.h>
#include <vtkFloatArray.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
#include <vtkPointData.h>
#include <vtkGraphicsFactory.h>
//#include <vtkImagingFactory.h>
#include <vtkPNGWriter.h>
#include <vtkImageData.h>
#include <vtkCamera.h>
#include <vtkProperty.h>

#include "helpers.h"
#include "Config.h"


#define STR_VALUE(arg)      #arg
#define FUNCTION_NAME(name) STR_VALUE(name)
#define TAILLE_NAME FUNCTION_NAME(TAILLE)

//ICI PLACER LA TAILLE DU FICHIER 512 ou 1024
#define TAILLE 512



#define FICHIER MY_MESHES_PATH "/sn_resamp" TAILLE_NAME

int gridSize = TAILLE;
const char *location = FICHIER;
using std::cerr;
using std::endl;

// Function prototypes
vtkRectilinearGrid  *ReadGrid(int zStart, int zEnd);

bool PositiveSideOfPlane(const double *origin, const double *normal,
                         const double *point)
{
    double sum = normal[0]*(origin[0]-point[0])
               + normal[1]*(origin[1]-point[1])
               + normal[2]*(origin[2]-point[2]);
    return (sum < 0.);
}

bool intersectPlane(const double *origin, const double *normal,const int slice){
  double  point[3];
  point[0]=0;
  point[1]=0;
  point[2]=slice/(gridSize-1.0);
  bool side=PositiveSideOfPlane(origin,normal,point);

  point[1]=1;
  if( side!=PositiveSideOfPlane(origin,normal,point)){
    return true;
  }

  point[0]=1;
  if( side!=PositiveSideOfPlane(origin,normal,point)){
    return true;
  }

  point[1]=0;
  if( side!=PositiveSideOfPlane(origin,normal,point)){
    return true;
  }

  return false;
}

void
GetRelevantZRange(const double *origin, const double *normal, int *range){
    // The data goes from 0 to 1 in X, Y, and Z.
    // There are gridSize points in each direction.
    // The i^{th} z-plane is located at i/(gridSize-1.0).
    //range[0] = 100;
    //range[1] = gridSize*0.56-1;

    range[0] = 1;
    range[1] = gridSize - 1;
    bool debut = false;

    for(int i=0;i<gridSize;i++){
      if (!debut && intersectPlane(origin, normal,(double)i)){
        printf("debut en : %d",i);
        range[0]=i;
        debut=true;
      }else if(debut && !intersectPlane(origin, normal,(double)i)){
        printf("fin en : %d",i);
        range[1]=i;
        break;
      }
    }
}

vtkRectilinearGrid *
ReadRelevantGrid(const double *origin, const double *normal)
{
    int range[2];
    GetRelevantZRange(origin, normal, range);
  cerr << "Decided on " << range[0] << "-" << range[1] << endl;

    return ReadGrid(range[0], range[1]);
}


int main(int argc, char *argv[]){
  printf("coucou :-)\n");
  GetMemorySize("initialization");
    int t1;
    t1 = timer->StartTimer();

   // Correct answer is 176/335.
   double origin[3] = { 0.5, 0.5, 0.5 };
   double normal[3] = { 0.1, 0.2, 0.974679 };

   // Read the data.
    GetMemorySize("before read");
   vtkRectilinearGrid *rg = ReadRelevantGrid(origin, normal);
  GetMemorySize("after read");
   vtkCutter *cutter = vtkCutter::New();
   vtkPlane *plane = vtkPlane::New();
   plane->SetNormal(normal);
   plane->SetOrigin(origin);
   cutter->SetCutFunction(plane);
   cutter->SetInputData(rg);

   vtkDataSetMapper *mapper = vtkDataSetMapper::New();
   mapper->SetInputConnection(cutter->GetOutputPort());

   vtkLookupTable *lut = vtkLookupTable::New();
   mapper->SetLookupTable(lut);
   mapper->SetScalarRange(0,15);
   lut->Build();

   vtkActor *actor = vtkActor::New();
   actor->SetMapper(mapper);
//actor->GetProperty()->SetRepresentationToWireframe();
   vtkRenderer *ren = vtkRenderer::New();
   ren->AddActor(actor);

   vtkCamera *cam = ren->GetActiveCamera();
   cam->SetFocalPoint(0.5, 0.5, 0.5);
   cam->SetPosition(1.5, 1.5, 1.5);

   vtkRenderWindow *renwin = vtkRenderWindow::New();
   renwin->SetSize(768, 768);
   renwin->AddRenderer(ren);


   vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
   iren->SetRenderWindow(renwin);
   renwin->Render();
    GetMemorySize("end");
    timer->StopTimer(t1,"time");
   iren->Start();
}


// You should not need to modify these routines.
vtkRectilinearGrid *
ReadGrid(int zStart, int zEnd)
{
    int  i;

    if (zStart < 0 || zEnd < 0 || zStart >= gridSize || zEnd >= gridSize || zStart > zEnd)
    {
        cerr << location << "  Invalid range: " << zStart << "-" << zEnd << endl;
        return NULL;
    }

    ifstream ifile(location);
    if (ifile.fail())
    {
        cerr << location << "  Unable to open file: " << location << "!!" << endl;
    }

    cerr << location << "  Reading from " << zStart << " to " << zEnd << endl;

    vtkRectilinearGrid *rg = vtkRectilinearGrid::New();
    vtkFloatArray *X = vtkFloatArray::New();
    X->SetNumberOfTuples(gridSize);
    for (i = 0 ; i < gridSize ; i++)
        X->SetTuple1(i, i/(gridSize-1.0));
    rg->SetXCoordinates(X);
    X->Delete();
    vtkFloatArray *Y = vtkFloatArray::New();
    Y->SetNumberOfTuples(gridSize);
    for (i = 0 ; i < gridSize ; i++)
        Y->SetTuple1(i, i/(gridSize-1.0));
    rg->SetYCoordinates(Y);
    Y->Delete();
    vtkFloatArray *Z = vtkFloatArray::New();
    int numSlicesToRead = zEnd-zStart+1;
    Z->SetNumberOfTuples(numSlicesToRead);
    for (i = zStart ; i <= zEnd ; i++)
        Z->SetTuple1(i-zStart, i/(gridSize-1.0));
    rg->SetZCoordinates(Z);
    Z->Delete();

    rg->SetDimensions(gridSize, gridSize, numSlicesToRead);

    int valuesPerSlice  = gridSize*gridSize;
    int bytesPerSlice   = 4*valuesPerSlice;

#if TAILLE == 512
    unsigned int offset          = (unsigned int)zStart * (unsigned int)bytesPerSlice;
    unsigned int bytesToRead     = bytesPerSlice*numSlicesToRead;
    unsigned int valuesToRead    = valuesPerSlice*numSlicesToRead;
#elif TAILLE == 1024
    unsigned long long offset          = (unsigned long long)zStart * bytesPerSlice;
    unsigned long long  bytesToRead     = (unsigned long long )bytesPerSlice*numSlicesToRead;
    unsigned int valuesToRead    = (unsigned int )valuesPerSlice*numSlicesToRead;
#else
#error Unsupported choice setting
#endif

    vtkFloatArray *scalars = vtkFloatArray::New();
    scalars->SetNumberOfTuples(valuesToRead);
    float *arr = scalars->GetPointer(0);
    ifile.seekg(offset, ios::beg);
    ifile.read((char *)arr, bytesToRead);
    ifile.close();

    scalars->SetName("entropy");
    rg->GetPointData()->AddArray(scalars);
    scalars->Delete();

/*
    vtkFloatArray *pr = vtkFloatArray::New();
    pr->SetNumberOfTuples(valuesToRead);
    for (int i = 0 ; i < valuesToRead ; i++)
        pr->SetTuple1(i, parRank);

    pr->SetName("par_rank");
    rg->GetPointData()->AddArray(pr);
    pr->Delete();
 */

    rg->GetPointData()->SetActiveScalars("entropy");

    cerr << location << "  Done reading" << endl;
    return rg;
}

void
WriteImage(const char *name, const float *rgba, int width, int height)
{
    vtkImageData *img = vtkImageData::New();
    img->SetDimensions(width, height, 1);
#if VTK_MAJOR_VERSION <= 5
    img->SetNumberOfScalarComponents(3);
    img->SetScalarTypeToUnsignedChar();
#else
    img->AllocateScalars(VTK_UNSIGNED_CHAR,3);
#endif

    for (int i = 0 ; i < width ; i++)
        for (int j = 0 ; j < height ; j++)
        {
            unsigned char *ptr = (unsigned char *) img->GetScalarPointer(i, j, 0);
            int idx = j*width + i;
            ptr[0] = (unsigned char) (255*rgba[4*idx + 0]);
            ptr[1] = (unsigned char) (255*rgba[4*idx + 1]);
            ptr[2] = (unsigned char) (255*rgba[4*idx + 2]);
        }


    vtkPNGWriter *writer = vtkPNGWriter::New();
    writer->SetInputData(img);
    writer->SetFileName(name);
    writer->Write();

    img->Delete();
    writer->Delete();
}

// - 13h 30
// sudo apt install openmpi-bin openmpi-common
