
#include <iostream>
#include <fstream>
#include <string>
#include <mpi.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSetReader.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include <vtkRectilinearGrid.h>
#include <vtkFloatArray.h>
#include <vtkContourFilter.h>
#include <vtkPointData.h>
#include <vtkGraphicsFactory.h>
//#include <vtkImagingFactory.h>
#include <vtkPNGWriter.h>
#include <vtkImageData.h>
#include <vtkCamera.h>
#include <unistd.h>

#include "Config.h"
#include "helpers.h"
#include <mpi.h>

#define STR_VALUE(arg)      #arg
#define FUNCTION_NAME(name) STR_VALUE(name)
#define TAILLE_NAME FUNCTION_NAME(TAILLE)

//ICI PLACER LA TAILLE DU FICHIER 512 ou 1024
#define TAILLE 512



#define FICHIER MY_MESHES_PATH "/sn_resamp" TAILLE_NAME


int winSize = 768;

const char *prefix = "";


int gridSize = TAILLE;
const char *location = FICHIER ;





int parRank = 0;
int parSize = 1;

using std::cerr;
using std::endl;

// Function prototypes
vtkRectilinearGrid  *ReadGrid(int zStart, int zEnd);
void                 WriteImage(const char *name, const float *rgba, int width, int height);


vtkRectilinearGrid *
ParallelReadGrid(void)
{
  //  int zStart = (gridSize/1.7)-1;
//    int zEnd = (gridSize/1.5)-1;

  int step = gridSize/parSize;

  int zStart = parRank*step;
  int zEnd = (parRank+1)*step;

  if(parRank==parSize-1){
    zEnd += (gridSize%parSize)-1 ;
  }

  // if you don't want any data for this processor, just do this:
  //    return NULL;

  return ReadGrid(zStart, zEnd);
}

bool
CompositeImage(float *rgba_in, float *zbuffer, float *rgba_out,
               int image_width, int image_height)
{

  int npixels = image_width*image_height;

  float *zbuffer_min = new float[npixels];
  float *rgba_tmp = new float[4*npixels];
  MPI_Allreduce(zbuffer, zbuffer_min,npixels,MPI_FLOAT, MPI_MIN, MPI_COMM_WORLD);

  for (int i = 0 ; i < npixels ; i++){
    //todo meme technique out of corergba
    if(zbuffer[i]==zbuffer_min[i]){
      rgba_tmp[4*i] = rgba_in[4*i];
      rgba_tmp[4*i+1] = rgba_in[4*i+1];
      rgba_tmp[4*i+1] = rgba_in[4*i+1];
      rgba_tmp[4*i+1] = rgba_in[4*i+1];
    }else{
      rgba_tmp[4*i] = 0.;
      rgba_tmp[4*i+1] = 0.;
      rgba_tmp[4*i+1] = 0.;
      rgba_tmp[4*i+1] = 0.;
    }
  }

  for (int i = 0 ; i < npixels*4 ; i++){
    //todo meme technique out of corergba
    //bool didComposite = ComposeImageZbuffer(rgba_out, zbuffer_min,  image_width, image_height);
    rgba_out[i] = rgba_tmp[i];
  }

  MPI_Reduce(rgba_tmp, rgba_out,4*npixels,MPI_FLOAT,MPI_MAX,0, MPI_COMM_WORLD);
  return true;
}

