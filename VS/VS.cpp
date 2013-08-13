#include "GridModel.h"
#include "GLContext.h"
#include <time.h>
#include "KinectTool.h"
#include "TriangleMesh.h"
#include "Soundify.h"

inline double diffclock( clock_t clock1, clock_t clock2 ) 
{
	double diffticks = clock1 - clock2;
    double diffms    = diffticks / ( CLOCKS_PER_SEC / 1000 );

    return diffms;
}

int main( int argc, UINT8** argv) 
{
	GLContext* cntx = new GLContext();//Window+render.
	Input* inp = new Input();//Input system.
	cntx->SetInput( inp );//Context redirects mouse+keyb to Input

	unsigned int power = 8;

	GridModel* model = new GridModel(power);//power of 2
	unsigned int side = model->GetDimm();
	inp->SetZoom(-(side*1.5f));
	inp->SetModel( model );

	KinectTool* tool = new KinectTool( (side*0.75f), (side*0.75f), side*.75f, -(side*.75f));

	Soundify snd;
	snd.Play();

	int acted = 0;

	while (cntx->alive())
	{	
		clock_t start = clock();
		acted = 0;

		inp->UpdateFrame();//Reset frame variables.
		cntx->doMessage();//Win message loop


		tool->DoToolUpdate();
		acted = tool->InteractModel( model, inp->GetObjectQ());
		
		model->UpdateGrid();
		cntx->renderScene(model, tool, inp->GetViewM(), inp->GetObjectM());
		
		if (acted)
		{
			snd.SetGain(1.0f);
			snd.SetPitch( acted/1000.0f );
		}
		else
		{
			snd.SetGain(0.0f);
		}

		clock_t end = clock();
		std::cout<<"Frame time = " << diffclock( end, start )<< " ms, " << "Interacted: "<< acted << std::endl;
	}

	snd.SetGain(0.0f);

	delete model;
	delete cntx;
	delete inp;
	delete tool;
	return 0;
}