int main(int argc, char *argv[])
{
   // MPI setup
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &parRank);
   MPI_Comm_size(MPI_COMM_WORLD, &parSize);

   // I use the variable "prefix" to get the print statements right.
    std::string p(std::to_string(parRank));
    int t1;
    t1 = timer->StartTimer();


   prefix = p.c_str();

    GetMemorySize((p+":initialization").c_str());


   // Read the data.
   vtkRectilinearGrid *rg = ParallelReadGrid();
    GetMemorySize((p+":After Read").c_str());

   // Contour the data.
   vtkContourFilter *cf = vtkContourFilter::New();
   cf->SetNumberOfContours(1);
   cf->SetValue(0, 5.0);
   cf->SetInputData(rg);

   // Force an update and set the parallel rank as the active scalars.
   cf->Update();
   cf->GetOutput()->GetPointData()->SetActiveScalars("par_rank");

   vtkDataSetMapper *mapper = vtkDataSetMapper::New();
   mapper->SetInputConnection(cf->GetOutputPort());

   vtkLookupTable *lut = vtkLookupTable::New();
   mapper->SetLookupTable(lut);
   mapper->SetScalarRange(0,parSize-1.0);
   lut->Build();

   vtkActor *actor = vtkActor::New();
   actor->SetMapper(mapper);

   vtkRenderer *ren = vtkRenderer::New();
   ren->AddActor(actor);

   vtkCamera *cam = ren->GetActiveCamera();
   cam->SetFocalPoint(0.5, 0.5, 0.5);
   cam->SetPosition(1.5, 1.5, 1.5);

   vtkRenderWindow *renwin = vtkRenderWindow::New();
   // THIS DOESN'T WORK WITHOUT MESA
    renwin->OffScreenRenderingOn();
   renwin->SetSize(winSize, winSize);
   renwin->AddRenderer(ren);


  //WE'RE JUST RENDERING A SINGLE IMAGE ... NO INTERACTOR
/*   vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
   iren->SetRenderWindow(renwin);
   renwin->Render();
   iren->Start();
 */

   float *rgba, *zbuffer;
   bool staggerGLRenders = false;
//   bool staggerGLRenders = true;

   if (staggerGLRenders)
   {
       for (int i = 0 ; i < parSize ; i++)
       {
           MPI_Barrier(MPI_COMM_WORLD);
           if (i == parRank)
           {
               renwin->Render();
               sleep(1);
               rgba = renwin->GetRGBAPixelData(0, 0, (winSize-1), (winSize-1), 1);
               zbuffer = renwin->GetZbufferData(0, 0, (winSize-1), (winSize-1));
           }
       }
   }
   else
   {
       renwin->Render();
       rgba = renwin->GetRGBAPixelData(0, 0, (winSize-1), (winSize-1), 1);
       zbuffer = renwin->GetZbufferData(0, 0, (winSize-1), (winSize-1));
   }

   float *new_rgba = new float[4*winSize*winSize];
   bool didComposite = CompositeImage(rgba, zbuffer, new_rgba, winSize, winSize);
	if (didComposite)
	{
		if (parRank == 0)
		{
			WriteImage("final_image.png", new_rgba, winSize, winSize);
		}

		char name[128];
		sprintf(name, "img%d.png", parRank);
		WriteImage(name, rgba, winSize, winSize);

	}

renwin->Delete();
rg->Delete();
GetMemorySize((p+":End").c_str());
    timer->StopTimer(t1,(p+":Time").c_str());
   MPI_Finalize();



}


// You should not need to modify these routines.


// You should not need to modify these routines.
vtkRectilinearGrid *
ReadGrid(int zStart, int zEnd)
{
    int  i;
    if (zStart < 0 || zEnd < 0 || zStart >= gridSize || zEnd >= gridSize || zStart > zEnd)
    {
        cerr << prefix << "Invalid range: " << zStart << "-" << zEnd << endl;
        return NULL;
    }

    ifstream ifile(location);
    if (ifile.fail())
    {
        cerr << prefix << "Unable to open file: " << location << "!!" << endl;
    }

    cerr << prefix << "Reading "<< location << " from " << zStart << " to " << zEnd << endl;

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


    vtkFloatArray *pr = vtkFloatArray::New();
    pr->SetNumberOfTuples(valuesToRead);
    for (int i = 0 ; i < valuesToRead ; i++)
        pr->SetTuple1(i, parRank);

    pr->SetName("par_rank");
    rg->GetPointData()->AddArray(pr);
    pr->Delete();

    rg->GetPointData()->SetActiveScalars("entropy");

    cerr << prefix << " Done reading" << endl;
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

// comme on le souhaite -> proposer un devoir et vous me le rendez ( tp ) rdv pour le faire, ouvre et 2h après on le rend -> voir avec lui pour
// le mettre sur edt -> ce qu'on a fait mais sur un sujet.
